/*!
 * @class dosgShmTimeWarp
 *
 * @brief DSO which calls DOSG::setTimeWarp based on the value in the DTK
 * shared memory.
 *
 *  The time warp value is in the shared memory segment named "dgl/timeWarp"
 *  and consists of a single floating point number.
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

#ifdef DGL_ARCH_WIN32
#  define SHM_NAME "dgl\\timeWarp"
#else
#  define SHM_NAME "dgl/timeWarp"
#endif

class dosgShmTimeWarp : public dtkAugment
{
public:
  dosgShmTimeWarp(dtkManager *);
  int postFrame(void);
  
private:
  
  dtkManager *mgr ;
  dtkSharedMem *shm ;
  float warp ;
  
};


dosgShmTimeWarp::dosgShmTimeWarp(dtkManager *m) :
  dtkAugment("dosgShmTimeWarp")
{
  mgr = m ;
  shm = NULL ;

  // see if shared memory already exists
  dtkSharedMem *old_shm = new dtkSharedMem(SHM_NAME, 0) ;

  // it does, but is of wrong size
  if (old_shm && old_shm->isValid() && (old_shm->getSize()!=sizeof(float))) {
    dtkMsg.add(DTKMSG_ERROR,"dosgShmTimeWarp: shared "
	       "memory \"%s\" already exists, but wrong size.\n", SHM_NAME) ;
    return ;
  }

  // it doesn't, so create a new one
  if ((!old_shm) || 
      (old_shm && old_shm->isInvalid())) { // have to create a new segment
    shm = new dtkSharedMem(sizeof(float), SHM_NAME) ;
    if(!shm || shm->isInvalid()) {
      dtkMsg.add(DTKMSG_ERROR,"dosgShmTimeWarp: unable to get valid shared "
		 "memory \"%s\".\n", SHM_NAME) ;
      return ;
    }
  } else { // reuse old segment
    shm = old_shm ;
  }
  
  if (shm->queue())  {
    dtkMsg.add(DTKMSG_ERROR,"dosgShmTimeWarp: unable to set queued read\n") ;
    return ;
  }

  shm->flush() ;
  
  setDescription("Controls the DOSG reference time via DTK shared memory") ;
  
  // dtkAugment::dtkAugment() will not validate the object
  validate() ;
}

int dosgShmTimeWarp::postFrame(void) 
{
  int ret = shm->qRead(&warp) ;
  if (ret<0) 
    {
      dtkMsg.add(DTKMSG_ERROR,"dosgShmTimeWarp: unable to read shared "
		 "memory \"%s\".\n", SHM_NAME) ;
      return ERROR_ ;
    }
  else if (ret>0)
    {
      dtkMsg.add(DTKMSG_INFO,"dosgShmTimeWarp: setting time warp to %f\n",warp) ;
      DOSG::setTimeWarp(warp) ;
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
  return new dosgShmTimeWarp(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}


