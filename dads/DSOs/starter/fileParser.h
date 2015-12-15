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
#ifndef _fileparse_h_
#define _fileparse_h_

#include <string>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

//Simple container that will hold all of the 
//information about a program
struct Program
{
	string Name;
	int    NumArgs;
	vector<string> Args;
};

//Simple container to hold DSO and program info
//for each host
class hostEnv
{
	public:
	hostEnv();
	//What computer is this, wrapper on gethostname()
	string getHostName();
	void setHostName( const string& hostname );
	//Returns a colon separated list of dsos meant to be run on this machine
	//when executed by the Dads listener
	string getDSO();
	void addDSO( const string& dso );
	string getEnv();
	void addEnv( const string& env );
	vector<string> parseEnv();
	//Get a specific program
	Program getProgram( const string& Name );
	//Get all of the programs that this specific machine has
	vector<Program> getPrograms();
	void addProgram( Program program );
	string getDisplay();
	void setDisplay( const string& display );
	private:
	string m_hostname;
	vector<Program> m_programs;
	string m_dsos;
	string m_envs;
	string m_display;
};

//The file parser class that does all of the work
class fileParser
{
	public:
	//The constructor
	fileParser() {};
	fileParser( const string& FILENAME );
	map<string,hostEnv> getHostEnvs();
	hostEnv getHostEnv( const string& hostname );
	void addHostEnv( const string& hostname, hostEnv hostenviron );
	private:
	map<string,hostEnv> m_hostEnvs;
//	string m_hostname;
//	vector<Program> m_programs;
//	string m_dso;
	ifstream m_inputFile;
	void parse();
};
#endif
