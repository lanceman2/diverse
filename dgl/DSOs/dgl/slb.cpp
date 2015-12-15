#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <string>

#include <dtk.h>
#include <dtk/dtkDSO_loader.h>

#include <dgl.h>

/*!
 * @class slb
 *
 * @brief DSO which sets dtk shared memory files based on button presses
 *
 * Reroute button data into separate shared memory segments, one or more
 * segment per button.  A button device can have up to 32 buttons, one per
 * bit of a u_int32_t (unsigned 32 bit int), number 0-31.
 *
 * Each shared memory file is one byte
 *
 * One env var is set per button.  if the env var isn't set for a particular
 * button, data isn't rerouted for that button.

 * The env var for button N is SLB_N, without leading zeros, so SLB_0 to
 * SLB_31 are evaluated.

 * If you specify more env vars than buttons on the device we unload.

 * Example:
 *
 *  one shared memory segment for button 0
 *  -  export SLB_0="manny"
 *
 *  two shared memory segments for button 1
 *  -  export SLB_1="moe jack"
 *
 * The mapping can be changed while running by passing commands through the
 * shared memory segment whose name is in the envvar SLB_COMMANDS
 *
 * The commands are of the form: n shm [ shm ... ] where n is the button
 * number, and shm are one or more names of shared memory segments.  a
 * shared memory name of NULL disables the button.
 *
 * By default slb reads button data from the dtkInButton "buttons".  The
 * envvar SLB_SHM can be set to the name of a dtk shared memory file of
 * size(unsigned char) and the shared memory will be read instead of the
 * dtkInButton
 *
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class slb : public dtkAugment {
public:
    slb(dtkManager *) ;
  
    int preFrame(void) ;
    int postFrame(void) ;
  
private:
    dtkManager *mgr ;
    dtkRecord *record ;
    dtkInButton *button ;

    dtkSharedMem *buttonShm ;
    
    int buttons ;
    dtkDequeuer *dequeuer ;
    char *envvar[32] ;
    std::vector<std::string> args[32] ;
    std::vector<dtkSharedMem *> shm[32] ;
    // button state the previous frame
    char last_b[32] ;
    dtkSharedMem *commandsShm ;
    char commands[256] ;
    char zero ;
} ;

slb::slb(dtkManager *m) :
    dtkAugment("slb") 
{
    
    mgr = m ;
    button = NULL ;
    buttonShm = NULL ;
    dequeuer = NULL ;

    zero = 0 ;

    char slbenvvar[32][10] ;
    for (int i = 0; i<32; i++) 
    {
	last_b[i] = 0 ;
	strcpy(slbenvvar[i], "SLB_") ;
	int n = strlen(slbenvvar[i]) ;
	if (i<10) 
	{
	    slbenvvar[i][n] = i+'0' ;
	    slbenvvar[i][n+1] = '\0' ;
	} 
	else 
	{
	    slbenvvar[i][n] = (i/10)+'0' ;
	    slbenvvar[i][n+1] = (i%10)+'0' ;
	    slbenvvar[i][n+2] = '\0' ;
	}
	envvar[i] = getenv(slbenvvar[i]) ;
	if (envvar[i]) 
	{
	    args[i] = DGLUtil::parseLine(envvar[i]) ;
	    //printf("%s %d:  args[i].size() = %d\n",__FILE__,__LINE__,args[i].size()) ;
	    //for (unsigned int n=0; n<args[i].size(); n++)
	    //printf("%s %d:  args[%d][%d]: \"%s\"\n",__FILE__,__LINE__,i,n,args[i][n].c_str()) ;
	}
    }
  
    setDescription("reroutes button data to seperate shared memory segments, one or more segments per button") ;
  
    // dtkAugment::dtkAugment() will not validate the object
    validate() ;
}

int slb::preFrame(void) 
{

    if (getenv("SLB_SHM")) // read buttons via shared memory
    {
	buttonShm = new dtkSharedMem(1 ,getenv("SLB_SHM")) ;
	if (buttonShm->isInvalid()) return ERROR_ ;
	buttonShm->queue() ;
	buttonShm->flush() ;
	buttons = 8 ;
    }
    else // read buttons via dtkInButton, the default
    {
	dequeuer = new dtkDequeuer(mgr->record()) ;
	if (!dequeuer || dequeuer->isInvalid()) return ERROR_ ;
	
	button = (dtkInButton *) mgr->get("buttons", DTKINBUTTON_TYPE) ;
	if (!button) return ERROR_ ;
	
	button->queue() ;
	buttons = button->number() ;
    }

    bool bomb = false ;
    for (int i = buttons; i<32; i++) 
    {
	if (envvar[i]) 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "slb::preFrame() failed:"
		       " device has %d buttons, but action is specified for"
		       " button %d\n", buttons, i) ;
	    bomb = true ;
	}
    }   

    if (bomb) return ERROR_ ;


    for (int i = 0; i < buttons; i++) 
    {
	if (envvar[i]) 
	{
	    for (unsigned int j=0; j<args[i].size(); j++) 
	    {
		dtkMsg.add(DTKMSG_INFO,
			   "slb::preFrame(): "
			   "button %d writes to DTK shared memory \"%s\"\n",
			   i,args[i][j].c_str()) ;
		shm[i].push_back(new dtkSharedMem(1, args[i][j].c_str(), &zero)) ;
		if (!shm[i].back() || shm[i].back()->isInvalid()) 
		{
		    dtkMsg.add(DTKMSG_ERROR,
			       "slb::preFrame() failed:"
			       " couldn't open shared memory segment\"%s\"\n",
			       args[i][j].c_str()) ; 
		    return ERROR_ ;
		}
	    }
	}
    }

    if (getenv("SLB_COMMANDS"))
    {
	commandsShm = new dtkSharedMem(getenv("SLB_COMMANDS"), 0) ;
	if (commandsShm->isInvalid())
	{
	    commandsShm = new dtkSharedMem(256, getenv("SLB_COMMANDS")) ;
	    if (commandsShm->isInvalid())
	    {
		dtkMsg.add(DTKMSG_ERROR,
			   "slb::preFrame() failed:"
			   " couldn't open shared memory segment\"%s\"\n",
			   getenv("SLB_COMMANDS")) ; 
		return ERROR_ ;
	    }
	}
	commandsShm->queue() ;
	commandsShm->flush() ;
    }
    else
    {
	commandsShm = NULL ;
    }

    return REMOVE_CALLBACK ;

}


int slb::postFrame(void) 
{
  

    while (commandsShm && commandsShm->qread(commands)>0)
    {
	std::vector<std::string> a = DGLUtil::parseLine(commands) ;	
	if (a.size() <2)
	{
	    dtkMsg.add(DTKMSG_WARN,"slb: ignoring invalid command \"%s\"\n",commands) ;
	}
	else
	{
	    int n = -1 ;
	    if ( (sscanf(a[0].c_str(), "%d", &n) != 1) || n<0 || n>=buttons)
	    {
		dtkMsg.add(DTKMSG_WARN,"slb: ignoring invalid command \"%s\"\n",commands) ;
	    }
	    else
	    {
		envvar[n] = strdup(commands) ;
		a.erase(a.begin()) ;
		args[n] = a ;
		shm[n].clear() ;
		for (unsigned int j=0; j<a.size(); j++) 
		{
		    if (!strcasecmp(a[j].c_str(),"null"))
		    {
			dtkMsg.add(DTKMSG_INFO,
				   "slb::setButton(): "
				   "shm==NULL, button %d now disabled\n", n) ;
			envvar[n] = "" ;
			args[n].clear() ;
			shm[n].clear() ;
		    }
		    else
		    {
			dtkMsg.add(DTKMSG_INFO,
				   "slb::setButton(): "
				   "button %d now writes to DTK shared memory \"%s\"\n",
				   n,a[j].c_str()) ;
			shm[n].push_back(new dtkSharedMem(1, a[j].c_str(), &zero)) ;
			if (!shm[n].back() || shm[n].back()->isInvalid()) 
			{
			    dtkMsg.add(DTKMSG_ERROR,
				       "slb::setButton() failed:"
				       " couldn't open shared memory segment\"%s\", disabling button\n",
				       a[j].c_str()) ; 
			    envvar[n] = "" ;
			    args[n].clear() ;
			    shm[n].clear() ;
			}
		    }
		}
		
	    }
	    
	}
    }


    // we don't get buttons until frame == 2
    static unsigned int frame=0 ;
    if (frame<3)
    {
	// manually flush button events
	while (dequeuer && dequeuer->getNextEvent(button)) {} ;
	if (buttonShm) buttonShm->flush() ;

    }
    frame++ ;
  
    if (dequeuer)
    {
	dtkRecord_event *e ;
	// if we have a button event
	while ((e = dequeuer->getNextEvent(button))) 
	{
	    // loop through all buttons we're watching 
	    for (int i = 0; i < buttons; i++) 
	    {
		// if we're watching this button
		if (shm[i].size()>0) 
		{
		    // get the status for this button
		    char b = (char) button->read(i,e) ;
		    if (b!=last_b[i]) 
		    {
			last_b[i] = b ;
			dtkMsg.add(DTKMSG_INFO,"slb: writing button %d, value = %d to ",i,b) ;
			// and write it to the slb shared memory
			for (unsigned int j=0; j<args[i].size(); j++) 
			{
			    dtkMsg.append("\"%s\" ",args[i][j].c_str()) ;
			    if (shm[i][j]->write(&b)) 
			    {
				dtkMsg.append(
					      "slb::postFrame() failed:"
					      " unable to write to shared memory \"%s\"\n",
					      args[i][j].c_str()) ;
				return ERROR_ ;
			    }
			}
			dtkMsg.append("\n") ;
		    }
		}
	    }
	}  
    }
    else if (buttonShm)
    {
	char bs ;
	while (buttonShm->qread(&bs)>0)
	{
	    //printf("bs = %d\n",bs) ;
	    // loop through all buttons we're watching 
	    for (int i = 0; i < buttons; i++) 
	    {
		// if we're watching this button
		if (shm[i].size()>0) 
		{
		    // get the status for this button
		    char b = bs>>i & 1 ;
		    //printf("bs = %d, b = %d\n",bs, i, bs>>i, b) ;
		    if (b!=last_b[i]) 
		    {
			last_b[i] = b ;
			dtkMsg.add(DTKMSG_INFO,"slb: writing button %d, value = %d to ",i,b) ;
			// and write it to the slb shared memory
			for (unsigned int j=0; j<args[i].size(); j++) 
			{
			    dtkMsg.append("\"%s\" ",args[i][j].c_str()) ;
			    if (shm[i][j]->write(&b)) 
			    {
				dtkMsg.append(
					      "slb::postFrame() failed:"
					      " unable to write to shared memory \"%s\"\n",
					      args[i][j].c_str()) ;
				return ERROR_ ;
			    }
			}
			dtkMsg.append("\n") ;
		    }
		}
	    }
	}
    }
  
    return CONTINUE ;
}

/************ DTK C++ dtkAugment loader/unloader functions ***************
 *
 * All DTK dtkAugment DSO files are required to declare these two
 * functions.  These function are called by the loading program to get
 * your C++ objects loaded.
 *
 *************************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p) {
    return new slb(manager) ;
}

static int dtkDSO_unloader(dtkAugment *augment) {
    delete augment ;
    return DTKDSO_UNLOAD_CONTINUE ;
}

