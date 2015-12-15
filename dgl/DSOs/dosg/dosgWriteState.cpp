#include <osg/Vec3>

#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <dosg.h>


/*!
 * @class  dosgWriteState
 *
 * @brief DSO which writes dosg state data to shared memory every frame, if
 * changed
 *
 * The shared memory files are in the "dosg/state" subdirectory of $DTK_SHAREDMEM_DIR 
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class dosgWriteState : public dtkAugment
{
public:
    dosgWriteState(dtkManager *);
    virtual ~dosgWriteState(void) ;
    virtual int postFrame(void) ;

private:

    dtkSharedMem *new_shm(unsigned int size, char *name, void *init) ;

    dtkSharedMem *center_shm ;
    osg::Vec3 center, old_center ;
    float center_array[3] ;

    dtkSharedMem *radius_shm ;
    float radius, old_radius ;

};


dosgWriteState::dosgWriteState(dtkManager *m) :
    dtkAugment("dosgWriteState")
{
    setDescription("writes dosg state data to shared memory every frame, if changed\n") ;

    validate() ;
}

dosgWriteState::~dosgWriteState(void) 
{
    printf("%s %d\n",__FILE__,__LINE__) ;
}  

int dosgWriteState::postFrame(void)
{

    static bool first = true ;
    if (first)
    {
	first = false ;

	////////////////
	old_center = center = DOSG::getScene()->getBound().center() ;
	center_array[0] = center.x();  
	center_array[1] = center.y();
	center_array[2] = center.z();

#ifdef DGL_ARCH_WIN32
	center_shm =  new_shm(sizeof(float)*3, "dgl\\state\\center", center_array) ; 
#else
	center_shm =  new_shm(sizeof(float)*3, "dgl/state/center", center_array) ; 
#endif

	old_radius = radius =  DOSG::getScene()->getBound().radius() ;
#ifdef DGL_ARCH_WIN32
	radius_shm =  new_shm(sizeof(float), "dgl\\state\\radius", &radius) ;
#else
	radius_shm =  new_shm(sizeof(float), "dgl/state/radius", &radius) ;
#endif

    }

    // do this every frame

    ////////////////
    if (center_shm)
    {
	center = DOSG::getScene()->getBound().center() ;
	if (old_center != center)
	{
	    old_center = center ;
	    center_array[0] = center.x();  
	    center_array[1] = center.y();
	    center_array[2] = center.z();
	    center_shm->write(center_array) ;
	    //printf("%s %d: writing center\n",__FILE__,__LINE__) ;
	} 
    }
  
    ////////////////
    if (radius_shm)
    {
	radius = DOSG::getScene()->getBound().radius() ;
	if (old_radius != radius)
	{
	    old_radius = radius ;
	    radius_shm->write(&radius) ;
	    //printf("%s %d: writing radius\n",__FILE__,__LINE__) ;
	} 
    }
  
    return CONTINUE;
}

dtkSharedMem *dosgWriteState::new_shm(unsigned int size, char *name, void *init)
{
    dtkSharedMem *shm = NULL ;
    shm = new dtkSharedMem(size, name) ;
    if (!shm || shm->isInvalid()) 
    {
	dtkMsg.add(DTKMSG_ERROR,
		   "dosgWriteState:: new_shm failed:"
		   " couldn't open shared memory segment\"%s\"\n",name) ;
	shm = NULL ;
    }
    shm->write(init) ;

    return shm ;
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
    return new dosgWriteState(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}


