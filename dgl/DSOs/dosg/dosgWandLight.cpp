#include <dgl.h>
#ifndef DGL_ARCH_WIN32
#include <strings.h>
#endif
#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dosg.h>
#include <osg/Transform>
#include <osg/StateAttribute>

/*!
 * @class  dosgWandLight
 *
 * @brief DSO which creates a light under the wand node
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class dosgWandLight : public dtkAugment
{
public:
  dosgWandLight(dtkManager *m) ;
  virtual int postConfig(void) ;
private:
  
} ;

dosgWandLight::dosgWandLight(dtkManager *m):
dtkAugment("dosgWandLight")
{

    setDescription("light that follows wand position and orientation") ;

    validate() ;
}

int dosgWandLight::postConfig(void)
{
  // create a new light and set its parameters
  osg::Light *light = new osg::Light;
  light->setLightNum(0);
  // someday make these values envvars
  light->setAmbient(osg::Vec4(0.8f, 0.8f, 0.8f, 1.f));
  light->setDiffuse(osg::Vec4(0.8f, 0.8f, 0.8f, 1.f));
  light->setPosition(osg::Vec4( 0.0, 0.0, 0.0, 1.0f));
  light->setDirection(osg::Vec3( 0.0f, 1.0f, 0.0f));
  light->setSpotCutoff(10.0f);
  light->setSpotExponent(0.0f);
  
  // get a new lightSource, add the light to it, put in the scenegraph
  osg::LightSource *lightSource = new osg::LightSource;
  lightSource->setName("dosgWandLight") ;
  lightSource->setLight(light);
  lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
  
  osg::Transform *wand = DOSG::getWand() ;
  if (!wand)
    {
      dtkMsg.add(DTKMSG_ERROR,"dosgWandLight:::postConfig: unable to get wand node\n") ;
      return ERROR_;
    }

  wand->addChild(lightSource);
  
  osg::StateSet *stateSet = DOSG::getScene()->getOrCreateStateSet() ;
  lightSource->setStateSetModes(*stateSet,osg::StateAttribute::ON);

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
    return new dosgWandLight(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}
