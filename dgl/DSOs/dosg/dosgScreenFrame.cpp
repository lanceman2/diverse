#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <dosg.h>
#include <osg/Transform>

/*!
 * @class  dosgScreenFrame
 *
 * @brief DSO which loads objects under the scene() node, transformed to
 * match each dglScreen's viewport geometry
 *
 * Each screen's visibility is based on the bits in the unsigned int stored
 * in the dtk shared memory file "dgl/dosgScreenFrame".  The first immersive
 * screnn's bit is 0, the second 1, and so forth.  When the bit is set, the
 * screen is visable, when clear, invisible.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class dosgScreenFrame : public dtkAugment
{
    public:
        dosgScreenFrame(dtkManager *m) ;
        virtual int postFrame(void) ;

    private:
        dtkManager     *manager;
        DGLDisplay     *display ;
        osg::Transform *scene ;
        osg::Node      *frameNode ;
        osg::Node      *panelNode ;
        osg::Switch    *screenSwitch ;
        dtkSharedMem   *dosgScreenFrame_shm ;
        unsigned int    dosgScreenFrame_kids ;
        unsigned int    screens ;

} ;

dosgScreenFrame::dosgScreenFrame(dtkManager *m):
dtkAugment("dosgScreenFrame")
{

    setDescription("loads objects under the scene() node, transformed to match each dglScreen's viewport geometry") ;

    manager = (dtkManager*) DGL::getApp();
    if (manager == NULL || manager->isInvalid())
    {
        printf("dosgScreenFrame::dosgScreenFrame: Bad manager :(\n");
        return;
    }
    display = DGL::getApp()->display() ;
    dosgScreenFrame_kids = 0 ;
    screens = 0 ;
    validate() ;
}


int dosgScreenFrame::postFrame(void)
{
    static bool first = true ;

    if (first)
    {
        first = false ;

#ifdef DGL_ARCH_WIN32
        dosgScreenFrame_shm =  new dtkSharedMem(sizeof(unsigned int), "dgl\\dosgScreenFrame",
#else
        dosgScreenFrame_shm =  new dtkSharedMem(sizeof(unsigned int), "dgl/dosgScreenFrame",
#endif
            &dosgScreenFrame_kids) ;
        if (!dosgScreenFrame_shm || dosgScreenFrame_shm->isInvalid())
        {
            dtkMsg.add(DTKMSG_ERROR,
                "dosgScreenFrame::postFrame failed:"
                " couldn't open shared memory segment\"dgl/dosgScreenFrame\"\n") ;
            return ERROR_ ;
        }

        dosgScreenFrame_shm->queue() ;
        dosgScreenFrame_shm->flush() ;

        scene = DOSG::getScene() ;
        if (!scene)
            return ERROR_ ;

        screenSwitch = new osg::Switch ;
        screenSwitch->setAllChildrenOff() ;
        scene->addChild(screenSwitch) ;

        frameNode = osgDB::readNodeFile("screenFrame.osg") ;
        panelNode = osgDB::readNodeFile("screenPanel.osg") ;

// for now, just load windows and positions once
        for (unsigned int p=0; p<display->getNumPipes(); p++)
        {
            DGLPipe *pipe = display->getPipe(p) ;
            for (unsigned int w=0; w<pipe->getNumWindows(); w++)
            {
                DGLWindow *window = pipe->getWindow(w) ;
                for (unsigned int s=0; s<window->getNumScreens(); s++)
                {
                    DGLScreen *screen = window->getScreen(s) ;
                    for (unsigned int v=0; v<screen->getNumViewports(); v++)
                    {
                        DGLViewport *viewport = screen->getViewport(v) ;
                        if (viewport->getViewportType() == DGLViewport::SCREEN)
                        {
                            screens++ ;
                            if (screens>8*sizeof(unsigned int))
                            {
                                dtkMsg.add(DTKMSG_ERROR,
                                    "dosgScreenFrame::postFrame failed:"
                                    " lame coding only allows for %d screens\n",8*sizeof(unsigned int)) ;
                                return ERROR_ ;

                            }
                            osg::PositionAttitudeTransform *pat = new osg::PositionAttitudeTransform ;
                            screenSwitch->addChild(pat) ;
                            dtkCoord offset = screen->getOffset() ;
                            float width = screen->getWidth() ;
                            float height = screen->getHeight() ;
//printf("height = %f, width = %f, offset:", height, width) ;
//offset.print() ;
                            pat->setScale(osg::Vec3(width, 1.f, height)) ;
                            pat->setPosition(osg::Vec3(offset.x, offset.y, offset.z)) ;
                            pat->setAttitude(osg::Quat(DOSGUtil::euler_to_quat(offset.h, offset.p, offset.r))) ;
                            pat->addChild(frameNode) ;
                            pat->addChild(panelNode) ;
                        }
                    }
                }
            }
        }

    }

    if (dosgScreenFrame_shm->qRead(&dosgScreenFrame_kids))
    {
        screenSwitch->setAllChildrenOff();
        for (unsigned int i=0; i<screens; i++)
        {
//printf("i = x%x, mask = x%x\n",i,i<<i) ;
            if (dosgScreenFrame_kids & 1<<i)
            {
                dtkMsg.add(DTKMSG_INFO,
                    "dosgScreenFrame::postFrame: turning screen on child %d\n",i) ;
                screenSwitch->setValue(i, true) ;
            }
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
    return new dosgScreenFrame(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}
