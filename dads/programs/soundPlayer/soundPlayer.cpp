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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
/*#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>*/

int playSound( char* arg );
int stopSound(); 

void sigchild_handler( int s )
{
	while( wait( NULL ) > 0 )
		;
}

int main( int argc, char** argv )
{
	int pidxmms = 0;
	int pidparent = getppid();
/*	int pidthis = fork();
	if( pidthis < 0 )
	{
		printf( "ERROR - Unable to spawn new process\n" );
		exit(1);
	}
	else if( pidthis > 0 )
		return 0;
	setsid();
	struct sigaction sa;
	sa.sa_handler = sigchild_handler;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = SA_RESTART;
	if( sigaction( SIGCHLD, &sa, NULL ) == -1 )
	{
		perror( "sigaction" );
		exit(1);
	}*/
	if( argc == 2 )
	{
		if( ( pidxmms = playSound( argv[1] ) ) > 0 )
		{
			while( getsid( pidparent ) != -1 && getsid( pidxmms ) != -1 )
				usleep( 10000 );
		}
		else
			return pidxmms;
	}
	if( pidxmms )
	{
		stopSound();
	}
	if( int sigrtrn = kill( pidxmms, SIGQUIT ) != 0 )
	{
		printf( "ERROR - Unable to exit xmms using signal SIGQUIT - returned %d\n", sigrtrn );
		exit(1);
	}
	return 0;
}

int playSound( char* arg )
{
//	string Name = Message->getString();
	int cleanpid= fork();
	if (cleanpid < 0)
	{
		printf("soundPlayer : playSound() - Error could not create child\n");
		exit(1);
	}
	else if( cleanpid > 0 )
		return cleanpid;
	//Child that destroys shared memory
	if (cleanpid == 0)
	{
		printf( "setsid pid = %d\n", setsid() );
		struct sigaction sa;
		sa.sa_handler = sigchild_handler;
		sigemptyset( &sa.sa_mask );
		sa.sa_flags = SA_RESTART;
		if( sigaction( SIGCHLD, &sa, NULL ) == -1 )
		{
			perror( "sigaction" );
			exit(1);
		}
		char* Cleanup[3];
		Cleanup[0]="xmms";
		Cleanup[1]=arg;
		Cleanup[2]=NULL;
		execvp(Cleanup[0], Cleanup);
		exit(0);
	}
	return cleanpid;
}

int stopSound()
{
	int cleanpid= fork();
	if (cleanpid < 0)
	{
		printf("soundPlayer : stopSound() - Error could not create child\n");
		exit(1);
	}

	//Child that destroys shared memory
	if (cleanpid == 0)
	{
		//Setup arguments for an execvp
		char* Cleanup[3];
		Cleanup[0]="xmms";
		Cleanup[1]="--stop";
		Cleanup[2]=NULL;
		execvp(Cleanup[0], Cleanup);
	}
	return cleanpid;
}
