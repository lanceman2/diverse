#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <dosg.h>

/*!
 * @class dosgThreadingTest
 *
 * @brief DSO which tests multi-threading.  Change as need to find bugs.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */

class dosgThreadingTest : public dtkAugment
{
public:
    dosgThreadingTest(dtkManager *m) ;
    virtual int postConfig(void) ;
    virtual int preFrame(void) ;

private:
    osg::PositionAttitudeTransform *left, *right ;
    osg::Node *model ;
    unsigned int counter ;
    bool leftSide ;
} ;

dosgThreadingTest::dosgThreadingTest(dtkManager *m):
    dtkAugment("dosgThreadingTest")
{
    counter = 0 ;
    leftSide = true ;
    setDescription("dosg thread testing") ;
    validate() ;
}


int dosgThreadingTest::postConfig(void)
{
    left = new osg::PositionAttitudeTransform ;
    left->setPosition(osg::Vec3(-.25f, 2.f, 0.f)) ;
    DOSG::getWorld()->addChild(left) ;
    left->ref() ;

    right = new osg::PositionAttitudeTransform ;
    right->setPosition(osg::Vec3(.25f, 2.f, 0.f)) ;
    DOSG::getWorld()->addChild(right) ;
    right->ref() ;

    printf("DOSG_THREADING_TEST = %s\n",getenv("DOSG_THREADING_TEST")) ;
    if (getenv("DOSG_THREADING_TEST"))
    {
	model = osgDB::readNodeFile(getenv("DOSG_THREADING_TEST")) ;
    }
    else
    {
	model = osgDB::readNodeFile("clock.osg") ;
    }
    model->ref() ;

    left->addChild(model) ;

    return CONTINUE;
}

int dosgThreadingTest::preFrame(void)
{

    //usleep(10000) ;
    
    //printf("counter = %d\n",counter) ;
    if (counter>0)
    {
	//printf("counter = %d\n",counter) ;
	//printf("%d\n",__LINE__) ;
	if (leftSide)
	{
	    //printf("switching left to right\n") ;
	    right->addChild(model) ;
	    //printf("%d\n",__LINE__) ;
	    left->removeChild(model) ;
	    //printf("%d\n",__LINE__) ;
	}
	else
	{
	    //printf("switching right to left\n") ;
	    left->addChild(model) ;
	    //printf("%d\n",__LINE__) ;
	    right->removeChild(model) ;
	    //printf("%d\n",__LINE__) ;
	}
	counter = 0 ;
	leftSide = !leftSide ;
	//printf("%d\n",__LINE__) ;
    }
    else
    {
	counter++ ;
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
    return new dosgThreadingTest(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}
