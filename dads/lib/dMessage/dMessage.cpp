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
#include "dMessage.h"
#include <sstream>
#include <string>
using namespace std;

dMessage::dMessage()
{
	m_parameters = NULL;
	m_from = "";
	m_to = "";
	m_subject = "";
	m_parameters = new stringstream;
}

dMessage::~dMessage()
{
	m_from = "";
	m_to = "";
	m_subject = "";
	delete m_parameters;
}

//Construct the message from a string
dMessage::dMessage(const string& Input)
{
	m_parameters = new stringstream;
	*(m_parameters) << Input;
	*(m_parameters) >> m_from;
	*(m_parameters) >> m_to;
	*(m_parameters) >> m_subject;
	string Line; 
	getline(*m_parameters, Line);
	delete m_parameters;
	m_parameters = new stringstream;
	*(m_parameters) << Line;
}

void dMessage::clear()
{
	m_from = "";
	m_to = "";
	m_subject = "";
	delete m_parameters;
	m_parameters = new stringstream;
}

string dMessage::getFrom()
{
	return m_from;
}

void dMessage::setFrom(const string& From)
{
	if (From !="")
		m_from = From;
}

string dMessage::getTo()
{
	return m_to;
}

void dMessage::setTo(const string& To)
{
	if (To !="")
		m_to = To;
}

string dMessage::getSubject()
{
	return m_subject;
}

void dMessage::setSubject(const string& Subject)
{
	if (Subject !="")
		m_subject = Subject;
}

int dMessage::getInt()
{
	int  Return = -1;
	*(m_parameters) >> Return;
	return Return;
}

float dMessage::getFloat()
{
	float Return = -1.0;
	*(m_parameters) >> Return;
	return Return;
}
double dMessage::getDouble()
{
	double Return = -1.0;
	*(m_parameters) >> Return;
	return Return;
}

string dMessage::getString()
{
	string Return = "";
	*(m_parameters) >> Return;
	char Hrm = m_parameters->peek();
	string Temp;
	//Get those nasty space delimited strings that windows users
	//tend to use
	while (Hrm == ' ')
	{
		*(m_parameters) >> Temp;
		Return += " " + Temp;
		Hrm = m_parameters->peek();
	}
		
	//*(m_parameters) >> Return;
	return Return;
}
char dMessage::getChar()
{
	char Return = ' ';
	*(m_parameters) >> Return;
	return Return;
}
void dMessage::addInt(const int& Value)
{
	*(m_parameters) << "\t" << Value;
}

void dMessage::addFloat(const float& Value)
{
	*(m_parameters) << "\t" << Value;
}

void dMessage::addDouble(const double& Value)
{
	*(m_parameters) << "\t" << Value;
}

void dMessage::addString(const string& Value)
{
	*(m_parameters) << "\t" << Value;
}

void dMessage::addChar(const char& Value)
{
	*(m_parameters) << "\t" << Value;
}

//Used to return contents of message in buffer form for use in the post office
string dMessage::getMessage()
{
		if (m_from == "")
			m_from = "BLANK";
		if (m_to == "")
			m_to = "BLANK";
		if (m_subject  == "")
			m_subject = "BLANK";
		return m_from + "\t" + m_to +"\t" + m_subject + m_parameters->str();
}
dMessage dMessage::operator=(dMessage& Other)
{
	string Temp = Other.getMessage();
	if (m_parameters !=NULL)
		delete m_parameters;
	m_parameters = new stringstream;
	*(m_parameters) << Temp;
	*(m_parameters) >> m_from;
	*(m_parameters) >> m_to;
	*(m_parameters) >> m_subject;
	//Now make it a virgin string stream that we can reuse
	string Line; 
	getline(*m_parameters, Line);
	delete m_parameters;
	m_parameters = new stringstream;
	*(m_parameters) << Line;
	return *this;
}
dMessage::dMessage(dMessage& Other)
{
	string Temp = Other.getMessage();
	if (m_parameters !=NULL)
		delete m_parameters;
	m_parameters = new stringstream;
	*(m_parameters) << Temp;
	*(m_parameters) >> m_from;
	*(m_parameters) >> m_to;
	*(m_parameters) >> m_subject;
	//Now make it a virgin string stream that we can reuse
	string Line; 
	getline(*m_parameters, Line);
	delete m_parameters;
	m_parameters = new stringstream;
	*(m_parameters) << Line;
}
