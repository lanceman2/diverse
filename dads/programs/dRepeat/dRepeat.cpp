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
// dRepeat.cpp
// Written by Patrick Shinpaugh
// Date: 12/30/2003
//======================================================
#include <dtk.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

vector<string> args;
vector<string> fromname;
vector<string> toname;
vector<dtkSharedMem*> fromsegs;
vector<dtkSharedMem*> tosegs;
unsigned int size = 0;
unsigned int maxsize = 0;

void quit( int code );
void interpretArgs();
void usage( int code );

int main(int argc, char** argv)
{
	if( argc == 1 )
		usage( 1 );
	for( int i=1;i<argc;i++ )
		args.push_back( argv[i] );
	interpretArgs();
	for( unsigned int i=0;i<size;i++ )
	{
		dtkSharedMem* fromshm = new dtkSharedMem( fromname[i].c_str() );
		if( fromshm->isInvalid() )
		{
			delete fromshm;
			printf( "ERROR - shared memory segments to be repeated must already\n"
				       "exist!!! %s does NOT exist!!!\n", fromname[i].c_str() );
			quit( 1 );
		}
		fromsegs.push_back( fromshm );
		dtkSharedMem* toshm = new dtkSharedMem( toname[i].c_str() );
		maxsize = maxsize > fromshm->getSize() ? maxsize : fromshm->getSize();
		if( toshm->isInvalid() )
		{
			delete toshm;
			toshm = new dtkSharedMem( fromshm->getSize(), toname[i].c_str() );
		}
		tosegs.push_back( toshm );
	}
	void* buffer = new char[maxsize+1];
	int pid = fork();
	if (pid <0)
	{
		printf("Error, could not fork\n");
		exit(1);
	}
	else if (pid > 0)
	{
		exit(0);
	}
	if (setsid() <0)
	{
		printf("Error, could not become daemon\n");
		exit(1);
	}
	chdir( "/" );
	while( 1 )
	{
		for( unsigned int i=0;i<size;i++ )
		{
			fromsegs[i]->read( &buffer );
			tosegs[i]->write( &buffer );
			usleep(1);
		}
	}
}

void quit( int code )
{
	unsigned int i;
	for( i=0;i<fromsegs.size();i++ )
	{
		delete fromsegs[i];
	}
	for( i=0;i<tosegs.size();i++ )
	{
		delete tosegs[i];
	}
	exit( code );
}

void interpretArgs()
{
//	vector<string>::iterator itr;
//	for( itr=args.begin();itr!=args.end();itr++ )
	for( int i=0;i<(int)args.size();i++ )
	{
		if( args[i] == "-h" || args[i] == "--help" )
			usage( 0 );
		else if( args[i] == "--segment" || args[i] == "-s" )
		{
			i++;
			int num = 0;
			while( i < (int)args.size() )
			{
				if( args[i][0] == '-' )
					break;
				fromname.push_back( args[i] );
				toname.push_back( args[i] + string( "repeat" ) );
				num++;
				i++;
			}
			if( !num )
			{
				printf( "ERROR - missing segment list!!!\n" );
				usage( 1 );
			}
		}
		else if( i == (int)args.size() )
			break;
		else if( args[i] == "--from" || args[i] == "-f" )
		{
			i++;
			if( i >= (int)args.size() )
			{
				printf( "ERROR - missing segment - incorrect usage!!!\n" );
				usage( 1 );
			}
			if( args[i][0] == '-' )
			{
				printf( "ERROR - segment names cannot begin with \"-\"!!!\n" );
				usage( 1 );
			}
			fromname.push_back( args[i] );
			i++;
			if( i >= (int)args.size() )
			{
				printf( "ERROR - missing segment - incorrect usage!!!\n" );
				usage( 1 );
			}
			if( args[i] == "--to" || args[i] == "-t" )
			{
				i++;
				if( i >= (int)args.size() )
				{
					printf( "ERROR - missing segment - incorrect usage!!!\n" );
					usage( 1 );
				}
				if( args[i][0] == '-' )
				{
					printf( "ERROR - segment names cannot begin with \"-\"!!!\n" );
					usage( 1 );
				}
				toname.push_back( args[i] );
			}
		}
		else if( i >= (int)args.size() )
			break;
	}
	if( fromname.size() != toname.size() )
	{
		printf( "ERROR - the number of from and to segments must be equal!!!\n" );
		usage( 1 );
	}
	size = fromname.size();
}

void usage( int code )
{
	printf( "\nUsage: dRepeat [options]\n\n" );
	printf( "dRepeat will repeat shared memory segments such that other machines\n"
			"can connect via remote shared memory. There are 2 methods of\n"
			"repeating the shared memory segments with this daemon. The first\n"
			"is using --segment and providing a list of segments where the\n"
			"repeated segments will each have the basename with a suffix of\n"
			"repeat added to the ending. (i.e. dRepeat --segment head wand\n"
			"will result in repeated shared memory segments of headrepeat\n"
			"and wandrepeat).\n"
			"The second method is to use --from segment --to segment pairs\n"
			"where the from segment will be repeated with the to segment\n"
			"name. Within each pair the from segment must precede the to\n"
			"segment.\n\n" );
	printf( "Options:\n" );
	printf( "    --segment segmentlist  This will repeat each segment in the segment\n" );
	printf( "    -s        segmentlist  list with a suffix of repeat (i.e. dRepeat\n" );
	printf( "                           --segment head wand will result in repeated\n" );
	printf( "                           shared memory segments of headrepeat and\n" );
	printf( "                           wandrepeat).\n\n" );
	printf( "    --from    segment      Must be paired with a --to segment. Specify\n" );
	printf( "    -f        segment      the existing segment to be repeated.\n\n" );
	printf( "    --to      segment      Must be paired with a --from segment. Specify\n" );
	printf( "    -t        segment      the name of the repeated segment.\n\n" );
	printf( "    -h | --help            Shows this usage/help information.\n\n" );
	exit( code );
}
