#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <dosg.h>

//#define AXIS

/*!
 * @class dosgWorldPivotNavMod
 *
 * @brief DSO which sets the pivot point of the current navigation to a
 * world coordinate specified in dtk shared memory.  If the shared memory
 * file doesn't exist, the origin is used
 *
 * the dtk shared memory file is "dgl/worlPivot", and contains 3 floats
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */


class dosgWorldPivotNavMod : public dtkAugment
{
public:
  dosgWorldPivotNavMod(dtkManager *m) ;
  virtual int postFrame(void) ;
  
private:
  dtkManager *mgr ;
  dtkNavList *navList;
  bool first ;
  osg::Transform *scene ;
  dtkMatrix worldMat ;
  dtkSharedMem *worldPivotShm ;
  float worldPivot[6] ;
  dtkCoord worldPivotCoord ;
  dtkCoord dglPivotCoord ;
  
#ifdef AXIS
  osg::PositionAttitudeTransform *axis ;
#endif
} ;

dosgWorldPivotNavMod::dosgWorldPivotNavMod(dtkManager *m):
  dtkAugment("dosgWorldPivotNavMod")
{
  
  mgr = m ;
  setDescription("dosg world coordinate pivot navigation modifier") ;
  navList = NULL;

  // see if shared memory already exists
#ifdef DGL_ARCH_WIN32
  worldPivotShm = new dtkSharedMem("dgl\\worldPivot",0) ;
#else
  worldPivotShm = new dtkSharedMem("dgl/worldPivot",0) ;
#endif
  // it does, but is of wrong size
  if (worldPivotShm && worldPivotShm->isValid() && (worldPivotShm->getSize()!=sizeof(float)*3)) 
    {
      dtkMsg.add(DTKMSG_ERROR,"dosgWorldPivotNavMod: shared "
		 "memory \"%s\" exists, but wrong size.\n", "dgl/worldPivot") ;
      return ;
    }
  else if (!worldPivotShm || worldPivotShm->isInvalid())
    {
      dtkMsg.add(DTKMSG_ERROR,"dosgWorldPivotNavMod: shared "
		 "memory \"%s\" doesn't exist, using origin\n", "dgl/worldPivot") ;
      worldPivotShm = NULL ; 
    }
  
  first = true ;
  validate() ;
}


int dosgWorldPivotNavMod::postFrame(void)
{
  if (first)
    {

      first = false ;
      scene =  DOSG::getScene() ;
      
#ifdef AXIS
      // hang an object under where we think the pivot point is
      axis = new osg::PositionAttitudeTransform ;
      scene->addChild(axis) ;
      axis->addChild(osgDB::readNodeFile("axis.osg")) ;
#endif
      
#ifdef __APPLE__
#ifdef check
#undef check
#endif                                // check
#endif                                // __APPLE__
      navList = (dtkNavList *) mgr->check("dtkNavList", DTKNAVLIST_TYPE) ;
      if(!navList)
	{
	  navList = new dtkNavList;
	  if(mgr->add(navList))
	    {
	      delete navList;
	      navList = NULL;
	      return ERROR_;
	    }
	}
    }
  
  
  dtkNav *currentNav = navList->current() ;
  if(currentNav)
    {
      if (worldPivotShm)
	{
	  worldPivotShm->read(worldPivot) ;
	  // don't mess with pivot HPR
	  worldPivot[3] = worldPivot[4] = worldPivot[5] = 0.f ;
	  worldPivotCoord.set(worldPivot) ;
	}
      else
	{
	 worldPivotCoord = DGL::getApp()->getOrigin() ; 
	}

      //convert world coordinate to diverse coordinate
      dglPivotCoord = DOSGUtil::world_to_dgl(worldPivotCoord) ;

#ifdef AXIS
      // update axis position
      axis->setPosition(osg::Vec3(dglPivotCoord.x, dglPivotCoord.y, dglPivotCoord.z)) ;
#endif
      // don't mess with pivot HPR
      dglPivotCoord.h = 0.f ;
      dglPivotCoord.p = 0.f ;
      dglPivotCoord.r = 0.f ;

      // get a pointer to the current pivot, we just copy its hpr
      dtkCoord *pivot = currentNav->pivot() ;
      if (pivot)
	{
	  dglPivotCoord.h = pivot->h ;
	  dglPivotCoord.p = pivot->p ;
	  dglPivotCoord.r = pivot->r ;
	}
      
      // set a non-default pivot
      currentNav->pivot(&dglPivotCoord) ;
      
      
    }
  
  
  return CONTINUE;
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
  return new dosgWorldPivotNavMod(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}
