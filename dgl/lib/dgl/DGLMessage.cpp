#include <sstream>
#include <string>
#ifdef DGL_ARCH_WIN32
#  include <windows.h>
#endif
#include <dgl.h>
using namespace std;

DGLMessage::DGLMessage()
{
	m_parameters = NULL;
	m_from = "";
	m_to = "";
	m_subject = "";
	m_parameters = new stringstream;
}

DGLMessage::~DGLMessage()
{
	m_from = "";
	m_to = "";
	m_subject = "";
	delete m_parameters;
}

//Construct the message from a string
DGLMessage::DGLMessage(const string& Input)
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

void DGLMessage::clear()
{
	m_from = "";
	m_to = "";
	m_subject = "";
	delete m_parameters;
	m_parameters = new stringstream;
}

string DGLMessage::getFrom()
{
	return m_from;
}

void DGLMessage::setFrom(const string& From)
{
	if (From !="")
		m_from = From;
}

string DGLMessage::getTo()
{
	return m_to;
}

void DGLMessage::setTo(const string& To)
{
	if (To !="")
		m_to = To;
}

string DGLMessage::getSubject()
{
	return m_subject;
}

void DGLMessage::setSubject(const string& Subject)
{
	if (Subject !="")
		m_subject = Subject;
}

int DGLMessage::getInt()
{
	int  Return = -1;
	*(m_parameters) >> Return;
	return Return;
}

float DGLMessage::getFloat()
{
	float Return = -1.0;
	*(m_parameters) >> Return;
	return Return;
}
double DGLMessage::getDouble()
{
	double Return = -1.0;
	*(m_parameters) >> Return;
	return Return;
}

string DGLMessage::getString()
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
char DGLMessage::getChar()
{
	char Return = ' ';
	*(m_parameters) >> Return;
	return Return;
}
void DGLMessage::addInt(const int& Value)
{
	*(m_parameters) << "\t" << Value;
}

void DGLMessage::addFloat(const float& Value)
{
	*(m_parameters) << "\t" << Value;
}

void DGLMessage::addDouble(const double& Value)
{
	*(m_parameters) << "\t" << Value;
}

void DGLMessage::addString(const string& Value)
{
	*(m_parameters) << "\t" << Value;
}

void DGLMessage::addChar(const char& Value)
{
	*(m_parameters) << "\t" << Value;
}

//Used to return contents of message in buffer form for use in the post office
string DGLMessage::getMessage()
{
		if (m_from == "")
			m_from = "BLANK";
		if (m_to == "")
			m_to = "BLANK";
		if (m_subject  == "")
			m_subject = "BLANK";
		return m_from + "\t" + m_to +"\t" + m_subject + m_parameters->str();
}
DGLMessage DGLMessage::operator=(DGLMessage& Other)
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
DGLMessage::DGLMessage(DGLMessage& Other)
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
