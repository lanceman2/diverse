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
//======================================================
// dCreateShm.cpp
// Written by Patrick Shinpaugh
// Date: 12/30/2003
//======================================================
#include <dtk.h>
#include <dPostOffice.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

vector<string> args;
vector<string> shmnames;
vector<int> shmsizes;
vector<string> ponames;
bool caveshm = false;

void createShmSegs( string name, int size );
void createPOSegs( string name );
void createCaveSegs();
void interpretArgs();
void usage( int code );

int main(int argc, char** argv)
{
	if( argc == 1 )
		usage( 1 );
	for( int i=1;i<argc;i++ )
		args.push_back( argv[i] );
	interpretArgs();
	if( caveshm )
		createCaveSegs();
	for( unsigned int i=0;i<shmnames.size();i++ )
		createShmSegs( shmnames[i], shmsizes[i] );
	for( unsigned int i=0;i<ponames.size();i++ )
		createPOSegs( ponames[i] );
	return 0;
}

void createShmSegs( string name, int size )
{
	dtkSharedMem* shm = new dtkSharedMem( name.c_str(), 0 );
	if( shm->isInvalid() )
	{
		delete shm;
		shm = new dtkSharedMem( sizeof(char)*size, name.c_str() );
	}
	delete shm;
}

void createPOSegs( string name )
{
	dPostOffice* po = new dPostOffice( name );
	delete po;
}

void createCaveSegs()
{
	createShmSegs( "head", 24 );
	createShmSegs( "wand", 24 );
	createShmSegs( "joystick", 8 );
	createShmSegs( "buttons", 1 );
}

void interpretArgs()
{
	vector<string>::iterator itr;
	for( itr=args.begin();itr!=args.end();itr++ )
	{
		if( *itr == "-h" || *itr == "--help" )
			usage( 0 );
		else if( *itr == "-s" || *itr == "--shm" )
		{
			itr++;
			if( itr == args.end() )
			{
				printf( "ERROR - missing segment name!\n" );
				usage( 1 );
			}
			if( (*itr)[0] == '-' )
			{
				printf( "ERROR - segment names cannot begin with \"-\"!!!\n" );
				usage( 1 );

			}
			shmnames.push_back( *itr );
			itr++;
			if( itr == args.end() )
			{
				printf( "ERROR - missing segment size!\n" );
				usage( 1 );
			}
			if( atoi( (*itr).c_str() ) < 1 )
			{
				printf( "ERROR - segment size must be an integer > 0 !!!\n" );
				usage( 1 );

			}
			shmsizes.push_back( atoi( (*itr).c_str() ) );
		}
		else if( *itr == "-p" || *itr == "--po" )
		{
			itr++;
			if( itr == args.end() )
			{
				printf( "ERROR - missing post office name!\n" );
				usage( 1 );
			}
			if( (*itr)[0] == '-' )
			{
				printf( "ERROR - post office names cannot begin with \"-\"!!!\n" );
				usage( 1 );

			}
			ponames.push_back( *itr );
		}
		else if( *itr == "-c" || *itr == "--cave" )
			caveshm = true;
		else
			usage( 1 );
	}
}

void usage( int code )
{
	printf( "\nUsage: dCreateShm [options]\n\n" );
	printf( "dCreateShm is used to create shared memory or post office segments.\n"
			"Any combination of options is valid though it is the\n"
			"repsonibility of the user to avoid duplicating names.\n\n" );
	printf( "Options:\n" );
	printf( "    --shm name size  This will create a shared memory segment with\n" );
	printf( "    -s    name size  the specified name and size.\n\n" );
	printf( "    --po  name       This will create a PostOffice with the name\n" );
	printf( "    -p    name       specified for use with messaging.\n\n" );
	printf( "    --cave           This will create the standard shared memory\n" );
	printf( "    -c               segments head, wand, joystick, and buttons.\n\n" );
	printf( "    -h | --help      Shows this usage/help information.\n\n" );
	exit( code );
}
