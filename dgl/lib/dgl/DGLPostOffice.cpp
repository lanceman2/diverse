#include <dgl.h>
#define SIZE 1024

DGLPostOffice::DGLPostOffice(const string& Name)
{
	m_size = SIZE;
	m_buff = new char[m_size];
	//Initalize and setup
	m_defaultName = Name;
	m_defaultSegment = new dtkSharedMem(Name.c_str(), 0);
	if( !m_defaultSegment->isValid() )
	{
		delete m_defaultSegment;
		m_defaultSegment = new dtkSharedMem(SIZE, Name.c_str());
	}
	m_defaultSegment->flush();
	// default queue length is ridiculously short
	m_defaultSegment->queue(200);
	if (m_defaultSegment == NULL || m_defaultSegment->isInvalid() || m_defaultSegment->getSize() < SIZE)
		printf("Error, could not create default segment\n");
	m_segments.push_back(m_defaultSegment);
	m_segmentNames.push_back(Name);
	m_client = NULL;
}

int DGLPostOffice::sendMessage(DGLMessage* Message)
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
	int Return = m_defaultSegment->write(TMesg.c_str(), (TMesg.length()+1)*sizeof(char));
	usleep(1000/30);
	return Return;
}

DGLMessage* DGLPostOffice::getMessage()
{
	//No message case
	if (!m_defaultSegment->qread(m_buff))
		return NULL;
	//There is a message, return it
	m_temp = string(m_buff);
	DGLMessage* Return = new DGLMessage(m_temp);
	return Return;
}

//Connect the default segment to the specified computer
int DGLPostOffice::connectTo(const string& ComputerName)
{
	if (m_client == NULL)
	{
		m_client = new dtkClient;
	}
	return (m_client->connectRemoteSharedMem(m_defaultName.c_str(), ComputerName.c_str()));
}

int DGLPostOffice::addSegment(const string& Name)
{
	dtkSharedMem* myseg = new dtkSharedMem(SIZE, Name.c_str());
	m_segments.push_back(myseg);
	m_segmentNames.push_back(Name);
	if (myseg == NULL || myseg->isInvalid())
		return -1;
	else 
		return 0;
}

//Basically set the default segment to the  specified name
int DGLPostOffice::setDefaultSegment(const string& Name)
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
int DGLPostOffice::sendMessage(DGLMessage* Message, const string& SegmentName)
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
int DGLPostOffice::connectTo(const string& SegmentName, const string& ComputerName)
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

DGLMessage* DGLPostOffice::getMessageFrom(const string& Name)
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
	DGLMessage* Temp = getMessage();
	m_defaultName = backName;
	m_defaultSegment = backSeg;
	return Temp;
}

DGLPostOffice::~DGLPostOffice()
{
	if (m_buff != NULL)
		delete m_buff;
}
