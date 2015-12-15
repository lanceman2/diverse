/**************************************************************************
DIVERSE DADS v1.0
released: April 7th
The DIVERSE HOMEPAGE is at: diverse.sourceforge.net

Copyright (C) 2000 - 2004  Andrew A. Ray, Patrick Shinpaugh, and Ron Kriz

This is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (GPL) as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This software is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software, in the top level source directory in a file
named "COPYING.GPL"; if not, see it at:
http://www.gnu.org/copyleft/gpl.html or write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA.
**************************************************************************/
//Navigation writer DSO for the DADS cluster
//Written 11/21/03 
//By Andrew A. Ray and Patrick Shinpaugh
#include <stdio.h>
#include <arpa/inet.h>
#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <vector>
#include <string>
#include <map>
#include <dSocket.h>
#include <errno.h>

#define DADS_DEFAULT_TIMEOUT 20

using namespace std;

class navWrite : public dtkAugment
{
public:

	navWrite(dtkManager *app) ;
	~navWrite(void) ;
	int postConfig(void);
	int postFrame(void);
	void getHosts();
	void getSyncSegments();
	void getTimeout();

private:
	dtkNavList*   m_navlist;
	vector<string> m_hosts;
	map<int,string> m_status;
	dtkSharedMem* m_head;
	dtkSharedMem* m_wand;
	dtkSharedMem* m_joystick;
	dtkSharedMem* m_buttons;
	dtkNav* m_nav;
	dtkManager*  m_manager;
	dSocket* m_serversock;
	map<int,dSocket*> m_clientsock;
	fd_set m_master;
	int m_fdmax;
	int m_timeout;
	vector<string> m_syncsegmentnames;
	vector<dtkSharedMem*> m_syncsegments;
    char* m_values;
    int m_valuessize;
};

// create object and set description.
// be sure to validate it!
navWrite::navWrite(dtkManager *app) : dtkAugment("navWrite")
{
	m_timeout = DADS_DEFAULT_TIMEOUT;
	getTimeout();
	getHosts();
	m_head = new dtkSharedMem( "head", 0 );
	if( !m_head->isValid() )
	{
		delete m_head;
		m_head = new dtkSharedMem( sizeof(float)*6, "head" );
	}
	m_wand = new dtkSharedMem( "wand", 0 );
	if( !m_wand->isValid() )
	{
		delete m_wand;
		m_wand = new dtkSharedMem( sizeof(float)*6, "wand" );
	}
	m_joystick = new dtkSharedMem( "joystick", 0 );
	if( !m_joystick->isValid() )
	{
		delete m_joystick;
		m_joystick = new dtkSharedMem( sizeof(float)*2, "joystick" );
	}
	m_buttons = new dtkSharedMem( "buttons", 0 );
	if( !m_buttons->isValid() )
	{
		delete m_buttons;
		m_buttons = new dtkSharedMem( sizeof(unsigned char), "buttons" );
	}
	getSyncSegments();
	for( int i=0;i<(int)m_syncsegmentnames.size();i++ )
	{
		dtkSharedMem* sharedmem = new dtkSharedMem( m_syncsegmentnames[i].c_str() );
		if( !sharedmem->isValid() )
		{
			printf( "\n\nERROR: the sync segment %s does not exist - \n"
					"all sync segments should be created prior to\n"
					"the call to navWrite.\n\n",
					m_syncsegmentnames[i].c_str() );
			fflush( stdout );
			delete sharedmem;
		}
		else
			m_syncsegments.push_back( sharedmem );
	}
	char hostname[256];
	gethostname( hostname, 255 );
	if( !hostname )
	{
		printf( "ERROR - Unable to get the server hostname. "
				"Exiting!\n" );
		fflush( stdout );
		exit( 1 );
	}
	m_serversock = new dSocket( 20000 );
	m_serversock->bindSocket();
	m_serversock->listenSocket();
	m_manager = app;
	FD_ZERO( &m_master );
    if( !( m_values = (char*)realloc( 0, 1024 ) ) )
    {
        printf( "ERROR: Unable to allocate buffer\n" );
		fflush( stdout );
        exit( 1 );
    }
    m_valuessize = 1024;
	setDescription("Nav writer DSO for DADS cluster");
	validate();
}

// destructor
navWrite::~navWrite(void) 
{
	if( m_head )
		delete m_head;
	if( m_wand )
		delete m_wand;
	if( m_joystick )
		delete m_joystick;
	if( m_buttons )
		delete m_buttons;
	for( int i=0;i<(int)m_syncsegments.size();i++ )
	{
		delete m_syncsegments[i];
	}
	if( m_serversock )
		delete m_serversock;
	if( m_clientsock.size() )
	{
		map<int,dSocket*>::iterator itr;
		for( itr=m_clientsock.begin();itr!=m_clientsock.end();itr++ )
			delete itr->second;
	}
    if( m_values )
        free( m_values );
}

void navWrite::getHosts()
{
	char* hosts = getenv( "DADS_CLIENTS" );
	if( hosts != NULL )
	{
		char* host = strtok( hosts, ":" );
		while ( host )
		{
			m_hosts.push_back( string(host) );
			host = strtok( NULL, ":" );
		}
	}
}

void navWrite::getSyncSegments()
{
	char* syncsegs = getenv( "DADS_SYNC_SEGMENTS" );
	if( syncsegs != NULL )
	{
		char* seg = strtok( syncsegs, ":" );
		while( seg )
		{
			string segname = string(seg);
			if( segname == "head" || segname == "wand" ||
					segname == "joystick" || segname == "buttons" )
			{
				printf( "WARNING - environment variable"
						"DADS_SYNC_SEGMENTS should not\n"
						"contain the standard sync segments head,\n"
						"wand, joystick, or buttons as they are.\n"
						"automatically synced.\n"
						"Segment %s ignored.\n", segname.c_str() );
				fflush( stdout );
			}
			else
				m_syncsegmentnames.push_back( segname );
			seg = strtok( NULL, ":" );
		}
	}
}

void navWrite::getTimeout()
{
	char* timeoutenv = getenv( "DADS_TIMEOUT" );
	if( timeoutenv == '\0' )
		return;
	errno = 0;
	long timeout = strtol( timeoutenv, NULL, 10 );
	if( errno == ERANGE || errno == EINVAL )
	{
		printf( "ERROR: DADS_TIMEOUT contains an invalid value\n"
				"Using timeout default value: %d\n", DADS_DEFAULT_TIMEOUT );
		return;
	}
	m_timeout = timeout;
}

// called only once by dpf, after pfConfig() and dpfDisplay:postConfig()
// pfConfig()
int navWrite::postConfig(void) 
{
	FD_SET( m_serversock->getfd(), &m_master );
	m_fdmax = m_serversock->getfd();
	printf( "server fd: %d\n", m_serversock->getfd() );
	fflush( stdout );
	dSocket* client = NULL;
	timeval timeout;
	timeout.tv_sec = m_timeout;
	timeout.tv_usec = 0;
	fd_set read_fds;
	while( m_clientsock.size() < m_hosts.size() )
	{
		read_fds = m_master;
		int sets = select( m_fdmax+1, &read_fds, NULL, NULL, &timeout );
		if( sets == -1 )
		{
			perror( "select" );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
			exit(1);
		}
		else if( !sets )
		{
			printf( "ERROR: Timed out communicating with clients!\n" );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
		client = m_serversock->acceptSocket();
		if( client )
		{
			printf( "Accepted connection with %s\n", client->getAddress().c_str() );
			fflush( stdout );
			string message;
			if( m_syncsegments.size() )
			{
				message = "ADD_SEGMENTS";
				if( !client->writeSocket( message.c_str(), message.size() ) )
				{
					printf( "ERROR: Unable to write to socket\n" );
					fflush( stdout );
					m_manager->state &= ~(DTK_ISRUNNING);
					return DTK_CALLBACK_ERROR;
				}
				if( !client->readSocket() )
				{
					printf( "ERROR: Unable to read from socket\n" );
					fflush( stdout );
					m_manager->state &= ~(DTK_ISRUNNING);
					return DTK_CALLBACK_ERROR;
				}
				if( string( (char*)client->getBuffer() ) != "READY" )
				{
					printf( "ERROR - communication problem with client\n" );
					fflush( stdout );
					exit(1);
				}
				message.clear();
				for( int i=0;i<(int)m_syncsegments.size();i++ )
				{
					char temp[64];
					sprintf( temp, "%ld", m_syncsegments[i]->getSize() );
					message += string( m_syncsegments[i]->getName() ) +
						string( "," ) + string( temp );
					if( i < (int)m_syncsegments.size() - 1 )
						message += ":";
					else
						message += "\0";
				}
			}
			else
				message = "WAIT";
			if( !client->writeSocket( message.c_str(), message.size() ) )
			{
				printf( "ERROR: Unable to write to socket\n" );
				fflush( stdout );
				m_manager->state &= ~(DTK_ISRUNNING);
				return DTK_CALLBACK_ERROR;
			}
			while( string( (char*)client->getBuffer() ) != "WAITING" )
			{
				if( !client->readSocket() )
				{
					printf( "ERROR: Unable to read from socket\n" );
					fflush( stdout );
					m_manager->state &= ~(DTK_ISRUNNING);
					return DTK_CALLBACK_ERROR;
				}
			}
			m_status[client->getfd()] = "WAIT";
			m_clientsock[client->getfd()] = client;
			FD_SET( client->getfd(), &m_master );
			if( client->getfd() > m_fdmax )
				m_fdmax = client->getfd();
			printf( "client fd: %d\n", client->getfd() );
			fflush( stdout );
			client = NULL;
		}
	}
	// This usleep is to prevent concatenation of previous
	// writeSocket call with following writeSocket call.
	usleep( 1000 );
	map<int,dSocket*>::iterator itr;
	for( itr=m_clientsock.begin();itr!=m_clientsock.end();itr++ )
	{
		printf( "GO\n" );
		fflush( stdout );
		string message = "GO";
		if( !itr->second->writeSocket( message.c_str(), message.size() ) )
		{
			printf( "ERROR: communicating with %s\n",
					itr->second->getName().c_str() );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
		m_status[itr->second->getfd()] = "GO";
	}
	m_navlist = (dtkNavList*)m_manager->check("dtkNavList", DTKNAVLIST_TYPE);
	if (m_navlist == NULL)
	{
		printf("Error:  Navlist is NULL\n");
		fflush( stdout );
		exit(1);
	}
	m_nav = m_navlist->current();
	if (m_nav== NULL)
	{
		printf("Error:  Nav is NULL\n");
		fflush( stdout );
		exit(1);
	}
	return DTK_CONTINUE;
}

// called every display loop, as a pfFrame callback, after pfDraw()
// this return value causes it to only be called once, and the object removed
int navWrite::postFrame(void) 
{
	m_nav = m_navlist->current();
	unsigned int clients = 0;
	timeval timeout;
	timeout.tv_sec = 20;
	timeout.tv_usec = 0;
	fd_set read_fds;
	map<int,dSocket*>::iterator itr;
	while( clients < m_hosts.size() )
	{
		read_fds = m_master;
		int sets = select( m_fdmax+1, &read_fds, NULL, NULL, &timeout );
		if( sets == -1 )
		{
			perror( "select" );
			exit(1);
		}
		else if( !sets )
		{
			printf( "ERROR: Timed out communicating with clients!\n" );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
		for( itr=m_clientsock.begin();itr!=m_clientsock.end();itr++ )
		{
			if( FD_ISSET( itr->first, &read_fds ) )
			{
				if( itr->first == m_serversock->getfd() )
				{
					dSocket* client = m_serversock->acceptSocket();
					printf( "Attempt to connect from %s\n",
							client->getName().c_str() );
					fflush( stdout );
					delete client;
				}
				else
				{
					itr->second->readSocket();
					if( string((char*)itr->second->getBuffer()) == "READY" )
					{
						m_status[itr->first] = "READY";
						clients++;
					}
				}
			}
		}
	}
	float* loc = m_nav->location.d;
	int valuesize = 0;
	char* temp = m_values;
	memcpy( temp, loc, 24 );
	temp += 24;
	valuesize += 24;
	m_head->read( temp );
	temp += 24;
	valuesize += 24;
	m_wand->read( temp );
	temp += 24;
	valuesize += 24;
	m_joystick->read( temp );
	temp += 8;
	valuesize += 8;
	m_buttons->read( temp );
	temp += 4;
	valuesize += 4;
	for( int i=0;i<(int)m_syncsegments.size();i++ )
	{

		int size = m_syncsegments[i]->getSize();
		if( valuesize + size + 1 > m_valuessize )
		{
            if( !( m_values = (char*)realloc( m_values, valuesize + size  + 1 ) ) )
            {
                printf( "ERROR: Unable to resize buffer\n" );
			    fflush( stdout );
                free( m_values );
                m_values = 0;
                m_valuessize = 0;
			    exit(1);
            }
            temp = m_values + valuesize;
            m_valuessize = valuesize + size + 1;
		}
		m_syncsegments[i]->read( temp );
		temp += size;
		valuesize += size;
	}
	temp = '\0';
	valuesize += 1;
	for( itr=m_clientsock.begin();itr!=m_clientsock.end();itr++ )
	{
		if( !itr->second->writeSocket( m_values, valuesize ) )
		{
			printf( "ERROR: Unable to write to socket\n" );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
		m_status[itr->first] = "GO";
	}
	return DTK_CONTINUE ; 
}

/*************** DTK C++ DSO loader/unloader functions ***********/
/*
 * All DTK DSO files are required to declare these two functions.
 * These function are called by the loading program to get your
 * C++ objects loaded.
 *
 *****************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
	return new navWrite(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
	delete augment;
	return DTKDSO_UNLOAD_CONTINUE;
}
