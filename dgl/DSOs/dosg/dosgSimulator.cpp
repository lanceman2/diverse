#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <dosg.h>
#include <osg/Transform>
#include <osg/Switch>

/*!
 * @class  dosgSimulator
 *
 * @brief DSO which provides an immersive environement debugging tools
 *
 * The simulator can toggle the visibililty of objects that track the head
 * and wand, and provide a "jump view" which provides an out-of-body view of
 * the head.
 *
 * The simulator is typically driven by an interface program which sends
 * messges to the dglSimulator base class.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class dosgSimulator : public DGLSimulator
{
 public:
  dosgSimulator(dtkManager *m) ;
  virtual int postConfig(void) ;

  virtual void head(bool) ;
  virtual void wand(bool) ;
  virtual void jump(bool) ;
  virtual void jumpTo(dtkCoord) ;
  
private:
  dtkManager    *manager;
  osg::Switch *wand_sw, *head_sw ;
  osg::Transform *wandNode, *headNode ;
  bool headState ;
  bool wandState ; 
  DGLViewport *jumpViewport ;
  float jump_xyzhpr[6] ;

} ;


dosgSimulator::dosgSimulator(dtkManager *m):
  DGLSimulator(m)
{
  setDescription("dosg cave simulator") ;
  
  manager = (dtkManager*) DGL::getApp();
  if (manager == NULL || manager->isInvalid())
    {
      printf("dosgSimulator::dosgSimulator: Bad manager :(\n");
      return;
    }
  
  validate() ;
}

int dosgSimulator::postConfig(void) 
{
  headState = wandState = false ;

  jumpViewport = DGL::getApp()->display()->getDebugViewport();
  //jumpViewport = DGL::getApp()->display()->getPipe(0)->getWindow(0)->getScreen(0)->getViewport(0) ;
  if (!jumpViewport)
    {
      dtkMsg.add(DTKMSG_ERROR,"dosgSimulator::dosgSimulator: failed to get jumpViewport.\n");
      return ERROR_ ;
    }

  wandNode = DOSG::getWand() ;
  if (!wandNode) 
    return ERROR_ ;
  wand_sw = new osg::Switch ;
  wandNode->addChild(wand_sw) ;
  wand_sw->addChild(osgDB::readNodeFile("wand.osg")) ;
  wand_sw->setAllChildrenOff() ;

  headNode = DOSG::getHead() ;
  if (!headNode) 
    return ERROR_ ;
  head_sw = new osg::Switch ;
  headNode->addChild(head_sw) ;
  head_sw->addChild(osgDB::readNodeFile("head.osg")) ;
  head_sw->setAllChildrenOff() ;

  return CONTINUE;
}


void dosgSimulator::head(bool v) 
{
  if (v)
    head_sw->setAllChildrenOn() ;
  else
    head_sw->setAllChildrenOff() ;
  headState = v ;
}

void dosgSimulator::wand(bool v) 
{
  if (v)
    wand_sw->setAllChildrenOn() ;
  else
    wand_sw->setAllChildrenOff() ;
  wandState = v ;
}
 
void dosgSimulator::jump(bool v) 
{
  jumpViewport->setViewState(v) ;
}

void dosgSimulator::jumpTo(dtkCoord coord) 
{
  jumpViewport->setViewCoord(coord) ;
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
  return new dosgSimulator(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}



