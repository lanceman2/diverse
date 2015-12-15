/*!
 * @class dosgCullSceneForNFrames
 *
 * @brief DSO which culls all geometry under the scene node for N frames.  N
 * is given in the envvar DOSG_CULL_SCENE_FOR_N_FRAMES
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

// a cull callback which throws away all the geometry
class nullCullCallback : public osg::NodeCallback
{
public:
    nullCullCallback() {}
    
    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor )
    {
	return;
    }
protected:
    ~nullCullCallback(){}
};

class dosgCullSceneForNFrames : public dtkAugment
{
public:
    dosgCullSceneForNFrames(dtkManager *);
    int postConfig(void);
    int postFrame(void);
  
private:
    int n ;
    nullCullCallback *ncc ;
};


dosgCullSceneForNFrames::dosgCullSceneForNFrames(dtkManager *m) :
    dtkAugment("dosgCullSceneForNFrames")
{

    if (getenv("DOSG_CULL_SCENE_FOR_N_FRAMES"))
    {
	if (DGLUtil::string_to_int(getenv("DOSG_CULL_SCENE_FOR_N_FRAMES"), &n))
	{
	    dtkMsg.add(DTKMSG_ERROR,"dosgCullSceneForNFrames: envvar \"DOSG_CULL_SCENE_FOR_N_FRAMES\" is not set to a valid integer: %s\n", getenv("DOSG_CULL_SCENE_FOR_N_FRAMES")) ;
	    return ;
	}
    }
    else
    {
	dtkMsg.add(DTKMSG_ERROR,"dosgCullSceneForNFrames: envvar \"DOSG_CULL_SCENE_FOR_N_FRAMES\" is not set\n") ;
	return ;
    }

    dtkMsg.add(DTKMSG_WARN,"dosgCullSceneForNFrames: the scene node will be culled until the frame counter reaches %d\n",n) ;    

    setDescription("Culls all geometry under the scene node for N frames.  N is given in the envvar DOSG_CULL_SCENE_FOR_N_FRAMES") ;
  
    // dtkAugment::dtkAugment() will not validate the object
    validate() ;
}

int dosgCullSceneForNFrames::postConfig(void) 
{
    ncc = new nullCullCallback ;
    DOSG::getScene()->setCullCallback(ncc) ;

    return CONTINUE ;
}

int dosgCullSceneForNFrames::postFrame(void) 
{

    if (dtkMsg.isSeverity(DTKMSG_INFO) && (DOSG::getFrameNumber()%10 == 0))
    {
	printf("dosgCullSceneForNFrames: frame number = %d\r",DOSG::getFrameNumber()) ;
	fflush(stdout) ;
    }
    if (n<=DOSG::getFrameNumber())
    {
	dtkMsg.add(DTKMSG_WARN,"dosgCullSceneForNFrames: no longer culling the scene node\n") ;
	DOSG::getScene()->setCullCallback(NULL) ;
	return REMOVE_OBJECT ;

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
    return new dosgCullSceneForNFrames(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}


