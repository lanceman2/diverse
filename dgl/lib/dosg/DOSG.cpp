#define __ASSERTMACROS__
#include<iostream>
#include<string>
#include<vector>
#include<signal.h>
#include<dgl.h>
#ifndef DGL_ARCH_WIN32
#  include<dirent.h>
#endif /* WIN32 */

#ifndef __APPLE__
#include<GL/gl.h>
#endif 

#ifndef __sgi
#include<cstdio>
#else
#include<stdio.h>
#endif

#include<osg/Vec3>
#include<osg/Version>
#include<osgUtil/UpdateVisitor>
#include<osgDB/ReadFile>
#include<osg/Matrix>
#include <osg/Timer>
#include <osgDB/Registry>
#include <osgDB/DatabasePager>
#include<dosg.h>

using namespace std;

bool                                        DOSG::m_done        = false;
osg::PositionAttitudeTransform*             DOSG::m_scene       = NULL;
osg::PositionAttitudeTransform*             DOSG::m_ether       = NULL;
osg::PositionAttitudeTransform*             DOSG::m_world       = NULL;
osg::PositionAttitudeTransform*             DOSG::m_wand        = NULL;
dtkInLocator*                               DOSG::m_wandLoc     = NULL;
osg::PositionAttitudeTransform*             DOSG::m_head        = NULL;
dtkInLocator*                               DOSG::m_headLoc     = NULL;
osg::PositionAttitudeTransform*             DOSG::m_nav         = NULL;
osg::ClearNode*                             DOSG::m_clear       = NULL;
osg::FrameStamp*                  	    DOSG::m_frameStamp  = NULL;
int                                         DOSG::m_frameNumber = 0 ;
osg::Timer_t                                DOSG::m_thisTick;
osg::Timer_t                                DOSG::m_lastTick;
double                                      DOSG::m_deltaTick;
double                                      DOSG::m_thisRefTime = 0;
double                                      DOSG::m_thisSimTime = 0;
double                                      DOSG::m_timeWarp = 1;
bool                                        DOSG::m_autoNearFar = false ;

DOSG*				  	    DOSG::m_app         = NULL;
vector<osgUtil::SceneView*>                 DOSG::m_sceneview;
osg::LightModel*                            DOSG::m_lightModel = NULL;
vector<DGLWindow*>       		    DOSG::m_windows;;
int				            DOSG::m_numScenes   =0;
float                                       DOSG::m_scale = 1.f ;
dtkCoord                                    DOSG::m_origin ;
osgDB::DatabasePager*   		    DOSG::m_dp; 
osgUtil::UpdateVisitor*			    DOSG::m_updateVisitor=NULL;
bool					    DOSG::m_update = true;

//=============================================================================
// Function Name: internalUpdate
/// @brief Updates the head / wand / navigation info for DOSG.
//=============================================================================
int DOSG::internalUpdate()
{
   {
     // set the clear node's color based on what dgl uses
     static float r, g, b, a ;
     static bool once=true;
     //Only set the clear color once
     if (once)
     {
	 dtkMsg.add(DTKMSG_INFO,
		    "DOSG::internalUpdate: setting clear node color to (%f, %f, %f, %f)\n",r,g,b,a) ;
	 DGL::getApp()->getClearColor(r, g, b, a) ;
	 m_clear->setClearColor(osg::Vec4(r, g, b, a)) ;
	 once=false;
     }

     // set the ether and world matrices
     // world is always just a copy of ether- 
     //it's the nav node above it that makes the difference!
     setEtherNode() ;
     
     static float wand_xyzhpr[6] ;
     static float old_wand_xyzhpr[6] ;
     // set the wand node's transform if the wand exists
     if (!m_wandLoc) 
       {
	 m_wandLoc = (dtkInLocator*)DGL::getApp()->check("wand") ;
	 if (!m_wandLoc || m_wandLoc->isInvalid())
	   m_wandLoc = NULL ;
       }

     static bool UpdateWand=false;
     if (m_wandLoc) 
       {
	 m_wandLoc->read(wand_xyzhpr) ;
	 if (memcmp(wand_xyzhpr, old_wand_xyzhpr, sizeof(float)*3))
	   {
	     m_wand->setPosition(osg::Vec3(wand_xyzhpr[0],wand_xyzhpr[1],wand_xyzhpr[2])) ;
	     UpdateWand=true;
	     //printf("setting wand position to %f %f %f\n",wand_xyzhpr[0],wand_xyzhpr[1],wand_xyzhpr[2]);
	   }
	 if (memcmp(wand_xyzhpr+3, old_wand_xyzhpr+3, sizeof(float)*3))
	   {
	     osg::Quat q = osg::Quat(DOSGUtil::euler_to_quat(wand_xyzhpr[3],
							     wand_xyzhpr[4],
							     wand_xyzhpr[5])) ;
	     
	     m_wand->setAttitude(q) ;
	     UpdateWand=true;
	     //printf("setting wand angle to %f %f %f\n",wand_xyzhpr[3],wand_xyzhpr[4],wand_xyzhpr[5]);
	   }
	 
	 if (UpdateWand)
	 {
		 memcpy(old_wand_xyzhpr, wand_xyzhpr, sizeof(float)*6) ;
		 UpdateWand=false;
	 }
       }
     
     static float head_xyzhpr[6] ;
     static float old_head_xyzhpr[6] ;
     static bool UpdateHead=false;;
     // set the head node's transform if the head exists
     if (!m_headLoc) 
       {
	 m_headLoc = (dtkInLocator*)DGL::getApp()->check("head") ;
	 if (!m_headLoc || m_headLoc->isInvalid())
	   m_headLoc = NULL ;
       }

     if (m_headLoc) 
       {
	 m_headLoc->read(head_xyzhpr) ;
	 if (memcmp(head_xyzhpr, old_head_xyzhpr, sizeof(float)*3))
	   {
	     m_head->setPosition(osg::Vec3(head_xyzhpr[0],head_xyzhpr[1],head_xyzhpr[2])) ;
	     UpdateHead=true;
	     //printf("setting head position to %f %f %f\n",head_xyzhpr[0],head_xyzhpr[1],head_xyzhpr[2]);
	   }
	 if (memcmp(head_xyzhpr+3, old_head_xyzhpr+3, sizeof(float)*3))
	   {
	     osg::Quat q = osg::Quat(DOSGUtil::euler_to_quat(head_xyzhpr[3],
							     head_xyzhpr[4],
							     head_xyzhpr[5])) ;
	     
	     m_head->setAttitude(q) ;
	     UpdateHead=true;
	     //printf("setting head angle to %f %f %f\n",head_xyzhpr[3],head_xyzhpr[4],head_xyzhpr[5]);
	   }
	 
	     if (UpdateHead)
	     {
		 memcpy(old_head_xyzhpr, head_xyzhpr, sizeof(float)*6) ;
		 UpdateHead=false;
	     }
       }
     
     // get the list of navigations
     static dtkNavList* navList = NULL;
     static dtkCoord oldl;
     static float oldScale=0;
     if (!navList)
     {
       navList = (dtkNavList*) DGL::getApp()->check("dtkNavList", DTKNAVLIST_TYPE);
       oldl.zero();
     }
     
     static dtkNav*  nav = NULL;
     // get the currently selected navigation
     if (navList)
       nav = navList->current();
     
     if (nav != NULL)
       {
	 // get the current navigational offset
	 dtkCoord l = nav->location ;
	 float s = nav->scale ;
	 if (oldScale != s)
	 {
		 m_nav->setScale(osg::Vec3(s, s, s)) ;
		 oldScale = s;
	 }
	 
	 // set the nav node
	 if (! l.equals(&oldl))
	 {
		 m_nav->setPosition(osg::Vec3(l.x, l.y, l.z)) ;
		 m_nav->setAttitude(DOSGUtil::euler_to_quat(l.h, l.p, l.r)) ;
		 oldl = l;
	 }
       }
     else
       {
	 m_nav->setScale(osg::Vec3(1.f, 1.f, 1.f)) ;
	 m_nav->setPosition(osg::Vec3(0.f, 0.f, 0.f)) ;
	 m_nav->setAttitude(osg::Quat(0.f, 0.f, 0.f, 1.f)) ;
       }
   }
    
   //what time is it now?
   m_thisTick = osg::Timer::instance()->tick();

   // how long since last time?
   m_deltaTick = osg::Timer::instance()->delta_s(m_lastTick, m_thisTick) ;

   #if OSG_VERSION_MAJOR < 2
   {
       // ref time is warped sum of differences
       m_thisRefTime += m_timeWarp * m_deltaTick;
   }
   #else
   {
       m_thisRefTime += m_deltaTick;
       // sim time is warped sum of differences
       m_thisSimTime += m_timeWarp * m_deltaTick;
       m_frameStamp->setSimulationTime(m_thisSimTime);
   }
   #endif

   m_frameStamp->setReferenceTime(m_thisRefTime);

   //ready for next frame+
   m_lastTick = m_thisTick ;

   m_frameStamp->setFrameNumber(m_frameNumber++);

//Determine if culling is necessary
   bool cullPlanes = dosgClipCullNode::howManyNodes()>0 && 
     (dosgClipCullNode::cullType() !=  dosgClipCullNode::OFF) ;
   if (cullPlanes)
     dosgClipCullNode::resetNodemasks() ;

   m_updateVisitor->reset();
   m_updateVisitor->setFrameStamp(m_frameStamp);
   m_updateVisitor->setTraversalNumber(m_frameStamp->getFrameNumber());
   m_scene->accept(*m_updateVisitor);
   m_scene->getBound();

   //Do the culling after the update
   if (cullPlanes)
   {
     dosgClipCullPlaneVisitor ccpv;
     m_scene->accept(ccpv);
   }

   return 0;
}
//=============================================================================
// Function Name: init
/// @brief Creates the one and only DOSG object.
//=============================================================================
 void DOSG::init(char* name)
 {
     // Make sure this function is only called once
     static bool initialized = false;
     if( initialized ) return;
     initialized = true;

     DGL::getApp()->init() ;

     m_app = new DOSG(name);
     DGL::createLock("DOSG_UPDATE");
 }


 //=============================================================================
 /// @brief Draw the scene.
 //=============================================================================
 int DOSG::draw( )
 {

	 //Check to see if we need to update our internal state
	 //Includes position of the head node, etc...
	 if (m_update)
	 {
		 //This is done b/c of multiple draw threads 
		 DGL::lock("DOSG_UPDATE");
		 //The idea is only the first thread will be able to call 
		 //this function.
		 if (m_update)
		 {
			 internalUpdate();
			 m_update = false;
		 }
		 DGL::unlock("DOSG_UPDATE");
	 }
	 int value=-1;
	 Producer::Camera* cam;
	 if (DGL::getApp()->getMultiThreaded())
	 {
		 OpenThreads::Thread* currThread = OpenThreads::Thread::CurrentThread();
		 if (currThread !=NULL)
		 {
			Producer::Camera* cThread = (Producer::Camera*)currThread;
			cam = cThread;
			if (cThread == NULL)
			{
				printf("Error in DOSG - Wrong type of threads being used\n");
			}
			DGLWindow* win = DGL::getApp()->display()->getWFromR(cThread->getRenderSurface());
			DGLCameraThread* temp = (DGLCameraThread*)OpenThreads::Thread::CurrentThread();
			win = temp->getWindow();
			if (win !=NULL)
			{
				for (unsigned int i=0; i<m_windows.size(); i++)
				{
					if (m_windows[i] == win)
					{
						value=i;
					}
				}
			}
		 }
	 }
	 else
	 {
		DGLWindow* win = DGL::getApp()->display()->getWFromR( (DGL::getApp()->display()->getCamera()->getRenderSurface()));
		cam = (DGL::getApp()->display()->getCamera());
		for (unsigned int i=0; i<m_windows.size(); i++)
		{
			if (m_windows[i] == win)
			{
				value=i;
			}
		}
	}
	if (value == -1 || (unsigned int)value > m_sceneview.size())
	{
		printf("Error in DOSG:  SceneView number is not set correctly\n");
		return value;
	}

	 //Get information from OpenGL about its state
	 int vp[4];
	 glGetIntegerv(GL_VIEWPORT, (GLint*)vp);
	 float tempProject[16];
	 glGetFloatv(GL_PROJECTION_MATRIX, tempProject);
	 float tempModel[16];
	 glGetFloatv(GL_MODELVIEW_MATRIX, tempModel);

	 //Put this information into open scenegraph
	 m_sceneview[value]->setViewport(vp[0], vp[1], vp[2], vp[3]);
	 m_sceneview[value]->getProjectionMatrix().set(osg::Matrix(tempProject));
	 m_sceneview[value]->getViewMatrix().set(osg::Matrix(tempModel));
	 //m_sceneview[value]->update();
	 m_sceneview[value]->cull();
	 m_sceneview[value]->draw();

	 // add the data viewport so shaders can use it
	 osg::StateSet *localStateSet = m_sceneview[value]->getLocalStateSet();
	 osg::Uniform *uniform = localStateSet->getOrCreateUniform("dosg_Viewport", osg::Uniform::FLOAT_VEC4);
	 uniform->set(osg::Vec4f(vp[0],vp[1],vp[2],vp[3]));

	 if (m_autoNearFar)
	     m_sceneview[value]->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES) ;
	 else
	     m_sceneview[value]->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR) ;

     return DTK_CONTINUE;
}

 //=============================================================================
 // Function Name: initSceneView
 /// @brief For each context we create a new scene view and pass it the root
 //=============================================================================
int DOSG::initGL()
{
	 if (!_ds) _ds = osg::DisplaySettings::instance();

	 DGLWindow* win = NULL;
	 if (DGL::getApp()->getMultiThreaded())
	 {
		 OpenThreads::Thread* currThread = OpenThreads::Thread::CurrentThread();
		 if (currThread !=NULL)
		 {
			Producer::Camera* cThread = (Producer::Camera*)currThread;
			if (cThread == NULL)
			{
				printf("Error in DOSG - Wrong type of threads being used\n");
			}
			win = DGL::getApp()->display()->getWFromR(cThread->getRenderSurface());
			DGLCameraThread* temp = (DGLCameraThread*)OpenThreads::Thread::CurrentThread();
			win = temp->getWindow();
		 }
	 }
	 else
	 {
		win = DGL::getApp()->display()->getWFromR( (DGL::getApp()->display()->getCamera()->getRenderSurface()));
	 }
	 if (win == NULL)
	 {
		printf("Error:  could not get the DGLWindow that is calling DOSG::init, expect program to crash.\n");
	 }
	 else
		m_windows.push_back(win);

	 osgUtil::SceneView* tempScene = new osgUtil::SceneView (osg::DisplaySettings::instance());
         //tempScene->setDefaults(0);
	 tempScene->setDefaults( osgUtil::SceneView::STANDARD_SETTINGS );
	 //tempScene->setDefaults(osgUtil::SceneView::COMPILE_GLOBJECTS_AT_INIT) ;
	 m_sceneview.push_back(tempScene);

	 osg::GraphicsContext::incrementContextIDUsageCount(m_numScenes);
	 m_numScenes = osg::GraphicsContext::createNewContextID();
	 tempScene->init();
	 tempScene->setDefaults( osgUtil::SceneView::STANDARD_SETTINGS );
	 tempScene->getState()->setContextID(m_numScenes);
	 tempScene->setSceneData( m_scene );
	 tempScene->setDrawBufferValue(GL_NONE);
         tempScene->setFrameStamp( m_frameStamp );

	 // switch off small feature culling to prevent points from being culled.
	 tempScene->setCullingMode( tempScene->getCullingMode() & ~osg::CullStack::SMALL_FEATURE_CULLING);

	 // add some extra modes not set by default
	 tempScene->getGlobalStateSet()->setGlobalDefaults() ;

	 // resize normals when resizing geometry
	 tempScene->getGlobalStateSet()->setMode((osg::StateAttribute::GLMode)GL_NORMALIZE, osg::StateAttribute::ON);

	 // get correct specular lighting across pipes
	 // see http://www.ds.arch.tue.nl/General/Staff/Joran/osg/osg_specular_problem.htm
	 m_lightModel = new osg::LightModel ;
	 m_lightModel->setLocalViewer(true);
	 tempScene->getGlobalStateSet()->setAttribute(m_lightModel);

	 return 0;
}

 //=============================================================================
 // Function Name: DOSG
 /// @brief Default constructor, calls all config functions.
 //=============================================================================
DOSG::DOSG(char* name)
     :dglAugment(name, PRENAV)
{
	//Is this the magic multi threading fix?
	if (DGL::getApp()->getMultiThreaded())
		osg::Referenced::setThreadSafeReferenceCounting(true);
     m_scene  = new osg::PositionAttitudeTransform;

     m_dp = new osgDB::DatabasePager;
	if (DGL::getApp()->getMultiThreaded())
	     m_scene->setThreadSafeRefUnref(true);

	//End code ripped from the osg viewer.
     m_ether = new osg::PositionAttitudeTransform;
     m_world = new osg::PositionAttitudeTransform;
     m_wand  = new osg::PositionAttitudeTransform;
     m_head  = new osg::PositionAttitudeTransform;
     m_nav   = new osg::PositionAttitudeTransform;
     m_clear = new osg::ClearNode;
     m_scene->setName("scene") ;
     m_ether->setName("ether") ;
     m_world->setName("world") ;
     m_wand->setName("wand") ;
     m_head->setName("head") ;
     m_nav->setName("nav") ;
     m_clear->setName("clear") ;
     m_scene->addChild( m_ether );
     m_scene->addChild( m_wand );
     m_scene->addChild( m_head );
     m_scene->addChild( m_nav );
     m_nav->addChild( m_world );
     m_scene->addChild( m_clear );
     DOSGUtil::nodeList::add(m_scene->getName(), m_scene) ;
     DOSGUtil::nodeList::add(m_ether->getName(), m_ether) ;
     DOSGUtil::nodeList::add(m_world->getName(), m_world) ;
     DOSGUtil::nodeList::add(m_wand->getName(), m_wand) ;
     DOSGUtil::nodeList::add(m_head->getName(), m_head) ;
     DOSGUtil::nodeList::add(m_nav->getName(), m_nav) ;
     DOSGUtil::nodeList::add(m_clear->getName(), m_clear) ;

     m_frameStamp = new osg::FrameStamp;
     m_frameNumber=0;
     m_thisTick = osg::Timer::instance()->tick();
     m_lastTick = m_thisTick;

     m_updateVisitor = new osgUtil::UpdateVisitor;
     m_updateVisitor->setFrameStamp(m_frameStamp);

}

 //=============================================================================
 // Function Name: config
 /// @brief Runs preConfig, config, and postConfig for all augments and calls
 /// Init if DOSG hasn't been initialized
 //=============================================================================
int DOSG::preConfig()
{
  
  return DTK_CONTINUE;

}

 //=============================================================================
 // Function Name: postConfig 
 /// @brief Runs preConfig, config, and postConfig for all augments and calls
 /// Init if DOSG hasn't been initialized.
 //=============================================================================
int DOSG::postConfig()
{
  
  return DTK_CONTINUE;
 }


 //=============================================================================
 // Function  Name: postFrame
 /// @brief This function does all of the low-latency-required actions between 
 /// frames
 //=============================================================================

int DOSG::preFrame()
{
   m_update = true;
   return DTK_CONTINUE;
 }

 //=============================================================================
 // Function  Name: postFrame
 /// @brief This function does all of the "heavy lifting" between frames and is
 /// where all code that may make changes is called.
 //=============================================================================
 int DOSG::postFrame()
 {
   return DTK_CONTINUE;
 }

 //=============================================================================
 // Function  Name: setEtherNode
 /// @brief sets the ether node in the scenegraph based on scale, origin values
 //=============================================================================
void DOSG::setEtherNode(void) 
{
  float s = DGL::getApp()->getScale() ;
  if (s!=m_scale)
    {
      m_ether->setScale(osg::Vec3(s, s, s)) ;
      m_world->setScale(m_ether->getScale()) ;
      m_scale = s ;
    }
  dtkCoord o = DGL::getApp()->getOrigin() ;
	dtkMatrix m = dtkMatrix( o );
	m.invert();
	m.coord( &o );
  if (!m_origin.equals(&o))
    {
      m_ether->setAttitude(DOSGUtil::euler_to_quat(o.h, o.p, o.r)) ;
      m_ether->setPosition(osg::Vec3(o.x, o.y, o.z)) ;
      m_world->setAttitude(m_ether->getAttitude()) ;
      m_world->setPosition(m_ether->getPosition()) ;
      m_origin = o ;
    }
}
 
