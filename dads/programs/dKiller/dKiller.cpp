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
#include <stdlib.h>
#include <string.h>
#include <dMessage.h>
#include <dPostOffice.h>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

string         type;
string         prog;
vector<string> args;
vector<string> clients;
dPostOffice*   postoffice;
dMessage*      message;

void getHosts();
void killingSpree();
void interpretArgs();
void usage( int code );

int main(int argc, char** argv)
{
	for ( int i=1;i<argc;i++ )
	{
		args.push_back( argv[i] );
	}
	interpretArgs();
	killingSpree();
}

void interpretArgs()
{
	for( unsigned int i=0;i<args.size();i++ )
	{
		if( args[i] == "-h" || args[i] == "--help" )
			usage( 0 );
		else if( args[i] == "-k" || args[i] == "--kill" )
		{
			if( type.size() == 0 )
				type = args[i];
			else
			{
				printf( "ERROR - attempt to set multiple options!\n" );
				usage( 1 );
			}
			i++;
			if( i < args.size() )
				prog = args[i];
			else
			{
				printf( "ERROR - missing parameter for kill option!\n" );
				usage( 1 );
			}
		}
		else if( args[i] == "-a" || args[i] == "--killall" )
		{
			if( type.size() == 0 )
				type = args[i];
			else
			{
				printf( "ERROR - attempt to set multiple options!\n" );
				usage( 1 );
			}
		}
		else if( i == 0 )
		{
			if( args[i][0] == '-' )
			{
				printf( "ERROR - valid option not specified!\n" );
				usage( 1 );
			}
			clients.push_back( args[i] );
		}
		else
			usage( 1 );
	}
	if( !type.size() )
	{
		printf( "ERROR - valid option not specified!\n" );
		usage( 1 );
	}
	if( !clients.size() )
		getHosts();
}

void getHosts()
{
        char* hosts = getenv( "DADS_CLIENTS" );
        if( hosts != NULL )
        {
		char* host = strtok( hosts, ":" );
		while ( host )
		{
                        clients.push_back( string(host) );
                        host = strtok( NULL, ":" );
                }
        }
}

void killingSpree()
{
	message = new dMessage;
	for( unsigned int i=0;i<clients.size();i++ )
	{
		if( type == "--kill" && prog.size() )
			printf( "Killing %s for %s...\n", prog.c_str(),
					clients[i].c_str() );
		else
			printf( "Killing ALL processes for %s...\n",
					clients[i].c_str() );
		postoffice = new dPostOffice( string("COMMAND_SEGMENT") +
				clients[i] );
        postoffice->connectTo( clients[i] );
		message->clear();
		message->setFrom( "CONSOLE" );
		message->setTo( "DAEMON_SERVER" );
		message->setSubject( "STOP_PROCESS" );
		if( prog.size() > 0 )
			message->addString( prog );
		postoffice->sendMessage( message );
		delete postoffice;
	}
	delete message;
}

void usage( int code )
{
	printf( "\nUsage: dKiller [hostname] [ --kill progname | --killall ]\n\n" );
	printf( "dKiller is used to kill processes which were started by the dListener\n" );
	printf( "daemon running on a client machine. Processes can be killed on a specific\n" );
	printf( "machine or all machines defined in the DADS_CLIENTS environment\n" );
	printf( "variable. A specific program process can be killed or all processes\n" );
	printf( "started by the dListener daemon can be killed.\n\n" );
	printf( "Options:\n" );
	printf( "    hostname         If [hostname] is defined then a message will be\n" );
	printf( "                     sent to that client to kill the processes.\n" );
	printf( "                     Otherwise the message is sent to all hosts\n" );
	printf( "                     defined in the DADS_CLIENTS environment\n" );
	printf( "                     variable.\n\n" );
	printf( "    --kill progname  Use the [kill program] option to kill a specific\n" );
	printf( "    -k     progname  program which was started by the dListener where\n" );
	printf( "                     program is the name of the process to be killed.\n\n" );
	printf( "    --killall        Use the [killall] option to kill all processes\n" );
	printf( "    -a               which were started by the dListener.\n\n" );
        printf( "    -h | --help      Shows this usage/help information.\n\n" );
	exit( code );
}
