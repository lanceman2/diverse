/************************************************************************
* dtk-masterServer
*
* daemon to start, stop, and reset other DTK and DADS daemons.
*
* by Patrick Shinpaugh 9/15/2004
************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "dtk-mastercommon.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

using namespace std;

dSocket* serversocket;
dSocket* client;
fd_set master;
int fdmax;
pid_t childpid = 0;
dtkmasterCommon* common;
FILE* logfile;

void usage( int code )
{
	printf( "Usage: dtk-masterServer [ --shutdown | -s ]\n\n" );
	printf( "dtk-masterServer is a daemon which listens for messages from \n"
			"various programs to reset dtk-server and dtk-server services \n"
			"(i.e. is900, wingmanJoystick, etc) as well as dListener daemons. \n"
			"This was created to minimize security risks from the scripts and \n"
			"programs used to resetting the DADS and CAVE systems which \n"
			"required the direct use of the root password.\n\n" );
	printf( "Options:\n\n"
			"--shutdown | -s    Use this option to shutdown the dtk-masterServer\n"
			"                   daemon.\n\n" );
	printf( "Configuration:\n\n"
			"dtk-masterServer uses a configurations file /etc/dtk-masterServer.conf \n"
			"which should only be viewable by root. Below is a list of options \n"
			"which can be set in the configuration file.\n\n" );
	printf( "port               This is the port on which the server will listen. \n"
			"                   Each client program which is run on other machines \n"
			"                   should be running on the same port. The default is \n"
			"                   65535.\n\n" );
	printf( "hosts_accept       A colon separated list of hostnames from which the \n"
			"                   server will accept commands. Default is localhost.\n\n" );
	printf( "hosts_reset        A colon separated list of hostnames to which the \n"
			"                   server will send reset commands. There is no default."
			"\n\n" );
	printf( "start_dtkserver   A boolean value specifying whether the dtk-server \n"
			"                   should be started when a reset occurs. Acceptable \n"
			"                   values are yes, no, true, or false. The default \n"
			"                   value is false.\n\n" );
	printf( "start_dListener    A boolean value specifying whether the dListener \n"
			"                   should be started when a reset occurs. Acceptable \n"
			"                   values are yes, no, true, or false. The default \n"
			"                   value is false.\n\n" );
	printf( "dtk_services       A colon separated list of service names (i.e. is900, \n"
			"                   wingManJoystick, etc) which should be started when \n"
			"                   a reset occurs. There is no default.\n\n" );
	printf( "logging            A boolean value specifying whether warning and error \n"
			"                   messages should be logged. The default is false.\n\n" );
	printf( "logfile            A filename (including path) where log information \n"
			"                   should be stored. The default is \n"
			"                   /tmp/dtk-masterServer.log.\n\n" );
	printf( "apps_accept        A colon separated list of programs/commands/servers \n"
			"                   which can be started through the dtk-masterServer. \n"
			"                   If not defined, none may be started locally.\n\n" );
	printf( "services_accept    A colon separated list of dtk services which can be \n"
			"                   started through the dtk-masterServer. If not defined, \n"
			"                   none may be started locally.\n" );
	printf( "commands           A colon separated list of commands to be run after \n"
			"                   starting servers/services.\n" );
	printf( "reset_commands     A colon separated list of commands to be run after \n"
			"                   stopping any previously running servers/services but"
			"                   before starting any servers/services.\n" );
	exit( code );
}

int serverInit( int port )
{
	serversocket = new dSocket( port );
	if( !serversocket )
		return -1;
	serversocket->bindSocket();
	serversocket->listenSocket();
	FD_SET( serversocket->getfd(), &master );
	fdmax = serversocket->getfd();
	fprintf( logfile, "server fd: %d\n", serversocket->getfd() );
	return 0;
}

void runCommand( string cmd, bool waiting = false )
{
	fprintf( logfile, "Running... %s\n", cmd.c_str() );
	fflush( logfile );
	int pid = fork();
	if( pid < 0 )
	{
		fprintf( logfile, "ERROR: Unable to fork process -\n%s\n\t- exiting\n",
				cmd.c_str() );
	fflush( logfile );
		return;
	}
	else if( pid > 0 )
	{
		childpid = pid;
		if( waiting )
		{
			int status;
			wait( &status );
			if( !WIFEXITED( status) )
				fprintf( logfile, "ERROR: \"%s\" exited abnormally.\n", cmd.c_str() );
			else if( WEXITSTATUS( status ) )
				fprintf( logfile, "ERROR: \"%s\" exited with code %d.\n", cmd.c_str(),
						(int)WEXITSTATUS( status ) );
		}
		return;
	}
	delete serversocket;
	delete client;
	delete common;
	FD_ZERO( &master );
	vector<string> args;
	tokenize( cmd, args, " \t\\\n" );
	char** execlist = new char*[args.size()+1];
	for( unsigned int i=0;i<args.size();i++ )
		execlist[i] = strdup( args[i].c_str() );
	execlist[args.size()] = NULL;
	if( execvp( execlist[0], execlist ) != 0 )
	{
		fprintf( logfile, "ERROR: Unable to execute command - %s\n", execlist[0] );
		fflush( logfile );
		exit( -1 );
	}
	exit( 0 );
}

void commandStop()
{
#ifdef DADS_ARCH_IRIX
	runCommand( "dtk-swapServer-stop", true );
#endif
	runCommand( "dListener-stop", true );
	runCommand( "killall -9 dListener", true );
	runCommand( "dtk-shutdownServer", true );
	runCommand( "killall -9 dtk-server", true );
	runCommand( "dtk-destroySharedMem -r", true );
	runCommand( "dtk-destroySharedMem -r", true );
	for( unsigned int i=0;i<common->reset_commands.size();i++ )
		runCommand( common->reset_commands[i], true );
}

bool sendMessage( const string& hostname, int port, string msg )
{
	fprintf( logfile, "sendMessage( %s, %d, %s )\n", hostname.c_str(), port, msg.c_str() );
	fflush( logfile );
	dSocket* client = new dSocket( common->netport, hostname );
	if( !client )
	{
		fprintf( logfile, "ERROR: Unable to create client connection to %s\n",
				hostname.c_str() );
		fflush( logfile );
		return false;
	}
	if( !client->connectSocket() )
	{
		printf( "ERROR: Unable to connect to %s\n", hostname.c_str() );
		return false;
	}
	if( !client->writeSocket( msg.c_str(), msg.size() ) )
    {
        fprintf( logfile, "ERROR: Unable to write message\n" );
        fflush( logfile );
        delete client;
        return false;
    }
	while( msg != "COMPLETED" )
	{
		if( !client->readSocket() )
        {
            if( client->isClosed() )
            {
                printf( "ERROR: The socket closed unexpectedly! Exiting\n" );
                delete client;
                return false;
            }
        }
		printf( "read socket\n" );
		char* buffer = (char*)client->getBuffer();
        if( !buffer )
        {
            printf( "ERROR: buffer is NULL\n" );
            return false;
        }
		if( !strncmp( buffer, "COMPLETED!!!", 12 ) )
		{
			msg = buffer;
			client->writeSocket( "THANKS!!", 8 );
			printf( "Completed\n" );
			break;
		}
		if( !strncmp( buffer, "DENIED!!!", 9 ) )
		{
			printf( "ERROR: Access to %s denied!\n", hostname.c_str() );
			delete client;
			return false;
		}
        printf( "The current buffer is:\n\n%s\n\n", buffer );
		printf( "NOT YET COMPLETED\n" );
	}
	delete client;
	return true;
}

void commandReset()
{
#ifdef DADS_ARCH_IRIX
	system( "unset DTK_PORT" );
#else
	unsetenv( "DTK_PORT" );
#endif
	fprintf( logfile, "Entered commandReset()\n" );
	fflush( logfile );
	for( unsigned int i=0;i<common->hosts_reset.size();i++ )
	{
		sendMessage( common->hosts_reset[i], common->netport, "STOP\t\n" );
	}
	commandStop();
	for( unsigned int i=0;i<common->reset_commands.size();i++ )
		runCommand( common->reset_commands[i], true );
	if( common->start_dtkserver )
	{
		runCommand( "dtk-server -d" );
		for( unsigned int i=0;i<common->dtk_services.size();i++ )
		{
			if( i == 0 )
#ifdef DADS_ARCH_IRIX
				putenv( "DTK_PORT=NONE" );
#else
				setenv( "DTK_PORT", "NONE", 1 );
#endif
			string cmd = "dtk-server ";
			cmd += common->dtk_services[i];
			cmd += " -d";
			runCommand( cmd.c_str() );
		}
#ifdef DADS_ARCH_IRIX
		system( "unset DTK_PORT" );
#else
		unsetenv( "DTK_PORT" );
#endif
	}
	if( common->start_dListener )
	{
		runCommand( "dListener -l" );
	}
	for( unsigned int i=0;i<common->commands.size();i++ )
		runCommand( common->commands[i], true );
	for( unsigned int i=0;i<common->hosts_reset.size();i++ )
	{
		sendMessage( common->hosts_reset[i], common->netport, "RESET\t\n" );
	}
}

void commandServiceAdd( const string& param )
{
	vector<string> services;
	tokenize( param, services, ":\t" );
	for( unsigned int i=0;i<services.size();i++ )
	{
		string loadservice = services[i];
		if( loadservice[0] == '\"' )
			loadservice = loadservice.substr( 1, loadservice.size() );
		if( loadservice[loadservice.size()] == '\"' )
			loadservice = loadservice.substr( 0, loadservice.size() - 1 );
		loadservice = string( "dtk-loadService " ) + loadservice;
		runCommand( loadservice );
	}
}

void commandServiceRemove( const string& param )
{
	vector<string> services;
	tokenize( param, services, ":\t" );
	for( unsigned int i=0;i<services.size();i++ )
	{
		string unloadservice = services[i];
		if( unloadservice[0] == '\"' )
			unloadservice = unloadservice.substr( 1, unloadservice.size() );
		if( unloadservice[unloadservice.size()] == '\"' )
			unloadservice = unloadservice.substr( 0, unloadservice.size() - 1 );
		unloadservice = string( "dtk-unloadService " ) + unloadservice;
		runCommand( unloadservice );
	}
}

void interpretMessage( const string& message )
{
	timeval curtime;
	gettimeofday( &curtime, NULL );
	fprintf( logfile, "\n\n\nNEW MESSAGE: %s\n\n", ctime( &curtime.tv_sec ) );
	fprintf( logfile, "Enter interpretMessage( %s )\n", message.c_str() );
	fflush( logfile );
	vector<string> args;
	tokenize( message, args, "\t\n" );
	fprintf( logfile, "issue command...\n" );
	for( unsigned int i=0;i<args.size();i++ )
	{
		string command = args[i];
		if( command == "STOP" )
		{
			commandStop();
			return;
		}
		else if( command == "SHUTDOWN" )
		{
			fprintf( logfile, "\n\nReceived shutdown request\n\n" );
			sleep( 1 );
			delete client;
			delete serversocket;
			delete common;
			exit( 0 );
		}
		else if( command == "RESET" )
			commandReset();
		else if( command.substr( 0, 10 ) == "ADDSERVICE" )
			commandServiceAdd( command );
		else if( command.substr( 0, 10 ) == "DELSERVICE" )
			commandServiceRemove( command );
		else
			fprintf( logfile, "Unknown command: %s\n", command.c_str() );
	}
}

int serverLoop( dtkmasterCommon* common )
{
	while( 1 )
	{
		fd_set read_fds = master;
		fprintf( logfile, "\n\nWaiting for a connection...\n" );
		fflush( logfile );
		if( select( fdmax+1, &read_fds, NULL, NULL, NULL ) == -1 )
		{
			perror( "select" );
			return( -1 );
		}
		client = serversocket->acceptSocket();
		sockaddr_in socketaddress = client->getSocketAddress();
		string address = inet_ntoa( socketaddress.sin_addr );
		bool accepted = false;
		for( unsigned int i=0;i<common->hosts_accept.size();i++ )
		{
			hostent* hostentity = gethostbyname( common->hosts_accept[i].c_str() );
			if( !hostentity )
			{
				herror( "gethostbyname" );
				continue;
			}
			string check_address = inet_ntoa( *(struct in_addr *)hostentity->h_addr );
			if( address == check_address )
			{
				accepted = true;
				fprintf( logfile, "Accepted connection from %s\n", address.c_str() );
				break;
			}
		}
		if( !accepted )
		{
			fprintf( logfile, "ERROR: Host %s not allowed\n", address.c_str() );
			client->writeSocket( "DENIED!!!", 9 );
			sleep( 1 );
			delete client;
			continue;
		}
		fprintf( logfile, "client socket accepted fd: %d\n", client->getfd() );
		fflush( logfile );
		if( client )
		{
			if( !client->readSocket() )
            {
                if( client->isClosed() )
                {
                    printf( "ERROR: The socket closed unexpectedly!\n" );
                    delete client;
                    continue;
                }
            }
			fprintf( logfile, "read socket\n" );
			fflush( logfile );
			fprintf( logfile, "About to get buffer...\n" );
			fflush( logfile );
			string msg = (char*)client->getBuffer();
			fprintf( logfile, "buffer:\n\n%s\n", msg.c_str() );
			fflush( logfile );
/*			if( msg == "CHECK_FOR_RUNNING" )
			{
				fprintf( logfile, "Received status request - CURRENTLY_RUNNING!!!\n" );
				client->writeSocket( "CURRENTLY_RUNNING!!!", 20 );
				return;
			}*/
			if( msg.size() )
			{
				fprintf( logfile, "MESSAGE: %s\n", msg.c_str() );
				fflush( logfile );
				interpretMessage( msg );
				fprintf( logfile, "wrote socket - completed\n" );
				fflush( logfile );
				client->writeSocket( "COMPLETED!!!", 12 );
			}
			if( !client->readSocket() )
            {
                if( client->isClosed() )
                {
                    printf( "ERROR: The socket closed unexpectedly!\n" );
                    delete client;
                    continue;
                }
            }
			sleep( 1 );
			client->closeSocket();
			delete client;
		}
		else
		{
			fprintf( logfile, "ERROR: a socket was ready for reading but a dSocket\n"
					"\twas not returned when accepted.\n" );
			continue;
		}
		fflush( logfile );
	}
}

int shutdownServer()
{
	dSocket* clientsocket = new dSocket( common->netport, "localhost" );
	if( !clientsocket )
		return -1;
	if( !clientsocket->connectSocket() )
	{
		printf( "ERROR: Unable to shutdown dtk-masterServer - may not be running\n\n" );
		return -1;
	}
	printf( "\n\nAttempting to shutdown currently running dtk-masterServer\n\n" );
	if( !clientsocket->writeSocket( "SHUTDOWN\t\n", 10 ) )
	{
		printf( "ERROR: Unable to shutdown dtk-masterServer - may not be running\n\n" );
		delete clientsocket;
		return -1;
	}
	delete clientsocket;
	return 0;
}

int main( int argc, char** argv )
{
	logfile = NULL;
	common = new dtkmasterCommon;
	if( loadConfig( common ) )
	{
		printf( "ERROR: Configuration errors - please use -h "
				"parameter to see usage\n" );
		exit( -1 );
	}
	if( argc > 2 )
		usage( -1 );
	else if( argc == 2 )
	{
		string arg = argv[1];
		if( arg == "--help" || arg == "-h" )
			usage( 0 );
		else if( arg == "--shutdown" || arg == "-s" )
			exit( shutdownServer() );
		else
			usage( -1 );
	}

	FILE* temp = freopen( "/dev/null", "w", stdout );
	freopen( "/dev/null", "w", stderr );
	dSocket* client = new dSocket( common->netport, "localhost" );
	if( client )
	{
		if( client->connectSocket() )
		{
			printf( "ERROR: localhost:%d port already in use\n"
					"dtk-masterServer is probably already running\n", common->netport );
			delete client;
			exit( -1 );
		}
		delete client;
	}
	fclose( temp );

	int pid = fork();
	if( pid < 0 )
	{
		printf( "ERROR: Unable to fork process - exiting\n" );
		exit( -1 );
	}
	else if( pid > 0 )
		exit( 0 );
	if( setsid() == -1 )
		printf( "ERROR: Unable to become session leader\n" );
	if( common->logging )
	{
		if( !(logfile = freopen( common->loglocation.c_str(), "w", stdout ) ) )
			printf( "WARNING: Unable to open %s - continuing...\n",
					common->loglocation.c_str() );
		else if( !freopen( common->loglocation.c_str(), "a", stderr ) )
			printf( "WARNING: Unable to pipe stderr into %s - continuing...\n",
					common->loglocation.c_str() );
	}
	if( !logfile )
		logfile = stderr;
	fprintf( logfile, "%s\n", "dtk-masterServer successfully daemonized" );
	if( serverInit( common->netport ) )
		fprintf( logfile, "ERROR: Unable to initialize server state using port %d\n",
				common->netport );
	umask( 0 );
	chdir( "/" );
//	signal( SIGCHLD, sigchild_handler );
	struct sigaction sa;
	sa.sa_handler = sigchild_handler;
#ifdef DADS_ARCH_IRIX
	sigemptyset( &sa.sa_mask );
	sigaddset( &sa.sa_mask, SA_NOCLDWAIT );
#else
	sigemptyset( &sa.sa_mask );
#endif
	sa.sa_flags = SA_RESTART;
	if( sigaction( SIGCHLD, &sa, NULL ) == -1 )
	{
		perror( "sigaction" );
		exit(1);
	}
	fflush( logfile );
	string curpath = "/usr/local/bin:/usr/bin:/bin:/usr/sbin";
	for( unsigned int i=0;i<common->search_path.size();i++ )
	{
		curpath += ":";
		curpath += common->search_path[i];
	}
	printf( "PATH=%s", curpath.c_str() );
#ifdef DADS_ARCH_IRIX
	putenv( "SHELL=/bin/bash" );
	string pathenv = string( "PATH=" ) + curpath;
	putenv( pathenv.c_str() );
#else
	setenv( "SHELL", "/bin/bash", 1 );
	string pathenv = curpath;
	setenv( "PATH", pathenv.c_str(), 1 );
#endif
	string newpath = getenv( "PATH" );
	int retval = serverLoop( common );
	fclose( logfile );
	delete common;
    return retval;
}

