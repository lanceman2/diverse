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
#include <unistd.h>
#include <dMessage.h>
#include <dPostOffice.h>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

string         name;
vector<string> args;
vector<string> clients;
dPostOffice*   postoffice;
dMessage*      message;

void interpretArgs();
void getHosts();
void unsetEnv();
void usage( int code );

int main(int argc, char** argv)
{
	for ( int i=0;i<argc;i++ )
		args.push_back( argv[i] );
	interpretArgs();
	unsetEnv();
}

void interpretArgs()
{
	vector<string>::iterator itr;
	for( itr=args.begin();itr!=args.end();itr++ )
	{
		if( itr == args.begin() )
			continue;
		if( *itr == "-c" || *itr == "--computer" ||
				*itr == "--hostname" )
		{
			itr++;
			if( itr == args.end() )
			{
				printf( "ERROR - Missing a computer/host name"
						"for argument.\n\n" );
				usage( 1 );
			}
			clients.push_back( *itr );
		}
		else if( *itr == "-h" || *itr == "--help" )
		{
			usage( 0 );
		}
		else if( *itr == "-e" || *itr == "--env" ||
				*itr == "-n" || *itr == "--name" )
		{
			itr++;
			if( itr == args.end() )
			{
				printf( "ERROR - Missing an environment"
						"variable name for"
						"argument.\n\n" );
				usage( 1 );
			}
			name = *itr;
			break;
		}
		else
		{
			printf( "ERROR - improper usage...\n\n" );
			usage( 1 );
		}
	}
	if( !clients.size() )
		getHosts();
	if( !clients.size() )
	{
		printf( "ERROR - a computer/host name was not included in "
				"the arguments and the DADS_CLIENTS "
				"environment variable has not been set. "
				"Please set the DADS_CLIENTS "
				"environment variable or include a "
				"computer/host name using the -c, "
				"--computer, or --hostname options.\n\n" );
		usage( 1 );
	}
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

void unsetEnv()
{
	message = new dMessage;
	for( unsigned int i=0;i<clients.size();i++ )
	{
		printf( "Unsetting %s for %s...\n", name.c_str(), clients[i].c_str() );
		postoffice = new dPostOffice( string("COMMAND_SEGMENT") +
				clients[i] );
        postoffice->connectTo( clients[i] );
		message->clear();
		message->setFrom( "CONSOLE" );
		message->setTo( "DAEMON_SERVER" );
		message->setSubject( "UNSET_ENV" );
		message->addString( name );
		postoffice->sendMessage( message );
		delete postoffice;
	}
	delete message;
}

void usage( int code )
{
	printf( "\nUsage: dUnsetEnv [options]\n\n" );
	printf( "dUnsetEnv provides a means to unset environment variables using the\n"
			"dListener daemon running on a remote machine. The environment\n"
			"variable can be unset for all clients listed in the\n"
			"DADS_CLIENTS environment variable on the local machine\n"
			"or can be unset using the computer/host name option for individual\n"
			"machines.\n\n" );
	printf( "Options:\n" );
	printf( "    -c         hostname   These options will set the name of the client\n" );
	printf( "    --computer hostname   as the hostname provided for which to unset the\n" );
	printf( "    --hostname hostname   environment variable.\n\n" );
	printf( "    -e    name            This mandatory option specifies the environment\n" );
	printf( "    --env name            variable to be unset.\n\n" );
	printf( "    -h | --help           Shows this usage/help information.\n\n" );
	exit( code );
}
