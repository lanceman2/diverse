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
//Navigation reader DSO for the DADS cluster
//Written 11/21/03 
//By Andrew A. Ray and Patrick Shinpaugh
#include <stdio.h>
#include <dtk.h>
#include <unistd.h>
#include <stdlib.h>
#include <dtk/dtkDSO_loader.h>
#include <string>
#include <X11/Xlib.h>
#include <dSocket.h>
#include <vector>

using namespace std;

class navRead : public dtkNav
{
public:

	navRead(dtkManager* m);
	~navRead(void) ;
	int postConfig(void);
	int postFrame(void);

private:
	float*        m_tempPos;
	dtkManager*   m_manager;
	dtkNavList*   m_navlist;
	string        m_server;
	string        m_client;
	dtkNav*       m_nav;
	dtkSharedMem* m_head;
	dtkSharedMem* m_wand;
	dtkSharedMem* m_joystick;
	dtkSharedMem* m_buttons;
	string m_status;
	dSocket* m_socket;
	char* m_values;
	vector<dtkSharedMem*> m_syncsegments;
};

// create object and set description.
// be sure to validate it!
navRead::navRead(dtkManager *m):dtkNav(m, "navRead")
{
	m_manager = m;
	m_tempPos = new float[6];
	if (m_tempPos == NULL)
	{
		printf("Error could not allocate array\n");
		fflush( stdout );
		exit(1);
	}
	char* server = getenv( "DADS_SERVER" );
	if ( !server )
	{
		printf( "ERROR - the environment variable DADS_SERVER "
				"was not set. Unloading navRead DSO...\n" );
		fflush( stdout );
		return ;
	}
	m_server = server;
	char client[256];
	gethostname( client, 255 );
	if ( !client )
	{
		printf( "ERROR - unable to retrieve the hostname. "
				"Unloading navRead DSO...\n" );
		fflush( stdout );
	}
	m_client = client;
	m_head = new dtkSharedMem( sizeof(float) * 6, "head" );
	m_wand = new dtkSharedMem( sizeof(float) * 6, "wand" );
	m_joystick = new dtkSharedMem( sizeof(float) * 2, "joystick" );
	m_buttons = new dtkSharedMem( sizeof(char), "buttons" );
	setDescription("Nav reader DSO for DADS cluster");
	validate();
}

// destructor
navRead::~navRead(void) 
{
	if( m_socket )
		delete m_socket;
	if ( m_head )
		delete m_head;
	if ( m_wand )
		delete m_wand;
	if ( m_joystick )
		delete m_joystick;
	if ( m_buttons )
		delete m_buttons;
}

// called only once by dpf/dgl
int navRead::postConfig(void) 
{
	m_socket = new dSocket( 20000, m_server.c_str() );
	m_status = "NONE";
	m_values = 0;
	if( !m_socket->connectSocket() )
	{
		printf( "ERROR: Unable to connect to listener\n" );
		fflush( stdout );
		m_manager->state &= ~(DTK_ISRUNNING);
		return DTK_CALLBACK_ERROR;
	}
	if( !m_socket->readSocket() )
	{
		printf( "ERROR: Unable to read from socket\n" );
		fflush( stdout );
		m_manager->state &= ~(DTK_ISRUNNING);
		return DTK_CALLBACK_ERROR;
	}
	string message;
	if( string((char*)m_socket->getBuffer()) == "ADD_SEGMENTS" )
	{
		message = "READY";
		if( !m_socket->writeSocket( message.c_str(), message.size() ) )
		{
			printf( "ERROR: Unable to write to socket\n" );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
		if( !m_socket->readSocket() )
		{
			printf( "ERROR: Unable to read from socket\n" );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
		printf( "Received a sync segment list - setting up sync segments\n" );
		fflush( stdout );
		m_values = (char*)m_socket->getBuffer();
		char* seg = strtok( (char*)m_values, ":" );
		while( seg )
		{
			string segment = string( seg );
			int place = segment.find( ",", 0 );
			string segname = segment.substr( 0, place );
			int segsize = atoi( segment.substr( place + 1,
					segment.size() - place - 1 ).c_str() );
			dtkSharedMem* shmseg = new dtkSharedMem( segsize, segname.c_str() );
			if( shmseg )
			{
				m_syncsegments.push_back( shmseg );
				printf( "    Added segment %s\n", segname.c_str() );
				fflush( stdout );
			}
			else
			{
				printf( "ERROR - Unable to allocate shared memory segment %s\n",
						segment.c_str() );
				fflush( stdout );
			}
			seg = strtok( NULL, ":" );
		}
		message = "WAITING";
		if( !m_socket->writeSocket( message.c_str(), message.size() ) )
		{
			printf( "ERROR: Unable to write to socket\n" );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
	}
	else if( string((char*)m_socket->getBuffer()) == "WAIT" )
	{
		message = "WAITING";
		if( !m_socket->writeSocket( message.c_str(), message.size() ) )
		{
			printf( "ERROR: Unable to write to socket\n" );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
	}	
	else
	{
		printf( "ERROR - expected WAIT from server - EXITING!!!\n" );
		fflush( stdout );
		exit(1);
	}
	if( !m_socket->readSocket() )
	{
		printf( "ERROR: Unable to read from socket\n" );
		fflush( stdout );
		m_manager->state &= ~(DTK_ISRUNNING);
		return DTK_CALLBACK_ERROR;
	}
	m_values = (char*)m_socket->getBuffer();
	if( string((char*)m_values) != "WAIT" )
	m_status = "WAIT";
	printf( "WAITING...\n" );
	fflush( stdout );
	while( string( (char*)m_socket->getBuffer() ) != "GO" )
	{
		printf( "Waiting for GO\n" );
		if( !m_socket->readSocket() )
		{
			printf( "ERROR: Unable to read from socket\n" );
			fflush( stdout );
			m_manager->state &= ~(DTK_ISRUNNING);
			return DTK_CALLBACK_ERROR;
		}
	}
	m_status = "GO";
	printf( "Received GO - postConfig complete!\n" );
	return dtkNav::postConfig();
}

// called every display loop, as a pfFrame callback, after pfDraw()
// this return value causes it to only be called once, and the object removed
int navRead::postFrame(void) 
{
	m_status = "READY";
	if( !m_socket->writeSocket( m_status.c_str(), m_status.size() ) )
	{
		printf( "ERROR: Unable to write to socket\n" );
		fflush( stdout );
		m_manager->state &= ~(DTK_ISRUNNING);
		return DTK_CALLBACK_ERROR;
	}
	if( !m_socket->readSocket() )
	{
        if( m_socket->isClosed() )
        {
            printf( "Exiting\n" );
            fflush( stdout );
            m_manager->state &= ~(DTK_ISRUNNING);
            return dtkNav::postFrame();
        }
		printf( "ERROR: Unable to read from socket\n" );
		fflush( stdout );
		m_manager->state &= ~(DTK_ISRUNNING);
		return DTK_CALLBACK_ERROR;
	}
	m_values = (char*)m_socket->getBuffer();
	char* temp = (char*)m_values;
	this->location.set( (float*)temp );
	temp += 24;
	m_head->write( temp );
	temp += 24;
	m_wand->write( temp );
	temp += 24;
	m_joystick->write( temp );
	temp += 8;
	m_buttons->write( temp );
	temp += 4;
	for( int i=0;i<(int)m_syncsegments.size();i++ )
	{
		m_syncsegments[i]->write( temp );
		temp += m_syncsegments[i]->getSize();
	}
	return dtkNav::postFrame();
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
	return new navRead(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
	delete augment;
	return DTKDSO_UNLOAD_CONTINUE;
}
