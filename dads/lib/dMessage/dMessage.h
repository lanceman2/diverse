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
#ifndef dmessage_h
#define dmessage_h

#include <string>
#include <sstream>
using namespace std;

//A message class for Diverse
//Basically allows you to set and get all of the normal variables
//Author Andrew A. Ray
class dMessage
{
	public:
	dMessage();
	~dMessage();
	dMessage(const string& Input);
	dMessage(dMessage& Input);
	dMessage operator=(dMessage& Other);
	//Message header functions
	//The from field
	string getFrom();
	void   setFrom(const string& From);

	//The to field
	string getTo();
	void   setTo(const string& To);

	//The subject field
	string getSubject();
	void   setSubject(const string& Subject);

	//Parameter functions
	//Integer support
	void addInt(const int& Value);
	int  getInt();

	//Float support
	void addFloat(const float& Value);
	float getFloat();

	//Double support
	void addDouble(const double& Value);
	double getDouble();

	//String support
	void addString(const string& Value);
	string getString();

	//Character support
	void addChar(const char& character);
	char getChar();

	//Get the whole message
	string getMessage();
	//Clear the message
	void clear();
	private:
	string m_from;
	string m_to;
	string m_subject;
	stringstream* m_parameters;
};
#endif
