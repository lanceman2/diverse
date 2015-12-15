#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <dosg.h>

//#define AXIS

/*!
 * @class dosgCenterPivotNavMod
 *
 * @brief DSO which sets the pivot point of the current navigation to the
 * center of scenegraph's bounding box
 *
 * The pivot point is only calculated once, in the first call to postFrame.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
static dtkCoord centerPivot ;

class dosgCenterPivotNavMod : public dtkAugment
{
    public:
        dosgCenterPivotNavMod(dtkManager *m) ;
        virtual int postFrame(void) ;

    private:
        dtkManager *mgr ;
        dtkNavList *navList;
        osg::Transform *scene ;
        bool gotGeometry ;
        osg::Vec3 center ;
        dtkMatrix centerMat ;
        dtkCoord centerPivot ;

#ifdef AXIS
        osg::PositionAttitudeTransform *axis ;
#endif
} ;

dosgCenterPivotNavMod::dosgCenterPivotNavMod(dtkManager *m):
dtkAugment("dosgCenterPivotNavMod")
{

    mgr = m ;
    setDescription("dosg toggle objects") ;
    gotGeometry = false ;
    navList = NULL;
    validate() ;
}


int dosgCenterPivotNavMod::postFrame(void)
{
// wait until we get some geometry to measure
    if (!gotGeometry)
    {
        scene =  DOSG::getScene() ;

        osg::BoundingBox bbox = DOSGUtil::computeBoundingBox(*scene) ;

        center = (bbox._min + bbox._max)/2.f ;
        float radius = ((bbox._max - bbox._min).length())/2.f ;

        if (radius>0.f)
        {
            gotGeometry = true ;

            centerMat.identity() ;
            centerMat.translate(center.x(), center.y(), center.z()) ;

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
    }

    if (gotGeometry)
    {
        dtkNav *currentNav = navList->current() ;
        if(currentNav)
        {
// get a pointer to the current pivot, we just copy its hpr
            dtkCoord *pivot = currentNav->pivot() ;
            if (pivot)
            {
                centerPivot.h = pivot->h ;
                centerPivot.p = pivot->p ;
                centerPivot.r = pivot->r ;
            }

// set a non-default pivot
            currentNav->pivot(&centerPivot) ;

// place the pivot point- it moves with navigation
            dtkMatrix pivotMat ;
            pivotMat.translate(center.x(), center.y(), center.z()) ;
            pivotMat.mult(currentNav->matrix()) ;
// put it into non-default pivot
            pivotMat.translate(&centerPivot.x, &centerPivot.y, &centerPivot.z) ;
#ifdef AXIS
// update axis position
            axis->setPosition(osg::Vec3(centerPivot.x, centerPivot.y, centerPivot.z)) ;
#endif

        }
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
    return new dosgCenterPivotNavMod(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}
