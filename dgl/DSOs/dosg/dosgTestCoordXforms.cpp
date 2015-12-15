#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <dosg.h>

/*!
 * @class dosgTestCoordXforms
 *
 * @brief DSO tests the world to dgl coordinate transformations
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */


class dosgTestCoordXforms : public dtkAugment
{
public:
  dosgTestCoordXforms(dtkManager *m) ;
  virtual int postFrame(void) ;
  
private:
  dtkManager *mgr ;
  bool first ;
  osg::Transform *scene ;
  osg::Transform *world ;
  osg::PositionAttitudeTransform *axis ;

} ;

dosgTestCoordXforms::dosgTestCoordXforms(dtkManager *m):
  dtkAugment("dosgTestCoordXforms")
{
  
  mgr = m ;
  setDescription("tests coordinate conversions") ;

  first = true ;
  validate() ;
}


int dosgTestCoordXforms::postFrame(void)
{
  if (first)
    {
      first = false ;
      scene =  DOSG::getScene() ;
      world = DOSG::getWorld() ;
      
      // hang objects under the scene and world nodes
      // the PAT under the scene node will get the world transformation
      axis = new osg::PositionAttitudeTransform ;
      scene->addChild(axis) ;
      axis->addChild(osgDB::readNodeFile("axis.osg")) ;
      
    }
  
#ifdef __APPLE__
#ifdef check
#undef check
#endif                                // check
#endif                                // __APPLE__

  // identity world coord
  dtkCoord w ;

  // dgl coordinate
  dtkCoord c = DOSGUtil::world_to_dgl(w) ;

  // and a check- w2 "should" be almost the same as w
  dtkCoord w2 = DOSGUtil::dgl_to_world(c) ;
  w2.x -= w.x ;
  w2.y -= w.y ;
  w2.z -= w.z ;
  w2.h -= w.h ;
  w2.p -= w.p ;
  w2.r -= w.r ;
  //printf("%f %f %f %f %f %f                     \r",w2.x, w2.y, w2.z, w2.h, w2.p, w2.r) ;
  //fflush(stdout) ;
  if (w2.x > .001 || w2.x < -.001 ||
      w2.y > .001 || w2.y < -.001 ||
      w2.z > .001 || w2.z < -.001 ||
      w2.h > .001 || w2.h < -.001 ||
      w2.p > .001 || w2.p < -.001 ||
      w2.r > .001 || w2.r < -.001)
    {
      w2.print() ;
    }

  // update axis position
  axis->setPosition(osg::Vec3(c.x, c.y, c.z)) ;
  axis->setAttitude(osg::Quat(DOSGUtil::euler_to_quat(c.h, c.p, c.r))) ;
  
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
  return new dosgTestCoordXforms(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}
