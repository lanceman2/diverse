#include "dSocket.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <vector>

#define DEVICE "/dev/ttyS1"
#define NETPORT 65530
#define LOGLOCATION "/tmp/displayswitcher.log"

using namespace std;

dSocket* serversocket;
dSocket* client;
fd_set master;
int fdmax;
FILE* logfile;
int switchfd;

void sigchild_handler( int s )
{
	while( wait( NULL ) > 0 )
		signal( SIGCHLD, sigchild_handler );
}

void tokenize( const string& str, vector<string>& tokens, const string& delimiters = " " )
{
	size_t startpos = str.find_first_not_of( delimiters, 0 );
	size_t endpos = str.find_first_of( delimiters, startpos );
	while( startpos != string::npos )
	{
		tokens.push_back( str.substr( startpos, endpos - startpos) );
		startpos = str.find_first_not_of( delimiters, endpos );
		endpos = str.find_first_of( delimiters, startpos );
		if( startpos != string::npos && endpos == string::npos )
			endpos = str.size();
	}
}

int serverInit( int port )
{
	serversocket = new dSocket( port );
	if( !serversocket )
	{
		printf( "ERROR: Unable to create socket\n" );
		return -1;
	}
	serversocket->bindSocket();
	serversocket->listenSocket();
	FD_SET( serversocket->getfd(), &master );
	fdmax = serversocket->getfd();
	fprintf( logfile, "server fd: %d\n", serversocket->getfd() );
	return 0;
}

int openSwitch()
{
	struct termios datacomm;
	datacomm.c_iflag = 0;
	datacomm.c_oflag = 0;
	datacomm.c_lflag = 0;
	datacomm.c_cflag = CS8 | CREAD | CLOCAL;
	cfsetispeed( &datacomm, B9600 );
	cfsetospeed( &datacomm, B9600 );
	for( int i=0;i<NCCS;i++ )
		datacomm.c_cc[i] = 0;
	switchfd = open( DEVICE, O_RDWR | O_NDELAY );
	if( switchfd == -1 )
	{
		printf( "ERROR: Unable to open %s\n", DEVICE );
		return -1;
	}
	if( tcsetattr( switchfd, TCSANOW, &datacomm ) == -1 )
	{
		printf( "ERROR: Unable to set parameters for the serial device\n" );
		return -1;
	}
	if( tcflush( switchfd, TCIOFLUSH ) == -1 )
	{
		printf( "ERROR: Failed to flush the serial device\n" );
		return -1;
	}
	FD_SET( switchfd, &master );
	return 0;
}

string readSwitch()
{
	string feedback;
	char msg[256];
	bool reading = true;
	while( reading )
	{
		int size = 0;
		if( ( size = read( switchfd, msg, 255 ) ) == -1 )
		{
			perror( "read" );
			return feedback;
		}
		if( size )
		{
			msg[size] = '\0';
			printf( "size: %d\n", size );
			if( msg[0] == '|' )
				continue;
			feedback += msg;
			size_t pos = feedback.find_first_of( "\r", 0 );
			if( pos != string::npos )
			{
				printf( "carriage return found\n" );
				if( feedback[pos+1] == '\n' )
				{
					printf( "newline - linefeed found\n" );
					reading = false;
				}
			}
			printf( "%s\n", feedback.c_str() );
			fflush( logfile );
		}
	}
	return feedback;
}

string sendCommand( const string& cmd )
{
	printf( "sendCommand: %s\n", cmd.c_str() );
	string feedback;
	if( write( switchfd, cmd.c_str(), cmd.size() ) == -1 )
	{
		perror( "write" );
		return feedback;
	}
	return readSwitch();
}

string tieInput( int input, int output )
{
	char temp[16];
	sprintf( temp, "%d*%d!", input, output );
	return sendCommand( temp );
}

string tieMultiInput( vector<int> multi )
{
	string cmd = " Q";
	cmd[0] = 0x1B;
	for( unsigned int i=0;i<multi.size();i++ )
	{
		char temp[16];
		sprintf( temp, "%d*%d!", multi[i], multi[i+1] );
		i++;
		cmd += string( temp );
	}
	cmd += string( "\r" );
	return sendCommand( cmd );
}

string recallPreset( int input )
{
	char temp[16];
	sprintf( temp, "%d.", input );
	return sendCommand( temp );
}

string lockPanel( bool lock )
{
	if( lock )
		return sendCommand( "X" );
	else
		return sendCommand( "x" );
}

string viewOutputTie( int output = 0 )
{
	char temp[16];
	sprintf( temp, "V%d%%", output );
	return sendCommand( temp );
}

string viewPresetConfig( int input = 0 )
{
	char temp[16];
	sprintf( temp, "V%d.", input );
	return sendCommand( temp );
}

string infoRequest()
{
	return sendCommand( "I" );
}

string interpretMessage( string msg )
{
	printf( "interpretMessage: %s\n", msg.c_str() );
	vector<string> ties;
	if( msg.substr( 0, 3 ) == "TIE" )
	{
		msg = msg.substr( 3 );
		tokenize( msg, ties, " \t\n" );
		if( ties.size() % 2 )
			return "ERROR: Ties must be in input/output value pairs\n";
		vector<int> tievalues;
		for( unsigned int i=0;i<ties.size();i++ )
		{
			if( ties[i].find_first_not_of( "0123456789", 0 ) != string::npos )
				return "ERROR: Ties must be integer values\n";
			tievalues.push_back( atoi( ties[i].c_str() ) );
		}
		return tieInput( tievalues[0], tievalues[1] );
	}
	else if( msg.substr( 0, 5 ) == "MULTI" )
	{
		msg = msg.substr( 5 );
		tokenize( msg, ties, " \t\n" );
		if( ties.size() % 2 )
			return "ERROR: Ties must be in input/output value pairs\n";
		vector<int> tievalues;
		for( unsigned int i=0;i<ties.size();i++ )
		{
			if( ties[i].find_first_not_of( "0123456789", 0 ) != string::npos )
				return "ERROR: Ties must be integer values\n";
			tievalues.push_back( atoi( ties[i].c_str() ) );
		}
		return tieMultiInput( tievalues );
	}
	else if( msg.substr( 0, 4 ) == "LOCK" )
		return lockPanel( true );
	else if( msg.substr( 0, 4 ) == "UNLOCK" )
		return lockPanel( false );
	else if( msg.substr( 0, 6 ) == "RECALL" )
	{
		msg = msg.substr( 6 );
		tokenize( msg, ties, " \t\n" );
		if( !ties[0].size() )
			return "ERROR: Recall preset requires an integer value\n";
		if( ties[0].find_first_not_of( "0123456789", 0 ) != string::npos )
			return "ERROR: Recall presets must be integer values\n";
		return recallPreset( atoi( ties[0].c_str() ) );
	}
	else if( msg.substr( 0, 10 ) == "VIEWOUTPUT" )
	{
		msg = msg.substr( 10 );
		int output = atoi( msg.c_str() );
		return viewOutputTie( output );
	}
	else if( msg.substr( 0, 10 ) == "VIEWPRESET" )
	{
		msg = msg.substr( 10 );
		int input = atoi( msg.c_str() );
		return viewPresetConfig( input );
	}
	else if( msg.substr( 0, 4 ) == "INFO" )
	{
		msg = msg.substr( 4 );
		return infoRequest();
	}
	string retval = string( "ERROR: Invalid message - " ) + msg;
	return retval;
}

int serverLoop()
{
	while( 1 )
	{
		fd_set read_fds = master;
		fprintf( logfile, "Waiting for a connection...\n" );
		fflush( logfile );
		if( select( fdmax+1, &read_fds, NULL, NULL, NULL ) == -1 )
		{
			perror( "select" );
			return( -1 );
		}
		if( FD_ISSET( switchfd, &master ) )
		{
			char msg[256];
			int size = read( switchfd, msg, 255 );
			if( size == -1 )
				perror( "read" );
			printf( "switch: %s\n", msg );
		}
		if( !client && FD_ISSET( serversocket->getfd(), &master ) )
		{
			if( ( client = serversocket->acceptSocket() ) )
			{
				int clientfd = client->getfd();
				FD_SET( clientfd, &master );
				if( clientfd > serversocket->getfd() )
					fdmax = clientfd;
				fprintf( logfile, "client socket accepted fd: %d\n", client->getfd() );
				fflush( logfile );
			}
			else
			{
				fprintf( logfile, "ERROR: client socket not accepted\n" );
				fflush( logfile );
			}
		}
		if( client )
		{
			printf( "client exists\n" );
			if( FD_ISSET( client->getfd(), &master ) )
			{
				printf( "client is set\n" );
				if( !client->readSocket() )
				{
					if( client->isClosed() )
						printf( "WARNING: Connection closed\n" );
					else
						printf( "ERROR: Connection problem\n" );
					FD_CLR( client->getfd(), &master );
					fdmax = max( serversocket->getfd(), switchfd );
					delete client;
					client = NULL;
					continue;
				}
				fprintf( logfile, "read socket\n" );
				fflush( logfile );
				string msg = (char*)client->getBuffer();
				if( msg.size() )
				{
					fprintf( logfile, "MESSAGE: %s\n", msg.c_str() );
					fflush( logfile );
					string response = interpretMessage( msg );
					printf( "response: %s\n", response.c_str() );
					fprintf( logfile, "wrote socket - completed\n" );
					fflush( logfile );
					if( response.size() )
						client->writeSocket( response.c_str(), response.size() );
					else
						client->writeSocket( "COMPLETED!!!", 12 );
				}
			}
		}
		else
		{
			fprintf( logfile, "ERROR: a socket was ready for reading but a dSocket\n"
					"\twas not returned when accepted.\n" );
			continue;
		}
		fflush( logfile );
		sleep( 1 );
	}
}

int main( int argc, char** argv )
{
	client = NULL;
	switchfd = 0;
	if( !(logfile = freopen( LOGLOCATION, "w", stdout ) ) )
		printf( "WARNING: Unable to open %s - continuing...\n",
				LOGLOCATION );
	else if( !freopen( LOGLOCATION, "a", stderr ) )
		printf( "WARNING: Unable to pipe stderr into %s - continuing...\n",
				LOGLOCATION );
	int childpid = fork();
	if( childpid < 0 )
	{
		fprintf( logfile, "ERROR: Unable to fork process\n" );
		exit( -1 );
	}
	if( childpid > 0 )
	{
		fprintf( logfile, "Process daemonized...\n" );
		signal( SIGCHLD, sigchild_handler );
		exit( 0 );
	}
	if( openSwitch() == -1 )
		exit( -1 );
	serverInit( NETPORT );
	serverLoop();
	if( switchfd )
		close( switchfd );
}

