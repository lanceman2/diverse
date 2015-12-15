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
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#ifdef DADS_ARCH_DARWIN
 extern char **environ;
#endif
using namespace std;

//List of our names and ids
vector<string> PROCESS_NAMES;
vector<int>    PROCESS_IDS;
bool logthis = false;
string logfile = "/tmp/dListener.log";
FILE* redirect = NULL;

//Start and stop programs
void startProgram(dMessage* Message);
void stopProgram(dMessage* Message);
void addEnv(dMessage* Message);
void unsetEnv(dMessage* Message);

//Cleanup all running programs
void cleanup();
void startPrograms();

void sigchild_handler( int s )
{
	while( wait( NULL ) > 0 )
		;
}

void usage( int code )
{
	printf( "Usage: dListener [options]\n\n" );
	printf( "dListener is a daemon which listens for messages from various programs.\n" );
	printf( "It must be running on each client for the DADS system to work properly.\n\n" );
	printf( "Options:\n" );
	printf( "    -h | --help           Shows this usage/help information.\n\n" );
	printf( "    -l | --log [logfile]  Log all stdout and stderr output to a logfile.\n" );
	printf( "                          You may specify the logfile or use the default.\n" );
	printf( "                          Default logfile is /tmp/dListener.log.\n\n" );
	exit( code );
}

int main(int argc, char** argv)
{
	if( argc > 1 )
	{
		string arg = argv[1];
		if( arg == "-h" || arg == "--help" )
			usage( 0 );
		else if( arg == "-l" || arg == "--log" )
		{
			logthis = true;
			if( argc > 2 )
				logfile = argv[2];
		}
		else
			usage( 1 );
	}
	//The communication mechanisms this process uses with the outside world
	char host[256];
	gethostname( host, 255 );
	string po = "COMMAND_SEGMENT" + string( host );
	printf("Connecting to %s\n", po.c_str());
	dPostOffice PostOffice( po );
	dMessage* Message = new dMessage;

	//Check to see if I am already running on this machine
	//This is so we do not allow duplicate daemonss
	Message->setSubject("CHECK_STATUS");
	Message->setTo("DAEMON_SERVER");
	Message->setFrom("DAEMON_SERVER");
	//Send check message
	PostOffice.sendMessage(Message);
	delete Message;
	Message = NULL;
	//Get message
	Message = PostOffice.getMessage();
	//Go through this 10 times in case of message traffic
	for (int i=0; i<10; i++)
	{
		printf( "dListener - CHECK_STATUS\n" );
		if (Message !=NULL)
		{
			if (Message->getSubject() == "STATUS_RUNNING")
			{
				printf("Trying to start duplicate daemon, exiting\n");
				return 0;
			}
		}
		usleep(1000);
		Message = PostOffice.getMessage();
	}
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
	if( logthis )
	{
		if( (redirect = freopen( logfile.c_str(), "a", stdout )) == NULL )
		{
			printf( "ERROR - could not redirect stdout to %s\n", logfile.c_str() );
			exit( 1 );
		}
		if( freopen( logfile.c_str(), "a", stderr ) == NULL )
		{
			printf( "ERROR - could not redirect stderr to %s\n", logfile.c_str() );
			exit( 1 );
		}
	}
	struct sigaction sa;
	sa.sa_handler = sigchild_handler;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = SA_RESTART;
	if( sigaction( SIGCHLD, &sa, NULL ) == -1 )
	{
		perror( "sigaction" );
		exit(1);
	}
	printf( "dListener daemon started successfully.\n" );
	//Infinite loop until told to die
	while (1)
	{
		Message = PostOffice.getMessage();
		if (Message !=NULL)
		{
			printf("\n\n%s\n\n", Message->getMessage().c_str());
		}
		if (Message != NULL && Message->getTo() =="DAEMON_SERVER")
		{
			//Respond to another daemon trying to start up
			if (Message->getSubject() == "CHECK_STATUS")
			{
				printf( "dListener - CHECK_STATUS\n" );
				delete Message;
				Message = new dMessage;
				Message->setSubject("STATUS_RUNNING");
				PostOffice.sendMessage(Message);
			}
			//Start a process
			else if (Message->getSubject() == "START_PROCESS")
			{
				printf( "dListener - START_PROCESS\n" );
				startProgram(Message);
			}
			//Kill myself
			else if (Message->getSubject() == "DIE")
			{
				printf( "dListener - DIE\n" );
				cleanup();
				break;
			}
			//Stop a process
			else if (Message->getSubject() == "STOP_PROCESS")
			{
				printf( "dListener - STOP_PROCESS\n" );
				stopProgram(Message);
			}
			//Add an environmental variable
			else if (Message->getSubject() == "ADD_ENV")
			{
				printf( "dListener - ADD_ENV\n" );
				addEnv(Message);
			}
			else if (Message->getSubject() == "UNSET_ENV")
			{
				printf( "dListener - UNSET_ENV\n" );
				unsetEnv(Message);
			}
			else if( Message->getSubject() == "GET_PROCESSES" )
			{
				printf( "dListener - GET_PROCESSES\n" );
				delete Message;
				Message = new dMessage;
				Message->setFrom( "DAEMON_SERVER" );
				Message->setTo( "CONSOLE" );
				Message->setSubject( "PROCESSES" );
				for( int i=0;i<(int)PROCESS_NAMES.size();i++ )
				{
					Message->addString( PROCESS_NAMES[i] );
				}
				PostOffice.sendMessage( Message );
			}
			delete Message;
			Message = NULL;
		}
        fflush( stdout );
		usleep(100000);
	}
	return 0;
}

//Function designed to kill all children and then exit
void cleanup()
{
	for (unsigned int i=0; i< PROCESS_IDS.size(); i++)
	{
		kill(PROCESS_IDS[i], 9);
	}
	if( logthis )
		fclose( redirect );
}

//Stop a specific program as specified in Message
void stopProgram(dMessage* Message)
{
	printf( "Stopping programs\n" );
	//Give the child a little time to gracefully exit
	usleep(10000);
	//The process to stop
	string Name = Message->getString();
	printf( "Program to be stopped - %s\n", Name.size() ? Name.c_str() : "ALL" );
	//Go through the processes started, find it, and kill it
	for (unsigned int i=0; i< PROCESS_NAMES.size(); i++)
	{
		//Found it 
		if( Name.size() != 0 )
			if (PROCESS_NAMES[i] != Name)
				continue;
		//Kill it
        kill( PROCESS_IDS[i], SIGHUP );
        sleep( 1 );
		kill (PROCESS_IDS[i], 9);
		//Erase it from the history books
		PROCESS_NAMES.erase(PROCESS_NAMES.begin() + i);
		PROCESS_IDS.erase(PROCESS_IDS.begin() + i);
	}
}

//This executes a process with a given message
//Basically it records information in the parent
//Becomes the user that said start the program, then exec it like no tomorrow
void startProgram(dMessage* Message)
{
	int UID =-1;
	string cwd = "";
	int argc;
	vector<string> argv;
	UID = Message->getInt();
	cwd = Message->getString();
	argc = Message->getInt();
	for (int i=0; i<argc; i++)
		argv.push_back(Message->getString());
	for (int i=0; i<argc; i++)
		printf( "%s ", argv[i].c_str() );
    int fd[2] = { 0, 0 };
    if( pipe( fd ) == -1 )
    {
        printf( "ERROR: Unable to create pipe\n" );
    }
	printf( "\n" );
	pid_t pid = fork();
	//Could not fork
	if (pid < 0)
		return;
	//Parent code
	if (pid != 0)
	{
		//Store info in the parent
		Message->clear();
        close( fd[1] );
        char buffer[256];
        int size = read( fd[0], buffer, 256 );
        buffer[size] = '\0';
        pid = atoi( buffer );
        printf( "The new process has PID %d\n", pid );
        fflush( stdout );
		PROCESS_NAMES.push_back(argv[0]);
		PROCESS_IDS.push_back(pid);
        close( fd[0] );
		return;
	}
	//Child code, actually run the process!
	else
	{
        pid = fork();
        if( pid < 0 )
        {
            printf( "ERROR: Unable to refork the process\n" );
            exit( -1 );
        }
        else if( pid != 0 )
    	{
		    exit( 0 );
    	}
        else
        {
            close( fd[0] );
            pid = getpid();
            char buffer[256];
            int size = sprintf( buffer, "%d", pid );
            write( fd[1], buffer, size + 1 );
            close( fd[1] );
    	    printf( "%s started successfully.\n", argv[0].c_str() );
    		//Become session leader
	    	setsid();
		    //Become the user running the program
    		setuid(UID);
	    	//Goto the directory where it was started
		    chdir( cwd.c_str() );
    		char** args = new char*[argc+1];
	    	for (int i=0 ; i< argc; i++)
		    	args[i] = strdup(argv[i].c_str());
    		args[argc] = NULL;
	    	if( !execve(args[0], args, environ) )
                printf( "ERROR: Unable to execute command\n" );
	    	//Try seeing if the application is in your path first
		    if( !execvp(args[0], args) )
                printf( "ERROR: Unable to execute command\n" );
    		//If it isn't then execute in your cwd
	    	if( !execve(args[0], args, NULL) )
                printf( "ERROR: Unable to execute command\n" );
		    exit(0);
        }
	}
}
//Set an environmental variable from Message on this machine.
//This is used to replicate the environment for the user
//It also goes through and modifies DGL_DSO_FILES, DPF_DSO_FILES, and DTK_DSO_FILES to have specific DSO's for this machine
//as specified by the config file on the command line
void addEnv(dMessage* Message)
{
	string Name= Message->getString();
	string Value = Message->getString();
	if (Value == "BAD")
	{
		//printf("Trying to set %s to an undefined value\n", Name.c_str());
		return;
	}
	//Parse the env var
	if (Name == "DPF_DSO_FILES" || Name == "DTK_DSO_FILES" || Name == "DGL_DSO_FILES")
	{
		//Temp string for holding
		string NewVal="";
		//The env var value to pass
		char* Temp = strdup(Value.c_str());
		//Get values out of string piece by piece
		char* Val = strtok(Temp, ":");
		while (Val != NULL)
		{
			//If it isn't the starting dso or the writer dso
			if (strcmp(Val, "starter") !=0 && strcmp(Val, "navWrite")!=0)
			{
				if (NewVal.size() > 0)
					NewVal = NewVal + ":";
				NewVal = NewVal + string(Val);
			}
			Val = strtok(NULL, ":");
		}
		free(Temp);
		//Add in the cluster dso's that we need in addition to what was
		//sent over :)
//		Value =  FILEPARSER->getDSO() + NewVal;
		Value = NewVal;
	}
	printf( "\n\n%s=%s\n", Name.c_str(), Value.c_str() );
#ifdef DADS_ARCH_IRIX
	string name = Name + string( "=" ) + Value;
	putenv( name.c_str() );
#else
	setenv(Name.c_str(), Value.c_str(),1);
#endif
	/* DEBUG
	printf("Trying to add %s with value %s\n", Name.c_str(), Value.c_str());
	if (getenv(Name.c_str()) != NULL)
		printf("Result is %s\n", getenv(Name.c_str()));
	*/
}

void unsetEnv(dMessage* Message)
{
	string Name = Message->getString();
#ifdef DADS_ARCH_IRIX
	string cmd = string( "unset ") + Name;
	system( cmd.c_str() );
#else
	unsetenv( Name.c_str() );
#endif
}

