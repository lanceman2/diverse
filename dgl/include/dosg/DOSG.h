#ifndef _DOSG_H_
#define _DOSG_H_
#include<vector>
//#include<osgProducer/OsgSceneHandler>
#include<osg/PositionAttitudeTransform>
#include<osgUtil/SceneView>
#include<osg/LightModel>
#include<osg/FrameStamp>
//#include <osgProducer/OsgCameraGroup>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventHandler>
#include <osgGA/EventVisitor>
#include <osgUtil/UpdateVisitor>
#include <dgl.h>

using namespace std;

#ifdef DGL_ARCH_WIN32
	#define GETCURRENTCONTEXT wglGetCurrentContext
#else
	#define GETCURRENTCONTEXT glXGetCurrentContext
#endif

#ifdef DGL_ARCH_WIN32
	#define DGLCONTEXT HGLRC
#else
	#define DGLCONTEXT GLXContext
#endif
/*!
 * @class DOSG DOSG.h dosg.h
 *
 * @brief The DIVERSE interface to Open Scene Graph
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author Daniel Larimer, dlarimer@opentechinc.com
 * @author John Kelso, kelso@nist.gov
 */
class DOSGAPI DOSG : public dglAugment
{
 public:
  /*!
   * @brief Create and inititialze a new DGL object
   *
   * Creates scenegraph
   *
   * Only one DGL object can exist per program- a "singleton"
   *
   * @param name passed to dglAugment
   */
  static void init(char* name="DOSG_INTERFACE");
  static int internalUpdate();
        
  /*!
   * @brief preConfig callback, currently a no-op
   *
   * @return zero on success, non-zero if an error occurred
   */
  virtual int preConfig();

  /*!
   * @brief postConfig callback
   *
   * Sets lighting parameters
   *
   * @return zero on success, non-zero if an error occurred
   */
  virtual int postConfig();

  /* commented out jtk 7/06
  virtual void cull ( );
  */

  /*!
   * @brief Draw the scene
   *
   * @return zero on success, non-zero if an error occurred
   */
  virtual int draw ();

  /*!
   * @brief Create a new scene view and pass it the root
   *
   * Call once for each OpenGL context
   * @return zero on success, non-zero if an error occurred
   */
  virtual int initGL();
        
  //====== RUN LOOP======//
  
  /*!
   * @brief Perform all of the low-latency-required actions between frames
   *
   * Put as little code in preFrame callbacks as possible
   *
   * @return zero on success, non-zero if an error occurred
   */
    virtual int preFrame(); // low latency required

  /*!
   * @brief Perform all of the code that does "heavy lifting" between frames 
   *
   * Put as much code in postFrame callbacks as possible (but as little as
   * it takes to get the job done!)
   *
   * @return zero on success, non-zero if an error occurred
   */
    virtual int postFrame(); // high latency OK 
        
    /*!
     * @brief Return the "root" node of the scene graph
     *
     * @return the "root" node of the scene graph
     */
    static osg::Transform* getScene()   { return m_scene;  }

    /*!
     * @brief Return the "ether" node of the scene graph
     *
     * @return the "ether" node of the scene graph
     */
    static osg::Transform* getEther() { return m_ether; }

    /*!
     * @brief Return the "world" node of the scene graph
     *
     * @return the "world" node of the scene graph
     */
    static osg::Transform* getWorld()  { return m_world; }

    /*!
     * @brief Return the "wand" node of the scene graph
     *
     * @return the "wand" node of the scene graph
     */
    static osg::Transform* getWand()  { return m_wand; }

    /*!
     * @brief Return the "head" node of the scene graph
     *
     * @return the "head" node of the scene graph
     */
    static osg::Transform* getHead()  { return m_head; }

    /*!
     * @brief Return the "nav" node of the scene graph
     *
     * @return the "nav" node of the scene graph
     */
    static osg::Transform* getNav()  { return m_nav; }

    /*!
     * @brief Return the "clear" node of the scene graph
     *
     * @return the "clear" node of the scene graph
     */
    static osg::ClearNode* getClearNode()  { return m_clear; }
    
    /*!
     * @brief Return the current frame number
     *
     * @return the current frame number
     */
    static int getFrameNumber() { return m_frameNumber; }
    
    /*!
     * @brief Return the current reference time
     *
     * @return the current refewence time
     */
    static double getReferenceTime() { return m_thisRefTime; }
    
    /*!
     * @brief Return the current simulation time
     *
     * @return the current simulation time
     */
    static double getSimulationTime() { return m_thisSimTime; }
    
    /*!
     * @brief Return the "time warp" of the graphics clock
     *
     * @return the "time warp" of the graphics clock 
     */
    static double getTimeWarp()  { return m_timeWarp; }
    
    /*!
     * @brief Set the "time warp" of the graphics clock
     *
     * @param t is the time warp factor.  If omitted, it is reset to 1.0
     *
     */
    static void setTimeWarp(double t=0.0)  { m_timeWarp = t; }

    /*!
     * @brief Reset the "time warp" of the graphics clock to its initial value
     *
     */
    static void resetTimeWarp(void)  { m_timeWarp = 1.0; }
    
    /*!
     * @brief Return the current value of "autoNearFar"
     *
     * @return the current value of "autoNearFar"
     */
    static bool getAutoNearFar()  { return m_autoNearFar; }
    
    /*!
     * @brief Set the value of "autoNearFar".  True means that the near and
     * far clipping planes will automatically be calculated based on the
     * bounding sphere of the scenegraph.  False means that the values set
     * in DGLScreen will be used for the nar and far clipping planes.
     *
     * @param v is the value of "autoNearFar"
     *
     */
    static void setAutoNearFar(bool v)  { m_autoNearFar = v; }

 private:
    DOSG(char* name);
    
    static void setEtherNode(void) ;
    
    //static map<DGLCONTEXT,osgUtil::SceneView*> myMap;
    static DOSG* m_app;
    static bool m_done;
    static osg::FrameStamp*                          m_frameStamp;
    static int                                       m_frameNumber;
    // store scale, origin, so don't have to recalc if not changed
    static                                           float m_scale ;
    static dtkCoord                                  m_origin ;
    static osg::PositionAttitudeTransform*           m_scene;
    static osg::PositionAttitudeTransform*           m_ether;
    static osg::PositionAttitudeTransform*           m_world;
    static osg::PositionAttitudeTransform*           m_wand;
    static dtkInLocator*                             m_wandLoc;
    static osg::PositionAttitudeTransform*           m_head;
    static dtkInLocator*                             m_headLoc;
    static osg::PositionAttitudeTransform*           m_nav;
    static osg::ClearNode*                           m_clear ;
    static vector<osgUtil::SceneView*>               m_sceneview;
    static osg::LightModel*                          m_lightModel;
    static int                                       m_numScenes;
    // frameStamp times of this frame and last frame, and diff
    static osg::Timer_t                              m_thisTick;
    static osg::Timer_t                              m_lastTick;
    static double                                    m_deltaTick ;
    // ref time- sum since simulation started, this frame, last frame
    static double                                    m_thisRefTime;
    // sim time- sum since simulation started, this frame, last frame
    static double                                    m_thisSimTime;
    // multiply by time since last frame to warp simulation time
    // let's do the time warp again!
    static double                                    m_timeWarp;
    static bool                                      m_autoNearFar;
    //Database pager to signal the start and end of the drawing
    static osgDB::DatabasePager*                     m_dp;
    //Do we need to update our internal state
    static bool					     m_update;
    

    //The windows used to draw in, used to keep track of which sceneview 
    //object is used to draw which frame
    static vector<DGLWindow*>                        m_windows;
    //static vector<osgProducer::OsgSceneHandler*>     m_shandler;
    
    //The OSG specific timing variables (necessary for PRISM)
    osg::ref_ptr<osg::NodeVisitor>      _updateVisitor;
    osg::ref_ptr<osg::StateSet>          _global_stateset;
    osg::ref_ptr<osg::DisplaySettings>   _ds;
    osg::CullSettings                    _cullSettings;
    osg::ref_ptr<osgGA::EventVisitor>   _eventVisitor;
    //The update visitor that traverses the scenegraph once every draw
    static osgUtil::UpdateVisitor*      m_updateVisitor;
};
#endif
