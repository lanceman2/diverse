/************************************************************************
* dtk-masterControl
*
* command to start, stop, and reset other DTK and DADS daemons through
* communication with dtk-masterServer.
*
* by Patrick Shinpaugh 10/4/2004
************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "../common/dtk-mastercommon.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <netdb.h>
#include <sys/wait.h>

using namespace std;

dtkmasterCommon* common;
dSocket* clientsocket;

void usage( int code )
{
	printf( "Usage: dtk-masterControl [options]\n\n" );
	printf( "dtk-masterControl is a command which sends messages to the \n"
			"dtk-masterServer running on the local or another machine. It \n"
			"can be used to reset the specified machine to its default settings, \n"
			"add or remove services running on a machine, or stop all servers \n"
			"and services running on a machine. \n\n"
			"When resetting or stopping a machine, all machines listed in the \n"
			"hosts_reset parameter of the /etc/dtk-masterServer.conf file will \n"
			"be forced to reset or stop also.\n\n"
			"Any service may be removed from running on the specified machine, \n"
			"but only authorized services can be started on the specified machine. \n"
			"These are listed in the /etc/dtk-masterServer.conf file with the \n"
			"services_accept parameter. If services are added and deleted, deletions \n"
			"will occur first followed by additions. If services are added and a reset\n"
			"is also requested, the reset will occur first followed by the service\n"
			"additions\n\n" );
	printf( "dtk-masterControl uses the /etc/dtk-masterserver.conf file to determine\n"
			"how it communicates and what it is capable of doing. The following \n"
			"options are used with dtk-masterControl.\n\n" );
	printf( "--host | -H hostname The hostname of the machine which will receive the \n"
			"		              commands from dtk-masterControl. The default is \n"
			"		              the local machine.\n\n" );
	printf( "--addsvc|-a service  Add/start the specified service if it is not already \n"
			"		              running on the specified machine.\n\n" );
	printf( "--delsvc|-d service  Remove the specified service if it is currently \n"
			"		              running on the specified machine.\n\n" );
	printf( "--reset | -R	      Reset the servers running on the specified machine. \n"
			"		              This will also reset all machines listed in the \n"
			"		              hosts_reset parameter on the specified machine.\n\n" );
	printf( "--stop | -S          Stops all dtk-server and dListener daemons running on \n"
			"                     the specified machine. This will also stop all machines \n"
			"                     listed in the hosts_reset parameter on the specified \n"
			"                     machine.\n\n" );
	printf( "--list | -l	      List services that are currently running on \n"
			"		              the specified machine.\n\n" );
	printf( "--paramlist | -L     List services, apps, reset defaults from the \n"
			"		              configuration file for the specified machine.\n\n" );
	printf( "--verbose | -v       Prints verbose output for each command. The default \n"
			"                     is to print \"Success\" or \"Failure\" for each \n"
			"                     command.\n\n" );
	printf( "--help | -h	      Show this usage information.\n" );
	exit( code );
}

int clientsocketInit( const string& hostname, int port )
{
	clientsocket = new dSocket( port, hostname );
	if( !clientsocket )
		return -1;
	if( !clientsocket->connectSocket() )
	{
		printf( "ERROR: Unable to connect to %s\n", hostname.c_str() );
		return -1;
	}
	printf( "clientsocket fd: %d\n", clientsocket->getfd() );
	return 0;
}

int interpretArgs( int argc, char** argv )
{
	string hostname = "localhost";
	vector<string> addservicenames;
	vector<string> delservicenames;
	bool addsvc = false;
	bool delsvc = false;
	bool reset = false;
	bool stop = false;
	bool listsvc = false;
	bool listparam = false;
	bool verbose = false;
	for( int i=1;i<argc;i++ )
	{
		string param = argv[i];
		if( param == "--help" || param == "-h" )
			usage( 0 );
		else if( param == "--host" || param == "-H" )
		{
			if( i++ < argc )
			{
				hostname = argv[i];
				continue;
			}
			else
			{
				printf( "ERROR: must provide a hostname\n\n" );
			}
		}
		else if( param == "--add" || param == "-a" )
		{
			addsvc = true;
			if( i++ < argc )
			{
				addservicenames.push_back( argv[i] );
				continue;
			}
			else
			{
				printf( "ERROR: must provide a servicename\n\n" );
			}
		}
		else if( param == "--del" || param == "-d" )
		{
			delsvc = true;
			if( i++ < argc )
			{
				delservicenames.push_back( argv[i] );
				continue;
			}
			else
			{
				printf( "ERROR: must provide a servicename\n\n" );
			}
		}
		else if( param == "--reset" || param == "-R" )
			reset = true;
		else if( param == "--stop" || param == "-S" )
			stop = true;
		else if( param == "--list" || param == "-l" )
			listsvc = true;
		else if( param == "--parmalist" || param == "-L" )
			listparam = true;
		else if( param == "--verbose" || param == "-v" )
			verbose = true;
		else
		{
			printf( "ERROR: Invalid parameter %s.\n\n", param.c_str() );
			usage( -1 );
		}
	}
	string msg;
	if( verbose )
		msg += "VERBOSE\t";
	if( reset )
		msg += "RESET\t";
	if( delsvc )
	{
		msg += "DELSERVICE";
		for( unsigned int i=0;i<delservicenames.size();i++ )
		{
			msg += ":";
			msg += delservicenames[i];
		}
		msg += "\t";
	}
	if( addsvc )
	{
		msg += "ADDSERVICE";
		for( unsigned int i=0;i<addservicenames.size();i++ )
		{
			msg += ":";
			msg += addservicenames[i];
		}
		msg += "\t";
	}
	if( listsvc )
		msg += "LISTSERVICE\t";
	if( listparam )
		msg += "LISTPARAM\t";
	if( stop )
		msg = "STOP\t";
	msg += "\n";
	if( clientsocketInit( hostname, common->netport ) )
	{
		printf( "ERROR: Unable to initialize socket state using port %d"
				"for host %s\n", common->netport, hostname.c_str() );
		return 1;
	}
	clientsocket->writeSocket( msg.c_str(), msg.size() );
	printf( "wrote socket - data\n" );
	while( msg != "COMPLETED" )
	{
		if( !clientsocket->readSocket() )
        {
            if( clientsocket->isClosed() )
            {
                printf( "ERROR: The socket closed unexpectedly! Exiting\n" );
            	delete clientsocket;
                return 2;
            }
        }
		printf( "read socket\n" );
		char* buffer = (char*)clientsocket->getBuffer();
		if( !strncmp( buffer, "COMPLETED!!!", 12 ) )
		{
			msg = "COMPLETED";
			clientsocket->writeSocket( "THANKS!!", 8 );
			printf( "thank you\n" );
			return 0;
		}
		if( !strncmp( buffer, "DENIED!!!", 9 ) )
		{
			printf( "ERROR: Access to %s denied!\n", hostname.c_str() );
			break;
		}
        printf( "The current buffer is:\n\n%s\n\n", buffer );
		printf( "NOT YET COMPLETED\n" );
	}
	delete clientsocket;
    return 0;
}

int main( int argc, char** argv )
{
	if( argc < 2 )
		usage( 0 );
	common = new dtkmasterCommon;
	if( loadConfig( common ) )
	{
		printf( "ERROR: Configuration errors - please use -h "
				"parameter to see usage\n" );
		exit( -1 );
	}
	return interpretArgs( argc, argv );
}

