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
#include <unistd.h>
#include <dtk.h>
#include <unistd.h>
#include <stdlib.h>
#include <dtk/dtkDSO_loader.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <dPostOffice.h>
#include <dMessage.h>
using namespace std;

// example generic dtkAugment DSO

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
		//printf("%s\n", next_arg);
		Temp.push_back(string(next_arg));

		next_arg += strlen(next_arg)+1;
	}
	return Temp;
}
class Death : public dtkAugment
{
	public:

	Death(dtkManager* m);
	~Death(void) ;
	int preConfig(void);
	int postConfig(void);
	int preFrame(void);
	int postFrame(void);

	private:
	dPostOffice*  m_postoffice;
	dMessage*     m_message;
	string        m_name;
	dtkManager*   m_app;
};


// create object and set description.
// be sure to validate it!
Death::Death(dtkManager *m):dtkAugment("Death")
{
	m_app = m;
	//Make the message sender
        char host[256];
        gethostname( host, 255 );
        string po = "COMMAND_SEGMENT" + string( host );
	m_postoffice = new dPostOffice( po );
	//Figure out who I am so I can die when necessary
	vector<string> Temp = get_processargs(getpid());
	if (Temp.size() > 0)
		m_name = Temp[0];
	//Validation is important
	validate();
}

// destructor
Death::~Death(void) 
{
	delete m_postoffice;
}

// called only once by dpf, after pfInit() and dpfDisplay:preConfig()
// but before pfConfig()
int Death::preConfig(void) 
{
	return DTK_CONTINUE;
}

// called only once by dpf, after pfConfig() and dpfDisplay:postConfig()
// pfConfig()
int Death::postConfig(void) 
{
	return DTK_CONTINUE;
}

// called every display loop, as a pfFrame callback, before pfDraw()
// this return value causes it to only be called once
int Death::preFrame(void) 
{
	return DTK_CONTINUE;
}

// called every display loop, as a pfFrame callback, after pfDraw()
// this return value causes it to only be called once, and the object removed
int Death::postFrame(void) 
{
	m_message = m_postoffice->getMessage();
	if (m_message !=NULL)
	{
		if (m_message->getSubject() == "STOP_PROCESS" && m_name == m_message->getString())
		{
			m_app->state &= ~(DTK_ISRUNNING);
			dMessage Temp;
			Temp.setTo("DAEMON_SERVER");
			Temp.setSubject("KILL_PROCESS");
			Temp.addString(m_name);
			m_postoffice->sendMessage(&Temp);
		}
		delete m_message;
	}
	return DTK_CONTINUE;
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
  //dpf *app = static_cast<dpf *>(manager);
  return new Death(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}
