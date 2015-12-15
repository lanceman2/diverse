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
//A class for sending dMessages (warning, do not use tabs in your messages, spaces are ok though)
//Useful in general for Diverse applications
//Original idea and implementations by John Kelso and Mark Soltys and Andrew A. Ray
//Author Andrew A. Ray (Complete rewrite)

#ifndef dpostoffice_h
#define dpostoffice_h

class dMessage;
class dtkSharedMem;
class dtkClient;
#include <vector>
#include <string>
using namespace std;
#define SIZE  1024*sizeof(char)
class dPostOffice
{
	public:
	//Start the post office on a specific segment which is the default one
	dPostOffice(const string& segmentName);
	//Send a message on the default segment
	int sendMessage(dMessage* Message);
	//Get the message on the default segment
	dMessage* getMessage();
	//Connect the default segment to a remote computer named Name
	int connectTo(const string& ComputerName);
	//Get a message from the default segment
	//If you want to use the class with multiple shared memory segments
	//Use these functions
	int addSegment(const string& Name);
	int setDefaultSegment(const string& Name);
	int sendMessage(dMessage* Message, const string& SegmentName);
	int connectTo(const string& SegmentName, const string& ComputerName);
	dMessage*  getMessageFrom(const string& Name);
	private:
	dtkClient*  m_client;
	string m_defaultName;
	dtkSharedMem* m_defaultSegment;              //The default segment
	vector<dtkSharedMem*> m_segments;     //The shared memory segments
	vector<string>        m_segmentNames; //The segment names
	//These are mainly so that we don't have variables going in and out of scope repeatedly
	char m_buff[SIZE];                    //Temporary read buffer
	string m_temp;                        //Temporary string for use

};
#endif
