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
#include <string.h>
#include "fileParser.h"
#include <unistd.h>

fileParser::fileParser( const string& filename )
{
	m_inputFile.open( filename.c_str() );
	if ( !m_inputFile )
	{
		if ( filename !="")
			printf("ERROR:  could not open file %s as the DADS configuration file\n", filename.c_str());
		else
			printf("ERROR:  blank input file requested as the DADS configuraiton file\n");
	}
//	char Temp[255];
//	if (gethostname(Temp,255) !=0)
//		printf("ERROR:  could not get host name for this DADS machine\n");
//	m_hostname = string(Temp);
//	m_dso ="";
	parse();
}

hostEnv::hostEnv()
:m_display( ":0.0" )
{
}

string hostEnv::getHostName()
{
	return m_hostname;
}

void hostEnv::setHostName( const string& hostname )
{
	m_hostname = hostname;
}

string hostEnv::getDSO()
{
	return m_dsos;
}

void hostEnv::addDSO( const string& dso )
{
	if ( m_dsos.size() )
		m_dsos += ":";
	m_dsos += dso;
}

string hostEnv::getEnv()
{
	return m_envs;
}

void hostEnv::addEnv( const string& env )
{
	if( m_envs.size() )
		m_envs += ":";
	m_envs += env;
}

vector<string> hostEnv::parseEnv()
{
	vector<string> envs;
	char* envstr = strdup( m_envs.c_str() );
	char* curenv = strtok( envstr, ":" );
	while( curenv != NULL )
	{
		envs.push_back( string( curenv ) );
		curenv = strtok( NULL, ":" );
	}
	return envs;
}

vector<Program> hostEnv::getPrograms()
{
	return m_programs;
}

Program hostEnv::getProgram(const string& Name)
{
	for (unsigned int i=0; i< m_programs.size(); i++)
	{
		if (m_programs[i].Name == Name)
			return m_programs[i];
	}
	Program Temp;
	Temp.Name="BAD_PROGRAM";
	return Temp;
}

void hostEnv::addProgram( Program program )
{
	m_programs.push_back( program );
}

string hostEnv::getDisplay()
{
	return m_display;
}

void hostEnv::setDisplay( const string& display )
{
	m_display = display;
}

map<string,hostEnv> fileParser::getHostEnvs()
{
	return m_hostEnvs;
}

hostEnv fileParser::getHostEnv( const string& hostname )
{
	map<string,hostEnv>::iterator itr = m_hostEnvs.find( hostname );
	if( itr != m_hostEnvs.end() )
		return itr->second;
	hostEnv Temp;
	Temp.setHostName( "BAD_HOSTNAME" );
	return Temp;
}

void fileParser::addHostEnv( const string& hostname, hostEnv hostenviron )
{
	m_hostEnvs[hostname] = hostenviron;
}

void fileParser::parse()
{
	string Line;
	getline(m_inputFile, Line);
	while (!m_inputFile.eof())
	{
		//printf("%s\n", Line.c_str());
		if (Line.size() == 0)
		{
			getline(m_inputFile,Line);
		}
		else if (Line[0] == ';')
		{

			getline(m_inputFile,Line);
		}
		else 
		{
			char* Temp =strdup(Line.c_str());
			char* MachineName = strtok(Temp,"\t");
			hostEnv hostenviron;
			map<string,hostEnv>::iterator itr =
			       		m_hostEnvs.find( MachineName );
			if( itr != m_hostEnvs.end() )
				hostenviron = itr->second;
			else
				hostenviron.setHostName( MachineName );
			//Is this line for our machine
//			if (!strcmp(MachineName, m_hostname.c_str()))
//			{
				char* Command = strtok(NULL,"\t");
				//We have a DSO command
				if (!strcmp(Command, "DSO"))
				{
					char* Value = strtok(NULL, "\n");
					if (Value !=NULL)
						hostenviron.addDSO( Value );
				}
				else if (!strcmp(Command, "PROGRAM"))
				{
					Program AProgram;
				 	char* Name = strtok(NULL," ");
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
					hostenviron.addProgram( AProgram );
				}
				else
				{
					printf("Other command %s\n", Command);
				}
				getline(m_inputFile,Line);
//			}
			//Or is it for another machine
//			else
//			{
//				getline(m_inputFile,Line);
//			}
		}
	}
}

