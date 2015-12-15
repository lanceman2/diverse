/*!
 * @brief loads files
 */
 
// loads files
// John Kelso, kelso@nist.gov 4/06

//#define DEBUG 1

#define ORIGINCENTERED

#include<osgDB/WriteFile>
#include<osgDB/Registry>
#include<osg/LightModel>

#include <dgl.h>
#include <dosg.h>
#include "global.h"

#ifdef DGL_ARCH_WIN32
#  define SEPARATOR ";"
#else
#  define SEPARATOR ":"
#endif

#ifdef check
#undef check
#endif

// from processArgs.C
extern int processArgs(int argc, char **argv);

// hang a light under node at a given position
int makeLight(osg::Vec4 pos, osg::Group *node) {

    // create a new light and set its parameters
    osg::Light *light = new osg::Light;
    light->setLightNum(0);
    light->setPosition(pos);
    light->setAmbient(osg::Vec4(ambient, ambient, ambient, 1.f));
    light->setDiffuse(osg::Vec4(diffuse, diffuse, diffuse, 1.f));
    light->setSpecular(osg::Vec4(specular, specular, specular, 1.f));

    // get a new lightSource, add the light to it, put in the scenegraph
    osg::LightSource *lightSource = new osg::LightSource;
    lightSource->setLight(light);

    lightSource->setLocalStateSetModes(osg::StateAttribute::ON);

    // get a stateset in the node
    osg::StateSet *stateSet = DOSG::getScene()->getOrCreateStateSet() ;
    lightSource->setStateSetModes(*stateSet,osg::StateAttribute::ON);


    osg::LightModel* lightModel = new osg::LightModel;

    // get specular highlights to look correct across pipes
    lightModel->setLocalViewer(true) ;

    // turn on two sided lighting if not backface culling
    if (!backfacecull)
    {
	lightModel->setTwoSided(true) ;
    }
    else
    {
	lightModel->setTwoSided(false) ;
    }
    stateSet->setAttribute(lightModel);

    if(!node->addChild(lightSource)) 
    {
	dtkMsg.add(DTKMSG_ERROR,
		   "dosg-fly,makeLight: addChild failed.\n") ;
	return 1 ; // failure, error
    }
    return 0 ;
}

// counter of light numbers
int lightNum = 0 ;

class renumberLightVisitor : public osg::NodeVisitor
{
public :
    renumberLightVisitor() : 
	osg::NodeVisitor( TRAVERSE_ALL_CHILDREN ) {} ;
    
    virtual void apply( osg::LightSource &node )
    {

	int oldLightNum ;
	int newLightNum ;
	osg::StateAttribute::GLModeValue mode ;
	osg::Light *light = node.getLight() ;
	if (light)
	{
	    newLightNum = lightNum ;
	    lightNum++ ;
	    oldLightNum = light->getLightNum() ;
	    dtkMsg.add(DTKMSG_INFO,
		       "dosg-fly:renumberLightVisitor- renumbering light from %d to %d\n",oldLightNum, newLightNum) ;
	    light->setLightNum(newLightNum) ;

	    if (node.getName() == "")
	    {
		char name[100] ;
		sprintf(name, "dosg-fly_LightSource_%d", newLightNum) ;
		node.setName(name) ;
	    }



	    osg::StateSet *stateset = node.getOrCreateStateSet() ;
	    osg::StateSet *sceneStateSet = DOSG::getScene()->getOrCreateStateSet() ;

	    node.setStateSetModes(*sceneStateSet,osg::StateAttribute::ON);
	    mode = stateset->getMode(GL_LIGHT0+oldLightNum) ;
	    stateset->removeMode(GL_LIGHT0+oldLightNum) ;
	    stateset->setMode(GL_LIGHT0+newLightNum, mode) ;

	    numLights++ ;
	}
	traverse(node);
    }
} ;

int main(int argc, char **argv) 
{
  
    if (processArgs(argc, argv)) return 1 ;
  
    // set the search path for model files
    std::string path ;
    path = "." SEPARATOR DGL_DATA_DIR ;
    char *osgfilepath = getenv("OSG_FILE_PATH") ;
    if (osgfilepath)
    {
	path += SEPARATOR ;
	path += osgfilepath ;
    }
    osgDB::Registry::instance()->setDataFilePathList(path) ;

    // add the osgPlugins directory automatically
    osgDB::FilePathList lib_path_list = osgDB::Registry::instance()->getLibraryFilePathList();
    lib_path_list.push_back( OSG_PLUGINS_DIR );
    osgDB::Registry::instance()->setLibraryFilePathList( lib_path_list );
  
    //Create new DGL and DOSG objects
    DOSG::init();
  
    app = DGL::getApp();
  
    display = app->display();
  
    // do preConfig callbacks for all DGL, DOSG, and all DSOs
    if(app->preConfig()) return 1;
  
    // do Config callbacks for all DGL, DOSG, and all DSOs
    if(app->config()) return 1 ;
  
    // do postConfig callbacks for all DGL, DOSG, and all DSOs
    if(app->postConfig()) return 1 ;
  

#ifndef UNIMPLEMENTED_NEARFAR
    if (near)
    {
	for (int p=0; p<display->getNumPipes(); p++)
	{
	    for (int w=0; w<display->getPipe(p)->getNumWindows(); w++)
	    {
		for (int s=0; s<display->getPipe(p)->getWindow(w)->getNumScreens(); s++)
		{
		    display->getPipe(p)->getWindow(w)->getScreen(s)->setNear(*near) ;
		}
	    }
	}
    }

    if (far)
    {
	for (int p=0; p<display->getNumPipes(); p++)
	{
	    for (int w=0; w<display->getPipe(p)->getNumWindows(); w++)
	    {
		for (int s=0; s<display->getPipe(p)->getWindow(w)->getNumScreens(); s++)
		{
		    display->getPipe(p)->getWindow(w)->getScreen(s)->setFar(*far) ;
		}
	    }
	}
    }
#endif
#ifndef UNIMPLEMENTED
    if (convergence)
    {
	for (int p=0; p<display->getNumPipes(); p++)
	{
	    for (int w=0; w<display->getPipe(p)->getNumWindows(); w++)
	    {
		for (int s=0; s<display->getPipe(p)->getWindow(w)->getNumScreens(); s++)
		{
		    display->getPipe(p)->getWindow(w)->getScreen(s)->setConvergence(*convergence) ;
		}
	    }
	}
    }
#endif
    if (interoccular)
    {
	for (int p=0; p<display->getNumPipes(); p++)
	{
	    for (int w=0; w<display->getPipe(p)->getNumWindows(); w++)
	    {
		for (int s=0; s<display->getPipe(p)->getWindow(w)->getNumScreens(); s++)
		{
		    display->getPipe(p)->getWindow(w)->getScreen(s)->setInterOccular(*interoccular) ;
		}
	    }
	}
    }


#ifndef UNIMPLEMENTED
    if (splash)
    {
	if (!getenv("DPF_SPLASH_SCREEN"))
	{
	    putenv("DPF_SPLASH_SCREEN=diversiflySplashScreen.iv");
	}
	if ((timeout) && (!app->check("splashScreen")))
	    app->load("splashScreen") ;
      
	if ((!timeout) && (!app->check("splashScreenNoTimeout"))) 
	{
	    app->load("splashScreenNoTimeout") ;
	}
    }
#endif
  
    if (!null && !world.empty() && !head.empty() && !wand.empty() && !ether.empty() && !scene.empty() && !nochild.empty()) 
    {
	dtkMsg.add(DTKMSG_ERROR, 
		   "No files specified to be loaded, and NULL not specified!\n") ;
	return 1 ;
    }
  
    osg::Transform *scenen = DOSG::getScene() ;
    if (!scenen) 
	return 1 ;
  
    osg::Transform *headn = DOSG::getHead() ;
    if (!headn) 
	return 1 ;
  
    osg::Transform *wandn = DOSG::getWand() ;
    if (!wandn) 
	return 1 ;
  
    osg::Transform *ethern = DOSG::getEther() ;
    if (!ethern) 
	return 1 ;
  
    osg::Transform *navn = DOSG::getNav() ;
    if (!navn) 
	return 1 ;
  
    osg::Transform *worldn = DOSG::getWorld() ;
    if (!worldn) 
	return 1 ;
  
    // default light
    //if (nolights && sceneLight.size()==0 && etherLight.size()==0  && navLight.size()==0  && worldLight.size()==0) 
    if (nolights)
    {
	if (verbose) printf("turning off default light\n") ;
	DOSG::getScene()->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::OFF);
    }
    else
    {
	if (verbose) printf("adding default light to scenegraph\n") ;
	sceneLight.push_back(osg::Vec4(0.0f, -sqrt(2.0f), sqrt(2.0f), 0.f)) ;
    }
  
    int c ;
    for (c=0; c<sceneLight.size(); c++) 
    {
	if (makeLight(sceneLight[c],DOSG::getScene()))
	    return 1 ;
    }
  
    for (c=0; c<etherLight.size(); c++) 
    {
	if (makeLight(etherLight[c],DOSG::getEther()))
	    return 1 ;
    }
  
    for (c=0; c<navLight.size(); c++) 
    {
	if (makeLight(navLight[c],DOSG::getNav()))
	    return 1 ;
    }
  
    for (c=0; c<worldLight.size(); c++) 
    {
	if (makeLight(worldLight[c],DOSG::getWorld()))
	    return 1 ;
    }
    
    // Load graphics object files
    // doesn't evey program have one of these?
    int i ;
  
    for (i=0; i<scene.size(); i++) 
    {
	osg::Node *node =  osgDB::readNodeFile(std::string(scene[i])) ;
	if (node) 
	{
	    if (setname)
	    {
		node->setName(scene[i]) ;
		DOSGUtil::nodeList::add(scene[i], node) ;
	    }
	    if (!scenen->addChild(node)) 
	    {
		dtkMsg.add(DTKMSG_ERROR,
			   "scene->addChild(node) failed.\n") ;
		return 1 ;
	    }
	} 
	else 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to load file %s\n", scene[i].c_str()) ;
	    return 1 ;
	}
    }
  
    for (i=0; i<head.size(); i++) 
    {
	osg::Node *node = osgDB::readNodeFile(std::string(head[i])) ;
	if (node) 
	{
	    if (setname)
	    {
		node->setName(head[i]) ;
		DOSGUtil::nodeList::add(head[i], node) ;
	    }
	    if (!headn->addChild(node)) 
	    {
		dtkMsg.add(DTKMSG_ERROR,
			   "head->addChild(node) failed.\n") ;
		return 1 ;
	    }
	} 
	else 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to load file %s\n", head[i].c_str()) ;
	    return 1 ;
	}
    }
  
    for (i=0; i<wand.size(); i++) 
    {
	osg::Node *node = osgDB::readNodeFile(std::string(wand[i])) ;
	if (node) 
	{
	    if (setname)
	    {
		node->setName(wand[i]) ;
		DOSGUtil::nodeList::add(wand[i], node) ;
	    }
	    if (!wandn->addChild(node)) 
	    {
		dtkMsg.add(DTKMSG_ERROR,
			   "wand->addChild(node) failed.\n") ;
		return 1 ;
	    }
	} 
	else 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to load file %s\n", wand[i].c_str()) ;
	    return 1 ;
	}
    }
  
    for (i=0; i<ether.size(); i++) 
    {
	osg::Node *node = osgDB::readNodeFile(std::string(ether[i])) ;
	if (node) 
	{
	    if (setname)
	    {
		node->setName(ether[i]) ;
		DOSGUtil::nodeList::add(ether[i], node) ;
	    }
	    if (!ethern->addChild(node)) 
	    {
		dtkMsg.add(DTKMSG_ERROR,
			   "ether->addChild(node) failed.\n") ;
		return 1 ;
	    }
	} 
	else 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to load file %s\n", ether[i].c_str()) ;
	    return 1 ;
	}
    }
  
    for (i=0; i<nochild.size(); i++) 
    {
	osg::Node *node = osgDB::readNodeFile(std::string(nochild[i])) ;
	if (node) 
	{
	    if (setname)
	    {
		node->setName(nochild[i]) ;
		DOSGUtil::nodeList::add(nochild[i], node) ;
	    }
	} 
	else 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to load file %s\n", nochild[i].c_str()) ;
	    return 1 ;
	}
    }
  
    for (i=0; i<nav.size(); i++) 
    {
	osg::Node *node = osgDB::readNodeFile(std::string(nav[i])) ;
	if (node) 
	{
	    if (setname)
	    {
		node->setName(nav[i]) ;
		DOSGUtil::nodeList::add(nav[i], node) ;
	    }
	    if (!navn->addChild(node)) 
	    {
		dtkMsg.add(DTKMSG_ERROR,
			   "nav->addChild(node) failed.\n") ;
		return 1 ;
	    }
	} 
	else 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to load file %s\n", nav[i].c_str()) ;
	    return 1 ;
	}
    }
  
    for (i=0; i<world.size(); i++) 
    {
	osg::Node *node = osgDB::readNodeFile(std::string(world[i])) ;
	if (node) 
	{
	    if (setname)
	    {
		node->setName(world[i]) ;
		DOSGUtil::nodeList::add(world[i], node) ;
	    }
	    if (!worldn->addChild(node)) 
	    {
		dtkMsg.add(DTKMSG_ERROR,
			   "world->addChild(node) failed.\n") ;
		return 1 ;
	    }
	} 
	else 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to load file %s\n", world[i].c_str()) ;
	    return 1 ;
	}
    }
  
  
    // get the initial bounding box around the loaded files
    osg::BoundingBox bbox ;
    bbox.init() ;
    bbox = DOSGUtil::computeBoundingBox(*worldn) ;
    osg::Vec3 scenter = (bbox._max + bbox._min)/2.f ;
    float radius = ((bbox._max - bbox._min).length())/2.f ;
  
    // if nothing is found, just set to default 
    if (radius<=0.f || radius>=FLT_MAX)
    {
	dtkMsg.add(DTKMSG_WARNING, "No geometry found, setting bounding radius to 1, center to 0,0,0\n") ;
	radius = 1.f ;
	scenter = osg::Vec3(0.f, 0.f, 0.f) ;
    }

    if (verbose) 
    {
	printf("Bounding sphere for all objects loaded under world:\n") ;
	printf("   radius = %f\n",radius) ;
	printf("   center = (%f %f %f)\n",
	       scenter.x(), scenter.y(), scenter.z()) ; 
    }
  
#ifdef ORIGINCENTERED
    dtkCoord origin(0,0,0,0,0,0) ;
    if (xyz)
    {
	origin.x = (*xyz).x ;
	origin.y = (*xyz).y ;
	origin.z = (*xyz).z ;
    }
    if (hpr)
    {
	origin.h = (*hpr).h ;
	origin.p = (*hpr).p ;
	origin.r = (*hpr).r ;
    }
    if (bound)
    {
	scale = (*bound)/radius;
    }
    if (center)
    {
	origin.x =  (*center).x/scale - scenter.x();
	origin.y =  (*center).y/scale - scenter.y();
	origin.z =  (*center).z/scale - scenter.z();

	// this works, which I think implies origin is not using scaled coordinates!
	origin.x =  (*center).x - scenter.x()*scale;
	origin.y =  (*center).y - scenter.y()*scale;
	origin.z =  (*center).z - scenter.z()*scale;

    }


    if (examine)
    {
	dtkMatrix m = dtkMatrix(origin);
	m.invert();
	m.coord(&origin);
    }

    app->setOrigin(origin) ;
    app->setScale(scale) ;

    if (verbose)
    {
	printf("origin:") ;
	origin.print() ;
	printf("scale: %f\n",scale) ;
    }
    
#else
    // this was lifted from osgconv's OrientationConverter class
  
    // Order of operations here is :
    // 1. Translate center of objects to world origin (0,0,0)
    // 2. Rotate to new orientation
    // 3. Scale in new orientation coordinates
    // 4. If an absolute translation was specified then
    //        - translate to absolute translation in world coordinates
    //    else
    //        - translate back to model's original origin. 
  

    dtkMatrix C, R, S, T ;
    C.translate(-scenter.x(), -scenter.y(), -scenter.z()) ; 
    //~printf("initial centering translation = %f %f %f\n",-scenter.x(), -scenter.y(), -scenter.z()) ; 

    if (hpr)
    {
	R.rotateHPR(hpr->h, hpr->p, hpr->r) ;
	R.invert() ;
	//~printf("hpr = %f %f %f\n",hpr->h, hpr->p, hpr->r);
    }

    S.scale(scale);
    //~printf("scale = %f\n",scale);

    if (center)
    {
	T.translate(center->x, center->y, center->z);
	//~printf("center translation = %f %f %f\n",center->x, center->y, center->z) ;
    }
    else if (xyz)
    {
	T.translate(scenter.x()-xyz->x, scenter.y()-xyz->y, scenter.z()-xyz->z);
	//~printf("xyz translation = %f %f %f\n",xyz->x, xyz->y, xyz->z) ;
    }
    else
    { 
	T.translate(scenter.x(), scenter.y(), scenter.z()) ; 
	//~printf("translation back = %f %f %f\n",scenter[0], scenter[1], scenter[2]) ;
    }


    dtkMatrix transform ;

    transform.mult(&C) ;
    transform.mult(&R) ;
    transform.mult(&S) ;
    transform.mult(&T) ;
    if (verbose)
    {
	printf("Ether Matrix:\n") ;
	transform.print() ;
    }

    // get the goods back out of the matrix
    {
	dtkVec3 xlate ;
	transform.translate(&(xlate.x), &(xlate.y), &(xlate.z)) ;
	float s = transform.scale() ; 
	dtkVec3 rot ;
	transform.rotateHPR(&(rot.h), &(rot.p), &(rot.r)) ;

	dtkCoord origin(&xlate, &rot) ;
	app->setOrigin(origin) ;
	app->setScale(s) ;

	//~printf("%s %d: setting scale to %f, setting origin to",__FILE__,__LINE__,s) ;
	//~origin.print() ;

    }
#endif

    // ********************* Get the dtkNavList ***********************
    dtkNavList *navList = (dtkNavList *)
	app->check("dtkNavList", DTKNAVLIST_TYPE) ;
    if ((!navList) || (!(navList->first())))
	dtkMsg.add(DTKMSG_WARN, "No navigator loaded\n") ;
  
  
#ifndef UNIMPLEMENTED
    // apply lod
    pfChannel *chan ;
    if (display->masterScreen()) 
    {
	if ((chan=display->masterScreen()->pfchan(0))) 
	{
	    chan->setLODAttr(PFLOD_SCALE, lod);
	    chan->setLODAttr(PFLOD_FADE, 2.0);
	}
	if ((chan=display->masterScreen()->pfchan(1))) 
	{
	    chan->setLODAttr(PFLOD_SCALE, lod);
	    chan->setLODAttr(PFLOD_FADE, 2.0);
	}
    }
#endif
  
  
    osg::StateSet *stateSet = DOSG::getScene()->getOrCreateStateSet() ;
    if (!backfacecull) 
    {
	stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF) ;
    }
    else
    {
	stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::ON) ;
    }
  
#ifndef UNIMPLEMENTED
    if (axis) 
    {
	dtkCoord position ;
	if (pivot)
	    position = *pivot ;
	osg::Transform *dcs = new
	    dpfDCS(
		   &position, // pointer to the data that moves it 
		   1.0f, //  overall scale 
		   // file           scale          xyz position xyz  rotation hpr 
		   "axis.pfb",      1.0, 1.0, 1.0,  0.0, 0.0, 0.0,    0.0, 0.0, 0.0,
		   NULL);
	world->addChild(dcs);
    }
#endif
  
    if (verbose) 
    {
	float DUL = app->getMeters() ;
	float MUL = app->getMeters()*scale;
      
	//display->print() ;
	printf("one DIVERSE unit length = %f meters\n", DUL) ;
	printf("interoccular     = %f\n",
	       display->getPipe(0)->getWindow(0)->getScreen(0)->getInterOccular()) ;
#ifndef UNIMPLEMENTED
	printf("convergence      = %f\n",
	       display->getPipe(0)->getWindow(0)->getScreen(0)->convergence()) ;
	printf("level of detail  = %f\n",lod) ;
#endif
	if (backfacecull) 
	{
	    printf("backface culling is enabled\n") ;
	} 
	else 
	{
	    printf("backface culling is disabled\n") ;
	}
	printf("one model unit length (MUL) = %f meters\n", MUL);
	if (DOSG::getAutoNearFar())
	{
	    printf("near and far will be automatically calculated each frame based on the size of\n"
		   "the scene's bounding sphere\n") ;
	}
	else
	{
	    printf("near  = %f MULs = %f meters\n",
		   display->getPipe(0)->getWindow(0)->getScreen(0)->getNear(),
		   display->getPipe(0)->getWindow(0)->getScreen(0)->getNear()*MUL) ;
	    printf("far   = %f MULs = %f meters\n",
		   display->getPipe(0)->getWindow(0)->getScreen(0)->getFar(),
		   display->getPipe(0)->getWindow(0)->getScreen(0)->getFar()*MUL) ;
	}
	printf("%d files to be loaded under the scene node:\n", scene.size()) ;
	for (i=0; i<scene.size(); i++) 
	{
	    printf("  %d: %s\n",i+1,scene[i].c_str()) ;
	}
	printf("%d files to be loaded under the head node:\n", head.size()) ;
	for (i=0; i<head.size(); i++) 
	{
	    printf("  %d: %s\n",i+1,head[i].c_str()) ;
	}
	printf("%d files to be loaded under the wand node:\n", wand.size()) ;
	for (i=0; i<wand.size(); i++) 
	{
	    printf("  %d: %s\n",i+1,wand[i].c_str()) ;
	}
	printf("%d files to be loaded under the ether node:\n", ether.size()) ;
	for (i=0; i<ether.size(); i++) 
	{
	    printf("  %d: %s\n",i+1,ether[i].c_str()) ;
	}
	printf("%d files to be loaded but not under any node:\n", nochild.size()) ;
	for (i=0; i<nochild.size(); i++) 
	{
	    printf("  %d: %s\n",i+1,nochild[i].c_str()) ;
	}
	printf("%d files to be loaded under the nav node:\n", nav.size()) ;
	for (i=0; i<nav.size(); i++) 
	{
	    printf("  %d: %s\n",i+1,nav[i].c_str()) ;
	}
	printf("%d files to be loaded under the world node:\n", world.size()) ;
	for (i=0; i<world.size(); i++) 
	{
	    printf("  %d: %s\n",i+1,world[i].c_str()) ;
	}
    }
  
    if (pivot) 
    {
	if (verbose) 
	{
	    printf("Setting pivot to (%f, %f, %f) (%f, %f, %f)\n",
		   (*pivot).x, (*pivot).y, (*pivot).z,
		   (*pivot).h, (*pivot).p, (*pivot).r) ;
	  
	}
	dtkNav *nav ;
	for (nav = navList->first(); nav; nav = navList->next(nav)) 
	{
	    nav->pivot(pivot)  ;
	}
    }
  
    if (response) 
    {
	if (verbose) 
	{
	    printf("Setting response to %f\n",*response) ;
	}
	dtkNav *nav ;
	for (nav = navList->first(); nav; nav = navList->next(nav)) 
	{
	    nav->response(*response)  ;
	}
    }
  
#ifndef UNIMPLEMENTED
    // kludge to not show fileSwitcher geometry until splash screen is taken away
    pfNode *fileSwitcher = world->find("fileSwitcher",pfNode::getClassType()) ;
    if (fileSwitcher) 
	world->removeChild(fileSwitcher) ;
#endif
  
#ifndef UNIMPLEMENTED
    // objects aren't added until splash screen goes away
    int displayed = 0 ;
#else
    int displayed = 1 ;
#endif
  
  
    if (examine) 
    {
	// be sure everything is updated
	if(DOSG::internalUpdate()) return 1; // error

	// see if shared memory already exists
	dtkSharedMem *worldPivotShm = new dtkSharedMem(3*sizeof(float),"dgl/worldPivot") ;
	// it does, but is of wrong size
	if (worldPivotShm && worldPivotShm->isValid() && (worldPivotShm->getSize()!=sizeof(float)*3)) 
	{
	    dtkMsg.add(DTKMSG_ERROR,"dosg-fly: shared "
		       "memory \"%s\" exists, but wrong size.\n", "dgl/worldPivot") ;
	    return 1;
	}
	else if (!worldPivotShm || worldPivotShm->isInvalid())
	{
	    dtkMsg.add(DTKMSG_ERROR,"dosg-fly: can't create shared "
		       "memory \"%s\"\n", "dgl/worldPivot") ;
	    return 1;
	}
	else
	{
	    dtkCoord worldPivotCoord = DOSGUtil::dgl_to_world(dtkCoord(examineCenter.x, examineCenter.y, examineCenter.z, 0.f, 0.f, 0.f)) ;
	    float worldPivot[6] ;
	    worldPivotCoord.get(worldPivot) ;
	    // need to convert worldPivot from dgl top world
	    worldPivotShm->write(worldPivot) ;
	}
      
	// check and add other DSOs
	if (!app->check("dosgWorldPivotNavMod", DTKAUGMENT_TYPE))
	    app->load("dosgWorldPivotNavMod") ;

    }

    // renumber lights to be consecutive and unique
    renumberLightVisitor lv ;
    DOSG::getScene()->accept(lv) ;

    if (verbose)
    {
	printf("scenegraph contains %d lights\n",numLights) ;
	printf("ambient = %f, diffuse = %f, specular = %f\n",ambient, diffuse, specular) ;
    }

    // main application loop
    while(app->state & DTK_ISRUNNING) 
    {
	if(app->frame()) return 1; // error
      
#ifdef UNIMPLEMENTED
	if (!displayed) 
	{
	    displayed = 1 ;
#else
	    if (!displayed && 
		!(scene->find("splashScreen",pfNode::getClassType()))) 
	    {
#endif
#ifndef UNIMPLEMENTED
		if (fileSwitcher)
		    world->addChild(fileSwitcher) ;
#endif
#ifndef UNIMPLEMENTED
		if (verbose) 
		    printf("optimizing scenegraph geoStates\n") ;
		pfdMakeSharedScene(scene) ;
		if (verbose) 
		    printf("optimization finished\n") ;
#endif
		displayed = 1 ;
	    }
	  
	}
      
	if (dumpOnExit)
	{
	    if (!osgDB::writeNodeFile(*scenen, dumpOnExitFilename))
	    {
		dtkMsg.add(DTKMSG_WARN, 
			   "Unable to dump to file \"%s\"\n", dumpOnExitFilename.c_str()) ;
	    }
	    else
	    {
		dtkMsg.add(DTKMSG_INFO, 
			   "Scene dumped to file \"%s\"\n", dumpOnExitFilename.c_str()) ;
	    }
	}
      
	return 0 ; // success
    }
