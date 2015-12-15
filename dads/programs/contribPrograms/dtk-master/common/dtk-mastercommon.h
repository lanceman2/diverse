/************************************************************************
* dtk-masterServer
*
* daemon to start, stop, and reset other DTK and DADS daemons.
*
* by Patrick Shinpaugh 9/15/2004
************************************************************************/

#include "dSocket.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <netdb.h>
#include <sys/wait.h>

#define DEFAULT_LOG_LOCATION "/tmp/dtk-masterserver.log"
#define DEFAULT_NET_PORT 65535
#define DEFAULT_SEARCH_PATH "/usr/local/diverse/dtk/bin"

using namespace std;

class dtkmasterCommon
{
public:
	dtkmasterCommon()
	{
		netport = DEFAULT_NET_PORT;
		start_dtkserver = start_dListener = logging = false;
		loglocation = DEFAULT_LOG_LOCATION;
	};
	int netport;
	vector<string> hosts_accept;
	vector<string> hosts_reset;
	vector<string> dtk_services;
	vector<string> apps_accept;
	vector<string> services_accept;
	vector<string> commands;
	vector<string> reset_commands;
	vector<string> search_path;
	bool start_dtkserver;
	bool start_dListener;
	bool logging;
	string loglocation;
};

void sigchild_handler( int s )
{
	while( wait( NULL ) > 0 )
		;
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

int loadConfig( dtkmasterCommon* common )
{
	bool error = false;
	map<string,vector<string> > configuration;
	ifstream configfile( "/etc/dtk-masterServer.conf" );
	if( !configfile.is_open() )
	{
		printf( "ERROR: Unable to open configuration file "
				"\t/etc/dtk-masterServer.conf\n - exiting!\n\n" );
		exit( -1 );
	}
	char buffer[256];
	while( !configfile.eof() )
	{
		configfile.getline( buffer, 255 );
		string entry = buffer;
		vector<string> tokens;
		size_t startpos = entry.find_first_not_of( " \t\r\n", 0 );
		size_t endpos = entry.find_first_of( " \t\r\n", startpos );
		string key;
		if( startpos != string::npos )
		{
			if( endpos == string::npos )
				endpos = entry.size();
			key = entry.substr( startpos, endpos - startpos );
		}
		else
			continue;
		if( key[0] == '#' )
			continue;
		startpos = entry.find_first_not_of( " \t\r\n", endpos );
		endpos = entry.size();
		string values;
		values = entry.substr( startpos, endpos - startpos );
		if( key == "commands" || key == "reset_commands" )
		{
			tokenize( values, tokens, ":" );
		}
		else
			tokenize( values, tokens, " :\t\r\n" );
		if( !tokens.size() )
		{
			printf( "ERROR: Entry does not contain a key value pair!\n"
					"%s\n", entry.c_str() );
			error = true;
			continue;
		}
		configuration[key] = tokens;
	}
	map<string,vector<string> >::iterator itr = configuration.begin();
	for( ;itr!=configuration.end();itr++ )
	{
		if( itr->first != "port" )
			;
		else if( itr->first != "hosts_accept" )
			;
		else if( itr->first != "hosts_reset" )
			;
		else if( itr->first != "start_dtkserver" )
			;
		else if( itr->first != "start_dListener" )
			;
		else if( itr->first != "dtk_services" )
			;
		else if( itr->first != "logging" )
			;
		else if( itr->first != "logfile" )
			;
		else if( itr->first != "apps_accept" )
			;
		else if( itr->first != "services_accept" )
			;
		else if( itr->first != "commands" )
			;
		else if( itr->first != "search_path" )
			;
		else
		{
			printf( "ERROR: Invalid configuration option\n"
					"\t%s\n", itr->first.c_str() );
			return -1;
		}
	}
	if( ( itr = configuration.find( "port" ) ) != configuration.end() )
	{
		if( itr->second.size() > 1 )
		{
			string values;
			for( unsigned int i=0;i<itr->second.size();itr++ )
				values += itr->second[i];
			printf( "ERROR: Too many values for port\n"
					"\t%s\n", values.c_str() );
			error = true;
		}
		printf( "port: %s\n", itr->second[0].c_str() );
		common->netport = atoi( itr->second[0].c_str() );
	}
	else
		common->netport = DEFAULT_NET_PORT;
	if( ( itr = configuration.find( "hosts_accept" ) ) != configuration.end() )
	{
		for( unsigned int i=0;i<itr->second.size();i++ )
			tokenize( itr->second[i], common->hosts_accept, " :,\t" );
	}
	if( ( itr = configuration.find( "hosts_reset" ) ) != configuration.end() )
	{
		for( unsigned int i=0;i<itr->second.size();i++ )
			tokenize( itr->second[i], common->hosts_reset, " :,\t" );
	}
	if( ( itr = configuration.find( "start_dtkserver" ) ) != configuration.end() )
	{
		if( itr->second.size() > 1 )
		{
			string values;
			for( unsigned int i=0;i<itr->second.size();itr++ )
				values += itr->second[i];
			printf( "ERROR: Too many values for port\n"
					"\t%s\n", values.c_str() );
			error = true;
		}
		string value = itr->second[0];
		for( unsigned int i=0;i<value.size();i++ )
			value[i] = tolower( value[i] );
		if( value == "yes" || value == "true" )
			common->start_dtkserver = true;
		else if( value == "no" || value == "false" )
			common->start_dtkserver = false;
		else
		{
			printf( "ERROR: Invalid value for start_dtk-server\n"
					"\t%s\n", value.c_str() );
			error = true;
		}
	}
	else
		common->start_dtkserver = false;
	if( ( itr = configuration.find( "start_dListener" ) ) != configuration.end() )
	{
		if( itr->second.size() > 1 )
		{
			string values;
			for( unsigned int i=0;i<itr->second.size();itr++ )
				values += itr->second[i];
			printf( "ERROR: Too many values for port\n"
					"\t%s\n", values.c_str() );
			error = true;
		}
		string value = itr->second[0];
		for( unsigned int i=0;i<value.size();i++ )
			value[i] = tolower( value[i] );
		if( value == "yes" || value == "true" )
			common->start_dListener = true;
		else if( value == "no" || value == "false" )
			common->start_dListener = false;
		else
		{
			printf( "ERROR: Invalid value for start_dListener\n"
					"\t%s\n", value.c_str() );
			error = true;
		}
	}
	else
		common->start_dListener = false;
	if( ( itr = configuration.find( "dtk_services" ) ) != configuration.end() )
	{
		for( unsigned int i=0;i<itr->second.size();i++ )
			tokenize( itr->second[i], common->dtk_services, " :,\t" );
	}
	if( ( itr = configuration.find( "logging" ) ) != configuration.end() )
	{
		if( itr->second.size() > 1 )
		{
			string values;
			for( unsigned int i=0;i<itr->second.size();itr++ )
				values += itr->second[i];
			printf( "ERROR: Too many values for port\n"
					"\t%s\n", values.c_str() );
			error = true;
		}
		string value = itr->second[0];
		for( unsigned int i=0;i<value.size();i++ )
			value[i] = tolower( value[i] );
		if( value == "yes" || value == "true" )
			common->logging = true;
		else if( value == "no" || value == "false" )
			common->logging = false;
		else
		{
			printf( "ERROR: Invalid value for logging\n"
					"\t%s\n", value.c_str() );
			error = true;
		}
	}
	else
		common->logging = false;
	if( ( itr = configuration.find( "logfile" ) ) != configuration.end() )
	{
		if( itr->second.size() > 1 )
		{
			string values;
			for( unsigned int i=0;i<itr->second.size();itr++ )
				values += itr->second[i];
			printf( "ERROR: Too many values for port\n"
					"\t%s\n", values.c_str() );
			error = true;
		}
		common->loglocation = itr->second[0];
		printf( "logfile: %s\n", common->loglocation.c_str() );
	}
	else
		common->loglocation = DEFAULT_LOG_LOCATION;
	if( ( itr = configuration.find( "apps_accept" ) ) != configuration.end() )
	{
		for( unsigned int i=0;i<itr->second.size();i++ )
			tokenize( itr->second[i], common->apps_accept, " :,\t" );
	}
	if( ( itr = configuration.find( "services_accept" ) ) != configuration.end() )
	{
		for( unsigned int i=0;i<itr->second.size();i++ )
			tokenize( itr->second[i], common->services_accept, " :,\t" );
	}
	if( ( itr = configuration.find( "commands" ) ) != configuration.end() )
	{
		for( unsigned int i=0;i<itr->second.size();i++ )
			tokenize( itr->second[i], common->commands, ":" );
	}
	if( ( itr = configuration.find( "reset_commands" ) ) != configuration.end() )
	{
		for( unsigned int i=0;i<itr->second.size();i++ )
			tokenize( itr->second[i], common->reset_commands, ":" );
	}
	if( ( itr = configuration.find( "search_path" ) ) != configuration.end() )
	{
		for( unsigned int i=0;i<itr->second.size();i++ )
			tokenize( itr->second[i], common->search_path, " :,\t" );
	}
	else
		common->search_path.push_back( DEFAULT_SEARCH_PATH );
	common->hosts_accept.push_back( "localhost" );
	if( error )
	{
		printf( "GONNA EXIT!!!\n" );
		exit( -1 );
	}
	return 0;
}

string getHost( string hostname )
{
	hostent* entity = gethostbyname( hostname.c_str() );
	if( !entity )
		printf( "ERROR: Unable to get host info for %s\n", hostname.c_str() );
	return entity->h_addr;
}
