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
#include <dtk.h>
#include <unistd.h>
#include <stdlib.h>
#include <dtk/dtkDSO_loader.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
using namespace std;

void sigchild_handler( int s )
{
	while( wait( NULL ) > 0 )
		;
}

class dSound : public dtkAugment
{
	public:

	dSound(dtkManager* m);
	~dSound(void) ;
	int preConfig(void);
};

// create object and set description.
// be sure to validate it!
dSound::dSound(dtkManager *m):dtkAugment("dSound")
{
	setDescription("starter dso for cluster\n");
	printf( "STARTER - SUCCESSFULLY STARTED CLIENTS\n" );
	validate();
}

// destructor
dSound::~dSound(void) 
{
}

// called only once by dpf, after pfInit() and dpfDisplay:preConfig()
// but before pfConfig()
int dSound::preConfig(void) 
{
	char* soundfile = getenv( "DPF_SOUND_FILE" );
	if( soundfile )
	{
		int pidchild = fork();
		if( pidchild < 0 )
		{
			printf( "ERROR - Unable to fork process\n" );
		}
		else if( pidchild == 0 )
		{
			struct sigaction sa;
			sa.sa_handler = sigchild_handler;
			sigemptyset( &sa.sa_mask );
			sa.sa_flags = SA_RESTART;
			if( sigaction( SIGCHLD, &sa, NULL ) == -1 )
			{
				perror( "sigaction" );
				exit(1);
			}
			vector<string> sounds;
			char* temp = strtok( soundfile, ":" );
			while( temp )
			{
				sounds.push_back( temp );
				temp = strtok( NULL, ":" );
			}
			char** Cleanup = new char*[2+sounds.size()];
			Cleanup[0]="soundPlayer";
			for( int i=1;i<=(int)sounds.size();i++ )
				Cleanup[i] = strdup( sounds[i].c_str() );
			Cleanup[sounds.size()+1]=NULL;
			execvp(Cleanup[0], Cleanup);
			delete [] Cleanup;
			exit(0);
		}
	}
	return DTK_CONTINUE;
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
  return new dSound(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}
