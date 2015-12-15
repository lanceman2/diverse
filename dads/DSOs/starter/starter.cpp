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
#include <iostream>
#include <dtk/dtkDSO_loader.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <map>
#include <dPostOffice.h>
#include <dMessage.h>
#include "fileParser.h"

using namespace std;

void sigchild_handler( int s )
{
	while( wait( NULL ) > 0 )
		;
}

vector<string> COMPUTERS;

// Modified from googled source code found on informit.com 
// in a book about the proc file system
vector<string> get_processargs(pid_t pid)
{
	int fd;
	char filename[24];
	char arg_list[1024];
	size_t length;
	char* next_arg;
	snprintf(filename, sizeof(filename), "/proc/%d/cmdline", (int)pid);
	fd = open (filename, O_RDONLY);
	length = read(fd, arg_list, sizeof(arg_list));
	close(fd);
	arg_list[length] = '\0';
	next_arg = arg_list;
	vector<string> Temp;
	while (next_arg < arg_list + length)
	{
		Temp.push_back(string(next_arg));

		next_arg += strlen(next_arg)+1;
	}
	return Temp;
}

class Starter : public dtkAugment
{
	public:

	Starter(dtkManager* m);
	~Starter(void) ;
	void addEnv( dPostOffice* postoffice, const string& Name );
	void addEnv( dPostOffice* postoffice, const string& Name,
			const string& envvars );
	void addProgram( dPostOffice* postoffice, vector<Program> programs,
		    int uid, const string& buff );
    void tokenize( const string& str, vector<string>& tokens,
            const string& delimiters = " " );
	void parseEnv();
	vector<string> getHosts();

	private:
	map<string,dPostOffice*>  m_postoffice;
	vector<string> m_hosts;
	string m_hostname;
	dMessage*     m_message;
	string        m_name;
	fileParser*   m_fileparser;
};

// Parses the DADS_CLIENTS variable and returns a vector of arguments
// Basically takes the pain out of parsing an env var
vector<string> Starter::getHosts()
{
	char* Hosts = getenv("DADS_CLIENTS");
	vector<string> RETURN;
	if (Hosts == NULL)
		return RETURN;
	char* Temp = strdup(Hosts);
	printf("Hosts is %s\n", Hosts);
	char* Val = strtok(Temp, ":");
	while (Val != NULL)
	{
		RETURN.push_back(string(Val));
		Val = strtok(NULL, ":");
	}
	for (unsigned int i=0; i< RETURN.size(); i++)
	{
		printf("%s \n", RETURN[i].c_str());
	}
	free(Temp);
	return RETURN;
}

void Starter::addEnv( dPostOffice* postoffice, const string& Name )
{
	char* temp = getenv( Name.c_str() );
	if ( temp == NULL )
		return;
	addEnv( postoffice, Name, temp );
}

void Starter::addEnv( dPostOffice* postoffice, const string& Name,
		const string& envvars )
{
	if( envvars.size() == 0 )
		return;
	m_message->clear();
	m_message->setFrom( "CONSOLE" );
	m_message->setTo( "DAEMON_SERVER" );
	m_message->setSubject( "ADD_ENV" );
	m_message->addString( Name );
	m_message->addString( envvars );
	postoffice->sendMessage( m_message );
	printf("%s\n", m_message->getMessage().c_str());
	m_message->clear();
}

void Starter::addProgram( dPostOffice* postoffice, vector<Program> programs,
		int uid, const string& buff)
{
	vector<Program>::iterator itr;
	for( itr=programs.begin();itr!=programs.end();itr++ )
	{
		//Now go through and construct the actual message
		m_message->clear();
		m_message->setTo("DAEMON_SERVER");
		m_message->setFrom("CONSOLE");
		m_message->setSubject("START_PROCESS");
		m_message->addInt( uid );
		m_message->addString( buff );
		vector<string> args = (*itr).Args;
		m_message->addInt( args.size() );
		vector<string>::iterator argsitr;
		for( argsitr=args.begin();argsitr!=args.end();argsitr++)
			m_message->addString( *argsitr );
		postoffice->sendMessage(m_message);
	}
}

void Starter::tokenize( const string& str, vector<string>& tokens, const string& delimiters )
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

// Parses environment variables and stores them in the
// map associated with the fileParser
void Starter::parseEnv()
{
	vector<string>::iterator itr;
	for( itr=m_hosts.begin();itr!=m_hosts.end();itr++ )
	{
		hostEnv hostenvirons;
		if( m_fileparser->getHostEnv( *itr ).getHostName()
			       	== "BAD_HOSTNAME" )
			hostenvirons.setHostName( *itr );
		else
			hostenvirons = m_fileparser->getHostEnv( *itr );

		char* env = getenv( "DADS_DSO_FILES" );
		if( env != NULL )
			hostenvirons.addDSO( env );
		env = NULL;

		string temp;
		size_t len;
		if( ( len = (*itr).find( ".", 0 ) ) != string::npos )
			temp = (*itr).substr( 0, len );
		else
			temp = *itr;
		string envvar = "DADS_DSO_FILES_" + temp;
		env = getenv( envvar.c_str() );
		if( env != NULL )
			hostenvirons.addDSO( env );

		envvar = "DADS_ENV";
		env = getenv( envvar.c_str() );
		if( env != NULL )
			hostenvirons.addEnv( env );

		envvar = "DADS_ENV_" + temp;
		env = getenv( envvar.c_str() );
		if( env != NULL )
			hostenvirons.addEnv( env );

		envvar = "DADS_DISPLAY_" + temp;
		env = getenv( envvar.c_str() );
		if( env != NULL )
			hostenvirons.setDisplay( env );

		envvar = "DADS_PROGRAMS";
		env = getenv( envvar.c_str() );
		if( env != NULL )
		{
            string progenv = env;
            vector<string> progs;
            tokenize( progenv, progs, ":" );
            Program aprogram;
            for( unsigned int i=0;i<progs.size();i++ )
            {
                vector<string> args;
                tokenize( progs[i], args, " " );
                aprogram.Name = args[0];
                aprogram.NumArgs = args.size();
                aprogram.Args = args;
    			hostenvirons.addProgram( aprogram );
            }
/*			Program AProgram;
			char* Name = strtok(env," ");
			if (Name !=NULL)
				AProgram.Name = string(Name);
			AProgram.Args.push_back(Name);
			char* Value = NULL;
			Value = strtok(NULL, " ");
			while (Value !=NULL)
			{
				AProgram.Args.push_back(string(Value));
				Value = strtok(NULL, " ");
			}
                        AProgram.NumArgs = AProgram.Args.size();
			hostenvirons.addProgram( AProgram );*/
		}

		envvar = "DADS_PROGRAMS_" + temp;
		env = getenv( envvar.c_str() );
		if( env != NULL )
		{
            string progenv = env;
            vector<string> progs;
            tokenize( progenv, progs, ":" );
            Program aprogram;
            for( unsigned int i=0;i<progs.size();i++ )
            {
                vector<string> args;
                tokenize( progs[i], args, " " );
                aprogram.Name = args[0];
                aprogram.NumArgs = args.size();
                aprogram.Args = args;
    			hostenvirons.addProgram( aprogram );
            }
/*			Program AProgram;
			char* Name = strtok(env," ");
			if (Name !=NULL)
				AProgram.Name = string(Name);
			AProgram.Args.push_back(Name);
			char* Value = NULL;
			Value = strtok(NULL, " ");
			while (Value !=NULL)
			{
				AProgram.Args.push_back(string(Value));
				Value = strtok(NULL, " ");
			}
                        AProgram.NumArgs = AProgram.Args.size();
			hostenvirons.addProgram( AProgram );*/
		}

		//Figure out who I am
		int Test = getpid();
		//Get my argc / argv, neat trick huh?
		vector<string> MyArgs = get_processargs(Test);
		//Save the name so I can say who I am when I leave
		if (MyArgs.size() > 0)
			m_name = MyArgs[0];
		Program prog;
		prog.Name = m_name;
		prog.NumArgs = MyArgs.size();
		prog.Args = MyArgs;
		hostenvirons.addProgram( prog );
		m_fileparser->addHostEnv( *itr, hostenvirons );
	}
}

// create object and set description.
// be sure to validate it!
Starter::Starter(dtkManager *m):dtkAugment("Starter")
{
	char* configfile = getenv( "DADS_CONFIG_FILE" );
	m_hosts = getHosts();
	if( configfile != NULL )
	{
		m_fileparser = new fileParser( configfile );
	}
	else
	{
		printf( "STARTER - parse environment\n" );
		m_fileparser = new fileParser();
		parseEnv();
	}
	//Connect to all of the other computers we need to connect to
	vector<string>::iterator itr;
	for ( itr=m_hosts.begin();itr!=m_hosts.end();itr++)
	{
		//Make the message sender
		m_postoffice[*itr] = new dPostOffice( "COMMAND_SEGMENT" +
				*itr );
		printf("What is %s\n", string(*itr).c_str());
		printf("%s is a post office\n", string("COMMAND_SEGMENT" + *itr).c_str());
		//Make the message we will use in this dso
		m_message = new dMessage;
		char TEMP[255];
		gethostname(TEMP,255);
		m_hostname = TEMP;
		if (*itr !=  string(TEMP))
			m_postoffice[*itr]->connectTo( *itr );
	}

	//Get where I am working at
	char BUFF[200];
	getcwd(BUFF, 200);

	//Get who I am so the processes can masquerade as me
	int uid = getuid();
	map<string,hostEnv>::iterator hostitr;
	map<string,hostEnv> hostenviron = m_fileparser->getHostEnvs();
	for( hostitr=hostenviron.begin();hostitr!=hostenviron.end();hostitr++ )
	{
		addEnv( m_postoffice[hostitr->first], "DADS_SERVER",
				m_hostname );
		addEnv( m_postoffice[hostitr->first], "DISPLAY",
				hostitr->second.getDisplay() );
		addEnv( m_postoffice[hostitr->first], "DTK_DSO_FILES",
				hostitr->second.getDSO() );
		addEnv( m_postoffice[hostitr->first], "DGL_DSO_FILES",
				hostitr->second.getDSO() );
		addEnv( m_postoffice[hostitr->first], "DPF_DSO_FILES",
				hostitr->second.getDSO() );
		vector<string> envs = hostitr->second.parseEnv();
		vector<string>::iterator envitr;
		for( envitr = envs.begin();envitr != envs.end(); envitr++ )
		{
			char* envval = getenv( (*envitr).c_str() );
			if( !envval )
				envval = "";
			addEnv( m_postoffice[hostitr->first], *envitr, string( envval ) );
		}
		addProgram( m_postoffice[hostitr->first],
				hostitr->second.getPrograms(), uid, BUFF );
	}
	setDescription("starter dso for cluster\n");
	printf( "STARTER - SUCCESSFULLY STARTED CLIENTS\n" );
	validate();
}

// destructor
Starter::~Starter(void) 
{
	m_message->clear();
	m_message->setTo("DAEMON_SERVER");
	m_message->setFrom("CONSOLE");
	m_message->setSubject("STOP_PROCESS");
	m_message->addString(m_name);
	map<string,dPostOffice*>::iterator itr;
	for( itr=m_postoffice.begin();itr!=m_postoffice.end();itr++ )
	{
		m_postoffice[itr->first]->sendMessage(m_message);
		delete m_postoffice[itr->first];
	}
	delete m_message;
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
  return new Starter(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}
