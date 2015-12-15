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
#include "dPostOffice.h"
#include "dMessage.h"
#include <dtk.h>

dPostOffice::dPostOffice(const string& Name)
{
	//Initalize and setup
	m_defaultName = Name;
	m_defaultSegment = new dtkSharedMem(Name.c_str(), 0);
	if( !m_defaultSegment->isValid() )
	{
		delete m_defaultSegment;
		m_defaultSegment = new dtkSharedMem(SIZE, Name.c_str());
	}
	m_defaultSegment->flush();
	m_defaultSegment->queue();
	if (m_defaultSegment == NULL || m_defaultSegment->isInvalid() || m_defaultSegment->getSize() < SIZE)
		printf("Error, could not create default segment\n");
	m_segments.push_back(m_defaultSegment);
	m_segmentNames.push_back(Name);
	m_client = new dtkClient;
}

int dPostOffice::sendMessage(dMessage* Message)
{
	//Don't send a bad message
	if (Message == NULL)
	{
		printf("Error, NULL message attempted to be sent\n");
		return -1;
	}
	//Get the string out of it
	string TMesg = Message->getMessage();
	//Check to see if it is small enough for us
	if (TMesg.size() > SIZE )
	{
		printf("Error, message to large to be sent\n");
		return -1;
	}
	//Send the message!
	return m_defaultSegment->write(TMesg.c_str(), (TMesg.length()+1)*sizeof(char));
}

dMessage* dPostOffice::getMessage()
{
	//No message case
	if (!m_defaultSegment->qread(m_buff))
		return NULL;
	//There is a message, return it
	m_temp = string(m_buff);
	dMessage* Return = new dMessage(m_temp);
	return Return;
}

//Connect the default segment to the specified computer
int dPostOffice::connectTo(const string& ComputerName)
{
	return (m_client->connectRemoteSharedMem(m_defaultName.c_str(), ComputerName.c_str()));
}

int dPostOffice::addSegment(const string& Name)
{
	dtkSharedMem* myseg = new dtkSharedMem( Name.c_str(), 0 );
    if( !myseg )
    	myseg = new dtkSharedMem( SIZE, Name.c_str() );
	if( myseg == NULL )
		return -1;
    if( myseg->isInvalid() )
    {
        delete myseg;
		return -1;
    }
	m_segments.push_back(myseg);
	m_segmentNames.push_back(Name);
	return 0;
}

//Basically set the default segment to the  specified name
int dPostOffice::setDefaultSegment(const string& Name)
{
	for (unsigned int i=0; i<m_segmentNames.size(); i++)
	{
		if (m_segmentNames[i] == Name)
		{
			m_defaultName = Name;
			m_defaultSegment = m_segments[i];
			return 0;
		}
	}
	printf("Error segment %s not found setting default to NULL\n", Name.c_str());
	m_defaultName = "";
	m_defaultSegment = NULL;
	return -1;
}

//All of the send to other segments are implemented by temporarily switching
//the specified segment with the default segment and then sending the segment
//Send a message on a specific segment
int dPostOffice::sendMessage(dMessage* Message, const string& SegmentName)
{
	string backName = m_defaultName;
	dtkSharedMem* backSeg = m_defaultSegment;
	setDefaultSegment(SegmentName);

	if (m_defaultSegment == NULL)
	{
		m_defaultName = backName;
		m_defaultSegment = backSeg;
		return -1;
	}
	sendMessage(Message);
	m_defaultName = backName;
	m_defaultSegment = backSeg;
	return 0;
}
int dPostOffice::connectTo(const string& SegmentName, const string& ComputerName)
{
	string backName = m_defaultName;
	dtkSharedMem* backSeg = m_defaultSegment;
	setDefaultSegment(SegmentName);
	if (m_defaultSegment == NULL)
	{
		m_defaultName = backName;
		m_defaultSegment = backSeg;
		return -1;
	}
	connectTo(ComputerName);
	m_defaultName = backName;
	m_defaultSegment = backSeg;
	return 0;
}

dMessage* dPostOffice::getMessageFrom(const string& Name)
{
	string backName = m_defaultName;
	dtkSharedMem* backSeg = m_defaultSegment;
	setDefaultSegment(Name);
	if (m_defaultSegment == NULL)
	{
		m_defaultName = backName;
		m_defaultSegment = backSeg;
		return NULL;
	}
	dMessage* Temp = getMessage();
	m_defaultName = backName;
	m_defaultSegment = backSeg;
	return Temp;
}
