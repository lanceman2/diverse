/*!
 * @class dosgExitAfterNFrames
 *
 * @brief DSO which exits after the frame counter reaches a number specified
 * by the envvar DOSG_EXIT_AFTER_N_FRAMES
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */

#include <stdio.h>
#include <stdlib.h>

#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <dosg.h>

class dosgExitAfterNFrames : public dtkAugment
{
public:
    dosgExitAfterNFrames(dtkManager *);
    int postFrame(void);
    
private:
    int n ;
    int s ;
    dtkTime time ;
};


dosgExitAfterNFrames::dosgExitAfterNFrames(dtkManager *m) :
    dtkAugment("dosgExitAfterNFrames")
{

    setDescription("Exits after the frame counter reaches a number specified by the envvar DOSG_EXIT_AFTER_N_FRAMES.  DOSG_EXIT_AFTER_N_FRAMES_TIMERDELAY can be used to set a delay on when a frame timer will start") ;

    if (getenv("DOSG_EXIT_AFTER_N_FRAMES"))
    {
	if (DGLUtil::string_to_int(getenv("DOSG_EXIT_AFTER_N_FRAMES"), &n))
	{
	    dtkMsg.add(DTKMSG_ERROR,"dosgExitAfterNFrames: envvar \"DOSG_EXIT_AFTER_N_FRAMES\" is not set to a valid integer: %s\n", getenv("DOSG_EXIT_AFTER_N_FRAMES")) ;
	    return ;
	}
    }
    else
    {
	dtkMsg.add(DTKMSG_ERROR,"dosgExitAfterNFrames: envvar \"DOSG_EXIT_AFTER_N_FRAMES\" is not set\n") ;
	return ;
    }

    if (getenv("DOSG_EXIT_AFTER_N_FRAMES_TIMERDELAY"))
    {
	if (DGLUtil::string_to_int(getenv("DOSG_EXIT_AFTER_N_FRAMES_TIMERDELAY"), &s))
	{
	    dtkMsg.add(DTKMSG_ERROR,"dosgExitAfterNFrames: envvar \"DOSG_EXIT_AFTER_N_FRAMES_TIMERDELAY\" is not set to a valid integer: %s\n", getenv("DOSG_EXIT_AFTER_N_FRAMES_TIMERDELAY")) ;
	    return ;
	}
    }
    else
    {
	s = 3 ;
    }

    dtkMsg.add(DTKMSG_WARN,"dosgExitAfterNFrames: the application will exit when the frame counter reaches %d\n",n) ;    
    dtkMsg.add(DTKMSG_WARN,"dosgExitAfterNFrames: the timer will start when the frame counter reaches %d\n",s) ;    
    // dtkAugment::dtkAugment() will not validate the object
    validate() ;
}

int dosgExitAfterNFrames::postFrame(void) 
{
    static bool first = true ;
    if (DOSG::getFrameNumber() == s )
    {
	dtkMsg.add(DTKMSG_WARN,"dosgExitAfterNFrames: starting to time frames at frame number %d\n", s) ;
	time.reset(0.0,1);
	first = false ;
    }
    else if (n<=DOSG::getFrameNumber())
    {
	dtkMsg.add(DTKMSG_WARN,"dosgExitAfterNFrames: exiting after %d frames\n",n) ;
	dtkMsg.add(DTKMSG_WARN,"dosgExitAfterNFrames: time for %d frames = %f seconds, or %f frames/second\n",
		   n-s, (float)time.get(), ((float)(n-s))/((float)time.get())) ;
	DGL::getApp()->state  &= ~(DTK_ISRUNNING);
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

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
    return new dosgExitAfterNFrames(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}


