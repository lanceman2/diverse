#ifndef __DGL_H__
#define __DGL_H__
#include <vector>
#include <OpenThreads/Mutex>
using namespace std;
#include <dtk.h>
#include <dgl.h>

#define _DGL_BASE_NUM  ((u_int32_t) 2650388)
#define TYPE_DGL ((u_int32_t) _DGL_BASE_NUM*3)

class dglAugment;
class DGLDisplay;

/*!
 * @class DGL DGL-header.h dgl.h
 * @brief DGL Manager class, based on DTK's dtkManager class
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */

class DGLAPI DGL: public dtkManager
{
 public:

  /*!
   * @brief Initalize the system with default values
   */
  static void init();

  /*!
   * @brief Initalize the system, passing a list of file names containing
   * dglAugment DSOs @param argc the number of items in argv @param argv an
   * array of argc filenames
   */
  static void init(int argc, char** argv);

  /*!
   * @brief Initalize the system, passing a variable number of file names
   * containing dglAugment DSOs @param DSOFile0 the first DSO filename
   * @param ... additional DSO filenames
   */
  static void init(char* DSOFile0, ...);

  //Deconstructor for nice cleanup
  ~DGL();

  /*!
   * @brief return a pointer to the DGL object
   *
   * @return a pointer to the DGL object
   */
  static DGL* getApp();

  /*!
   * @brief return a pointer to the DGL display object
   *
   * @return a pointer to the DGL display object
  */
  static DGLDisplay* display();


  /*!
   * @brief Load dglAugment DSO files.  
   *
   * Same as dtkManager::load, except that a DSO is not loaded again if it's
   * already been loaded- this would cause a fatal error in dtkManager..
   *
   * @param file a NULL terminated array list of files.
   *
   * @param arg a pointer to data to pass to the DSO loader which in
   * turn may be passed to the dtkAugment constructor and so on.
   *
   * @return 0 on success and -1 if one DSO failed to load.
   */
  int load(const char *file, void *arg=NULL);
 
  /*!
   * @brief Set the user accesible data
   */
  static void setData(void* data);

  /*!
   * @brief Get the user accesible data
   */
  static void* getData();

  /*!
   * @brief Add a draw callback that is called before the navigation is applied
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int addPreNavAugment(dglAugment* newAugment);

  /*!
   * @brief Remove a draw callback that is called before the navigation is applied
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int removePreNavAugment(dglAugment* newAugment);

  /*!
   * @brief Add an augment
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int addAugment(dglAugment* newAugment);

  /*!
   * @brief Remove an augment
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int removeAugment(dglAugment* newAugment);

  /*!
   * @brief Add an overlay augment that is called last, so its graphics aren't occluded.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int addOverlay(dglAugment* newAugment);

  /*!
   * @brief Remove an overlay
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int removeOverlay(dglAugment* newAugment);

  /*!
   * @brief Execute one step through the run loop
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int frame();

  /*!
   * @brief Convenience method that calls DGL::preConfig, DGL::config,
   * DGL::postConfig, and DGL::frame
   */
  static void start();

  /*!
   * @brief Call DGL::frame until stopped, or (dtkManager::state &
   * dtkManager::DTK_ISRUNNING) is false
   */
  void run();

  /*!
   * @brief Config stub function for dtk
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int config();

  /*!
   * @brief Perform preConfig for augments / callback
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int preConfig();

  /*!
   * @brief Perform postConfig for augments / callback
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int postConfig();

  /*!
   * @brief Perform preframe for augments / callback
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int preFrame();

  /*!
   * @brief Draw the pre-nav draw objects
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int preNavDraw();

  /*!
   * @brief Perform draw for augments / callback
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int draw();

  /*!
   * @brief Perform postframe for augments / callback
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int postFrame();

  /*!
   * @brief Perform the init GL callback
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int initGL();

  /*!
   * @brief Perform the setup GL callback
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setupGL();

  /*!
   * @brief Switch current discrete stereo to left discrete stereo display callback.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static bool getDiscreteStereo();

  /*!
   * @brief Switch current discrete stereo to left discrete stereo display callback.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setDiscreteStereoLeft();

  /*!
   * @brief Switch current discrete stereo to right discrete stereo display callback.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setDiscreteStereoRight();

  //Public static data for DGL

  /*!
   * @brief The void pointer provided for the user callback data
   */
  static void*            m_data;

  /*!
   * @brief The post config callback pointer
   */
  static dglPostConfigCB  m_postconfig;

  /*!
   * @brief The pre config callback pointer
   */
  static dglPreConfigCB   m_preconfig;

  /*!
   * @brief The display callback
   */
  static dglDisplayCB     m_display;

  /*!
   * @brief The discrete stereo left display callback
   */
  static dglDisplayCB     m_display_discrete_left;

  /*!
   * @brief The discrete stereo right display callback
   */
  static dglDisplayCB     m_display_discrete_right;

  /*!
   * @brief spcify whether using discrete stereo or not
   */
  static bool m_discreteStereo;

  /*!
   * @brief The preFrame callback
   */
  static dglPreFrameCB    m_preframe;

  /*!
   * @brief The postFrame callback
   */
  static dglPostFrameCB   m_postframe;

  /*!
   * @brief The overlay callback
   */
  static dglOverlayCB     m_overlaycb;

  /*!
   * @brief The pre nav callback
   */
  static dglPreNavCB      m_prenavcb;

  /*!
   * @brief The setup GL callback
   */
  static dglSetupGLCB     m_setupglcb;

  /*!
   * @brief The init GL callback
   */
  static dglInitGLCB      m_initglcb;

  //Access functions

  /*!
   * @brief return a std::vector of the augments of type dglAugment::BASE
   *
   * @return a std::vector of the augments of type dglAugment::BASE
   */
  static vector<dglAugment*> getStandardAugments();

  /*!
   * @brief return a std::vector of the augments of type dglAugment::OVERLAY
   *
   * @return a std::vector of the augments of type dglAugment::OVERLAY
   */
  static vector<dglAugment*> getOverlayAugments();

  /*!
   * @brief return a std::vector of the augments of type dglAugment::PRENAV
   *
   * @return a std::vector of the augments of type dglAugment::PRENAV
   */
  static vector<dglAugment*> getPreNavAugments();

  //Control variables
  //These are used to turn on / off specific parts of DGL that
  //users may or may not want to use
  
  /*!
   * @brief enable / disable head tracking
   *
   * @param value set to true to enable head tracking, false to disable
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setHeadTracking(bool value);

  /*!
   * @brief return true if head tracking is enabled, otherwise false
   *
   * @return true if head tracking is enabled, otherwise false
   */
  static bool getHeadTracking();
  
  /*!
   * @brief enable / disable calling the setupGL callback
   *
   * @param value set to true to enable calling the setupGL callback, false to disable
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setSetupGL(bool value);

  /*!
   * @brief return true if the setupGL callback is called, otherwise false
   *
   * @return true if the setupGL callback is called, otherwise false
   */
  static bool getSetupGL();
  
  /*!
   * @brief enable / disable calling the initGL callback
   *
   * @param value set to true to enable calling the initGL callback, false to disable
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setInitGL(bool value);

  /*!
   * @brief return true if the initGL callback is called, otherwise false
   *
   * @return true if the initGL callback is called, otherwise false
   */
  static bool getInitGL();
  
  
  /*!
   * @brief enable / disable the usage of DIVERSE coordinates
   *
   * DIVERSE coordinates have Z going up and down, whereas OpenGL has Z
   * going in and out.  By default, DGL uses DIVERSE coordinates.
   *
   * @param value set to true to enable the usage of the usage of DIVERSE
   * coordinates, false to use OpenGL coordinates
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setDiverseCoord(bool value);

  /*!
   * @brief return true if DIVERSE coordinates are being used, otherwise false 
   *
   * @return true if DIVERSE coordinates are being used, otherwise false 
   */
  static bool getDiverseCoord();
  
  /*!
   * @brief Set the origin
   *
   * @param org is a dtkCoord specifying the positiif and orientation of the origin
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setOrigin(dtkCoord org);

  /*!
   * @brief return a dtkCoord specifying the positiif and orientation of the origin
   *
   * @return a dtkCoord specifying the positiif and orientation of the origin
   */
  static dtkCoord getOrigin();
  
  /*!
   * @brief Set the scale
   *
   * @param scale is the new scale value
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setScale(float scale);

  /*!
   * @brief return the scale value
   *
   * @return the scale value
   */
  static float getScale();
  
  /*!
   * @brief Set the length of a DGL unit in meters
   *
   * @param meters the new meters value
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setMeters(float meters);

  /*!
   * @brief return the meters value
   *
   * @return the meters value
   */
  static float getMeters();
  
#if 0
  //Conversion functions from world to DPF/DGL and DPF/DGL to world

  //Coordinates from DTK 2 World by John Kelso
  static int dglToWorld(dtkCoord& coord);
  
  //Coordinates from World 2 DTK by John Kelso
  static int worldToDgl(dtkCoord& coord);
  
  //Computes the ether matrix based on scale, origin, meters
  static dtkMatrix ether(void) ;
#endif
  /*!
   * @brief Create a lock for DGL objects to use.  Uses an OpenThreads Mutex
   *
   * The lock in DGL are meant to be used like this:
   * DGL::createLock("myLock");
   * DGL::lock("myLock");
   * DGL::unlock("myLock");
   * 
   * @param name is the name of the lock to create
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int createLock(const string& name);
  /*!
   * @brief Delete a lock that has been created for DGL
   *
   * @param name is the name of the lock to delete 
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int deleteLock(const string& name);

  /*!
   * @brief Lock the DGL object (using mutex threads) 
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int lock(const string& name="DEFAULT_LOCK");

  
  /*!
   * @brief Unlock the DGL object (using mutex threads) 
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int unlock(const string& name="DEFAULT_LOCK");

  /*!
   * @brief Enable/disable termination on escape key
   *
   * When enabled, pressing the escape key will cause the
   * dtkManager::IS_RUNNING bit in dtkManager::state to be cleared
   *
   * @param value is true to enable escape key termination, false to disable
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setShutdownOnEscape(bool value) ;

  /*!
   * @brief return true if termination on escape key is enabled, otherwise false
   *
   * @return true if termination on escape key is enabled, otherwise false
   */
  static bool getShutdownOnEscape() ;
  
  /*!
   * @brief Enable / disable multi threaded rendering
   *
   * Required to be called in preconfig to work properly!  The default is false.
   *
   * @param val is true to enable multi-threading
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int setMultiThreaded(bool val);

  /*!
   * @brief return true if multithreading is enabled, otherwise false
   *
   * @return true if multithreading is enabled, otherwise false
   */
  static bool getMultiThreaded();
  
  /*!
   * @brief Set the clear color.  The default is black (all zeros).
   * 
   * @param r the red component, between 0 and 1.0
   * 
   * @param g the green component, between 0 and 1.0
   * 
   * @param b the blue component, between 0 and 1.0
   * 
   * @param a the alpha component, between 0 and 1.0
   *
   * @return 0 on success, non-zero if an error occurs
   */

  static int setClearColor(const float& r, const float& g, const float& b, const float& a);
  /*!
   * @brief returns the clear color in the passed parameters
   * 
   * @param r the red component, between 0 and 1.0
   * 
   * @param g the green component, between 0 and 1.0
   * 
   * @param b the blue component, between 0 and 1.0
   * 
   * @param a the alpha component, between 0 and 1.0
   *
   * @return 0 on success, non-zero if an error occurs
   */
  static int getClearColor(float& r, float& g, float& b, float& a);
  
 private:
  /*!
   * Constructor for DGL
   */
  DGL(int argc, char** argv);
  // load a DSO by name if not already loaded
  int m_load(const char *file, void *arg) ;
  /*!
   * Has this class been initialized
   */
  static bool m_init;
  /*!
   * The list of augments to call draw on
   */
  static vector<dglAugment*> m_augments;
  /*!
   * The list of overlays to call draw on
   */
  static vector<dglAugment*> m_overlays;
  /*!
   * The list of pre-nav draw callbacks
   */
  static vector<dglAugment*> m_prenav;
  /*!
   * The pointer to myself
   */
  static DGL* m_instance;
  /*!
   * Has a display been made?
   */
  static bool m_madedisplay;
  /*!
   * A pointer to the display object
   */
  static DGLDisplay* m_dglDisplay;
  //The navigation list used in navigations
  static dtkNavList* m_navlist;
  //Do we use headtracking or not?
  static bool m_trackhead;
  //Do we rotate the world to DIVERSE coordinates or not?
  static bool m_diversecoord;
  //Do we call setup GL
  static bool m_dosetupgl;
  //Do we call init gl?
  static bool m_doinitgl;
  //The origin
  static dtkCoord m_origin;
  //The scale 
  static float    m_scale;
  //The meters value
  static float    m_meters;


  //static OpenThreads::Mutex  m_mutex;
  //Using 2 parallel vectors instead of a map
  //All of the locks
  static vector<OpenThreads::Mutex*>  m_locks;
  static vector<string>               m_lockNames;

  static bool m_shutdownonescape;
  //Are we multi threaded or not?
  static bool m_multiThreaded;
  //The clear color that the SceneHandlers use
  static float* m_clearColor;
};
#endif 
