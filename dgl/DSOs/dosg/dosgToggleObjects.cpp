#include <dgl.h>
#ifndef DGL_ARCH_WIN32
#include <strings.h>
#endif
#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dosg.h>
#include <osg/Switch>

#define NODES (4)

/*!
 * @class  dosgToggleObjects
 *
 * @brief DSO which toggles the visibility of objects based on DGLMessage data
 *
 * Items toggled are:
 * - cubeFrame
 * - cubePanel
 * - frame
 * - axis
 * - gnomon
 * - text with the name of a scenegraph node, using that node's transformation
 *
 * DGLMessage data is send using a DGLPostOffice using the dtk shared memory
 * file "dgl/dosgToggleObjects"
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class dosgToggleObjects : public dtkAugment
{
public:
    dosgToggleObjects(dtkManager *m) ;
    virtual int postFrame(void) ;
    virtual int postConfig(void) ;

private:
    DGLMessage *msg ;
    DGLPostOffice *msgSender ;

    osg::Node *cubeFrame ;
    osg::Node *cubePanel ;
    osg::Node *frame ;
    osg::Node *axis ;
    osg::Node *gnomon ;
    osg::Node *sceneText ;
    osg::Node *etherText ;
    osg::Node *navText ;
    osg::Node *worldText ;

    osg::Switch *scene_sw ;
    osg::Switch *ether_sw ;
    osg::Switch *nav_sw ;
    osg::Switch *world_sw ;

    enum NODENAME { SCENE=0, ETHER=1, NAV=2, WORLD=3 }
        ;
    class objVis
    {
    public:
	void clear(void)
	{ cubeFrame = cubePanel = frame = axis = gnomon = text = 0 ; }
	;
	int cubeFrame ;
	int cubePanel ;
	int frame ;
	int axis ;
	int gnomon ;
	int text ;
	osg::Switch *node ;
	char *name ;
    } ;

    objVis nodeInfo[NODES] ;

    void toggler(int i, const char *object, int v) ;
} ;

dosgToggleObjects::dosgToggleObjects(dtkManager *m):
    dtkAugment("dosgToggleObjects")
{

    setDescription("dosg toggle objects") ;

    //Class for sending the message
    msgSender = new DGLPostOffice("dgl/dosgToggleObjects");
    msg = NULL ;

    for (int i = 0; i<NODES; i++)
        nodeInfo[i].clear() ;

    validate() ;
}


int dosgToggleObjects::postConfig(void)
{
    scene_sw =  new osg::Switch ;
    DOSG::getScene()->addChild(scene_sw) ;

    ether_sw =  new osg::Switch ;
    DOSG::getEther()->addChild(ether_sw) ;

    nav_sw =  new osg::Switch ;
    DOSG::getNav()->addChild(nav_sw) ;

    world_sw =  new osg::Switch ;
    DOSG::getWorld()->addChild(world_sw) ;

    nodeInfo[SCENE].node = scene_sw ;
    nodeInfo[ETHER].node = ether_sw ;
    nodeInfo[NAV].node = nav_sw ;
    nodeInfo[WORLD].node = world_sw ;

    nodeInfo[SCENE].name = strdup("scene") ;
    nodeInfo[ETHER].name = strdup("ether" );
    nodeInfo[NAV].name = strdup("nav") ;
    nodeInfo[WORLD].name = strdup("world") ;

    gnomon = osgDB::readNodeFile("gnomon.osg") ;
    scene_sw->addChild(gnomon) ;
    ether_sw->addChild(gnomon) ;
    nav_sw->addChild(gnomon) ;
    world_sw->addChild(gnomon) ;

    cubeFrame = osgDB::readNodeFile("cubeFrame.osg") ;
    scene_sw->addChild(cubeFrame) ;
    ether_sw->addChild(cubeFrame) ;
    nav_sw->addChild(cubeFrame) ;
    world_sw->addChild(cubeFrame) ;

    cubePanel = osgDB::readNodeFile("cubePanel.osg") ;
    scene_sw->addChild(cubePanel) ;
    ether_sw->addChild(cubePanel) ;
    nav_sw->addChild(cubePanel) ;
    world_sw->addChild(cubePanel) ;

    axis = osgDB::readNodeFile("axis.osg") ;
    scene_sw->addChild(axis) ;
    ether_sw->addChild(axis) ;
    nav_sw->addChild(axis) ;
    world_sw->addChild(axis) ;

    frame = osgDB::readNodeFile("frame.osg") ;
    scene_sw->addChild(frame) ;
    ether_sw->addChild(frame) ;
    nav_sw->addChild(frame) ;
    world_sw->addChild(frame) ;

    sceneText = osgDB::readNodeFile("scene.osg") ;
    scene_sw->addChild(sceneText) ;
    ether_sw->addChild(sceneText) ;
    nav_sw->addChild(sceneText) ;
    world_sw->addChild(sceneText) ;

    etherText = osgDB::readNodeFile("ether.osg") ;
    scene_sw->addChild(etherText) ;
    ether_sw->addChild(etherText) ;
    nav_sw->addChild(etherText) ;
    world_sw->addChild(etherText) ;

    navText = osgDB::readNodeFile("nav.osg") ;
    scene_sw->addChild(navText) ;
    ether_sw->addChild(navText) ;
    nav_sw->addChild(navText) ;
    world_sw->addChild(navText) ;

    worldText = osgDB::readNodeFile("world.osg") ;
    scene_sw->addChild(worldText) ;
    ether_sw->addChild(worldText) ;
    nav_sw->addChild(worldText) ;
    world_sw->addChild(worldText) ;

    scene_sw->setAllChildrenOff() ;
    ether_sw->setAllChildrenOff() ;
    nav_sw->setAllChildrenOff() ;
    world_sw->setAllChildrenOff() ;

    return dtkAugment::CONTINUE;
}


int dosgToggleObjects::postFrame(void)
{
    while ((msg = msgSender->getMessage()))
    {
        static const char *nodeName ;
        nodeName = msg->getSubject().c_str() ;
        const char *object = msg->getString().c_str() ;
        int value =  msg->getInt() ;
        for (int i=0; i<NODES; i++)
        {
            if (!strcasecmp(nodeName,nodeInfo[i].name))
                toggler(i, object, value) ;
        }
        msg->clear() ;
    }

    return CONTINUE;
}


void dosgToggleObjects::toggler(int i, const char *object, int v)
{

    if (!strcasecmp(object, "cubePanel"))
    {
        if (v != nodeInfo[i].cubePanel)
        {
            if (v)
            {
                nodeInfo[i].node->setChildValue(cubePanel, true) ;
            }
            else
            {
                nodeInfo[i].node->setChildValue(cubePanel, false) ;
            }
            nodeInfo[i].cubePanel = v ;
        }
    }
    else if (!strcasecmp(object, "cubeFrame"))
    {
        if (v != nodeInfo[i].cubeFrame)
        {
            if (v)
            {
                nodeInfo[i].node->setChildValue(cubeFrame, true) ;
            }
            else
            {
                nodeInfo[i].node->setChildValue(cubeFrame, false) ;
            }
            nodeInfo[i].cubeFrame = v ;
        }
    }
    else if (!strcasecmp(object, "axis"))
    {
        if (v != nodeInfo[i].axis)
        {
            if (v)
            {
                nodeInfo[i].node->setChildValue(axis, true) ;
            }
            else
            {
                nodeInfo[i].node->setChildValue(axis, false) ;
            }
            nodeInfo[i].axis = v ;
        }
    }
    else if (!strcasecmp(object, "gnomon"))
    {
        if (v != nodeInfo[i].gnomon)
        {
            if (v)
            {
                nodeInfo[i].node->setChildValue(gnomon, true) ;
            }
            else
            {
                nodeInfo[i].node->setChildValue(gnomon, false) ;
            }
            nodeInfo[i].gnomon = v ;
        }
    }
    else if (!strcasecmp(object, "frame"))
    {
        if (v != nodeInfo[i].frame)
        {
            if (v)
            {
                nodeInfo[i].node->setChildValue(frame, true) ;
            }
            else
            {
                nodeInfo[i].node->setChildValue(frame, false) ;
            }
            nodeInfo[i].frame = v ;
        }
    }
    else if (!strcasecmp(object, "text"))
    {
        if (nodeInfo[i].node == scene_sw)
        {
            if (v != (nodeInfo[i].text))
            {
                if (v)
                {
		    nodeInfo[i].node->setChildValue(sceneText, true) ;
                }
                else
                {
		    nodeInfo[i].node->setChildValue(sceneText, false) ;
                }
                nodeInfo[i].text = v ;
            }
        }
        else if (nodeInfo[i].node == ether_sw)
        {
            if (v != (nodeInfo[i].text))
            {
                if (v)
                {
		    nodeInfo[i].node->setChildValue(etherText, true) ;
                }
                else
                {
		    nodeInfo[i].node->setChildValue(etherText, false) ;
                }
                nodeInfo[i].text = v ;
            }
        }
        else if (nodeInfo[i].node == nav_sw)
        {
            if (v != (nodeInfo[i].text))
            {
                if (v)
                {
		    nodeInfo[i].node->setChildValue(navText, true) ;
                }
                else
                {
		    nodeInfo[i].node->setChildValue(navText, false) ;
                }
                nodeInfo[i].text = v ;
            }
        }
        else if (nodeInfo[i].node == world_sw)
        {
            if (v != (nodeInfo[i].text))
            {
                if (v)
                {
		    nodeInfo[i].node->setChildValue(worldText, true) ;
                }
                else
                {
		    nodeInfo[i].node->setChildValue(worldText, false) ;
                }
                nodeInfo[i].text = v ;
            }
        }
    }
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
    return new dosgToggleObjects(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}
