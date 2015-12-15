#ifndef _dgl_display_h_
#define _dgl_display_h_


#ifdef OLDPRODUCER
#include <Producer/Camera>
#else
#include <Producer/Camera.h>
#endif

#include <dtk.h>

using namespace Producer;
class DGLPipe;
class DGLWindow;
class DGLViewport;

/*!
 * @class DGLDisplay DGLDisplay.h dgl.h
 *
 * @brief DGL Display class, based on DTK's dtkDisplay class
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */

class DGLAPI DGLDisplay : public dtkDisplay
{
 public:
  //Constructors / Destructors

  /*!
   * @param app_in a pointer to the DGL object
   *
   * @param name optional, passed to dtkDisplay constructor
   */
  DGLDisplay(DGL* app_in, const char* name="DGLDisplay");
  
  // plain ole destructor
  virtual ~DGLDisplay();

  /*!
   * @brief preConfig callback, currently a no-op.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int preConfig();

  /*!
   * @brief config callback- creates windows, viewports, screens 
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int config();

  /*!
   * @brief postConfig callback, currently a no-op.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int postConfig();

  /*!
   * @brief preFrame callback, currently a no-op.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int preFrame();

  /*!
   * @brief frame callback
   *
   * Calls DGL::setSetupGL callbacks, DGL::initGL() callbacks the first time
   * it's called.  Clears, draws and swaps buffers for all display surfaces.
   * Updates keyboard/mouse events.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int frame();

  /*!
   * @brief sync callback, currently a no-op.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int sync();

  /*!
   * @brief Return the DGLWindow containing a render surface
   *
   * @param rs a pointer to a Producer::RenderSurface
   *
   * @return The DGL window containing a render surface
   */
  virtual DGLWindow* getWFromR(Producer::RenderSurface* rs);
  
  /*!
   * @brief Add a pipe (graphics card) to the display
   *
   * @return the pipe's number, which can be used in DGLDisplay::getPipe
   */
  virtual int addPipe(void);

  /*!
   * @brief Add a pipe (graphics card) to the display
   *
   * @param displayNumber the display number of the pipe (1.0 for example).
   *
   * @param screenNumber the screen number inside of the pipe (0.1 for example).
   *
   * @return the pipe's number, which can be used in DGLDisplay::getPipe
   */
  virtual int addDisplayScreenPipe(unsigned int displayNumber, unsigned int screenNumber);

  /*!
   * @brief Return a pointer to a DGLPipe object
   *
   * @param displayNumber the display number of the pipe (1.0 for example).
   *
   * @param screenNumber the screen number inside of the pipe (0.1 for example).
   *
   * @return a pointer to a DGLPipe object
   */
  virtual DGLPipe* getDisplayScreenPipe(unsigned int displayNumber, unsigned int screenNumber);

  /*!
   * @brief Add a pipe (graphics card) to the display
   *
   * @param display - The display environment variable syntax function i.e. [localhost]:0[.1]
   *
   * @return the pipe's number, which can be used in DGLDisplay::getPipe
   */
  virtual int addEnvPipe(char* display);

  /*!
   * @brief Return a pointer to a DGLPipe object
   *
   * @param pipeNumber the pipe number as specified in DGLDisplay::addPipe
   *
   * @return a pointer to a DGLPipe object
   */
  virtual DGLPipe* getPipe(unsigned int pipeNumber);

  /*!
   * @brief Return a pointer to a DGLPipe object
   *
   * @param display - The display environment variable syntax function i.e. [localhost]:0[.1]
   *
   * @return a pointer to a DGLPipe object
   */
  virtual DGLPipe* getEnvPipe(const char* display);

  /*!
   * @brief Return the number of pipes
   *
   * @return the number of pipes
   */
  virtual unsigned int getNumPipes();

  /*!
   * @brief Return the number of Producer::Camera objects
   *
   * Accessor function for the cameras that are stored in this class-
   * provided so that DGL doesn't have to go through about 6 extra memory
   * jumps each frame
   *
   * @return the number of Producer::Camera objects
   */
  virtual unsigned int getNumCameras() 
    { return m_cameras.size();};

  /*!
   * @brief Add a Producer::Camera to the std::vector of Producer::Camera objects
   *
   * Accessor function for the cameras that are stored in this class-
   * provided so that DGL doesn't have to go through about 6 extra memory
   * jumps each frame
   *
   * @param cam a pointer to a Producer::Camera object
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int addCamera(Camera* cam) 
    { if (cam == NULL) return 1; else m_cameras.push_back(cam); return 0;};
  
  /*!
   * @brief Return a pointer to the specified Producer::Camera
   *
   * Accessor function for the cameras that are stored in this class-
   * provided so that DGL doesn't have to go through about 6 extra memory
   * jumps each frame
   *
   * @param num an index into the std::vector of Producer::Camera objects
   *
   * @return a pointer to the specified Producer::Camera
   */
  virtual Camera* getCamera(unsigned int num) 
    { if (num >= m_cameras.size()) return NULL; return m_cameras[num];};
  
  /*!
   * @brief Remove a Producer::Camera from the std::vector of Producer::Camera objects
   *
   * Accessor function for the cameras that are stored in this class-
   * provided so that DGL doesn't have to go through about 6 extra memory
   * jumps each frame
   *
   * @param num an index into the std::vector of Producer::Camera objects
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int removeCamera(unsigned int num) 
    {if (num >= m_cameras.size()) return 1; m_cameras.erase(m_cameras.begin() + num); return 0;};
  
  /*!
   * @brief Return the number of DGLWindow objects
   *
   * Accessor function for the windows that are stored in this class-
   * provided so that DGL doesn't have to go through about 6 extra memory
   * jumps each frame
   *
   * @return the number of DGLWindow objects
   */
  virtual unsigned int getNumWindows(){ return m_windows.size();};

  /*!
   * @brief Add a DGLWindow object to the std::vector of DGLWindow objects 
   *
   * Accessor function for the windowss that are stored in this class-
   * provided so that DGL doesn't have to go through about 6 extra memory
   * jumps each frame
   *
   * @param winder a pointer to a DGLWindow object
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int addWindow(DGLWindow* winder)
    { m_windows.push_back(winder); return 0;};

  /*!
   * @brief Return a pointer to the specified DGLWindow
   *
   * Accessor function for the windows that are stored in this class-
   * provided so that DGL doesn't have to go through about 6 extra memory
   * jumps each frame
   *
   * @param num an index into the std::vector of DGLWindow objects
   *
   * @return a pointer to the specified DGLWindow
   */
  virtual DGLWindow* getWindow(unsigned int num)
    { if (num >= m_windows.size()) return NULL; return m_windows[num];};

  /*!
   * @brief Remove a DGLWindow from the std::vector of DGLWindow objects 
   *
   * Accessor function for the windows that are stored in this class-
   * provided so that DGL doesn't have to go through about 6 extra memory
   * jumps each frame
   *
   * @param num an index into the std::vector of DGLWindow objects
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int removeWindow(unsigned int num) 
    {if (num >= m_windows.size()) return 1; m_windows.erase(m_windows.begin() + num); return 0;};

  /*!
   * @brief Return the Producer::Camera that is being drawn 
   *
   * @return the Producer::Camera that is being drawn
   */
  virtual Camera* getCamera();

  /*!
   * 
   * @brief Set the current Producer::Camera
   *
   * Never call this function unless you have written your own scenehandler
   * and then you need to call this function for single threaded drawing to work right
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setCamera(Camera* cam) { m_currentCam = cam; return 0;};

  /*!
   * @brief Return the designated "debug" DGLviewport.
   * 
   * Use this for whatever you want.  A handy way to specify where debug or
   * other informational messages or graphics should be displayed.  Defaults
   * to the first viewport of the first window of the first pipe created.
   *
   * @return the designated "debug" DGLviewport
   */
  virtual DGLViewport* getDebugViewport(void) 
    { return m_debug_viewport ; } ;

  /*!
   * @brief Set the designated "debug" DGLviewport.
   * 
   * Use this for whatever you want.  A handy way to specify where debug or
   * other informational messages or graphics should be displayed.  Defaults
   * to the first viewport of the first window of the first pipe created.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setDebugViewport(DGLViewport *v) 
    { m_debug_viewport = v ; return 0; } ;

  /*!
   * @brief Add a DGLCameraThread to the display
   * 
   * @param cam a DGLCameraThread to be added to the display
   */
  virtual void addCameraThread(DGLCameraThread* cam) {m_tcams.push_back(cam);};
  
  /*!
   * @brief Clear the DGLCameraThread std::vector
   */
  virtual void clearCameraThreads() {m_tcams.erase(m_tcams.begin(), m_tcams.end());};

  /*!
   * @brief Return the DGLCameraThread std::vector
   * 
   * @return the DGLCameraThread std::vector
   */
  virtual vector<DGLCameraThread*> getCameraThreads() { return m_tcams;};
  
  
 private:
  //Pointer to the master dgl object
  DGL* m_dgl;
  //The pipes in the display
  vector<DGLPipe*> m_pipes;
  //My name
  char* m_name;
  //The cameras stored here for ease of drawing
  vector<Camera*> m_cameras;
  vector<DGLCameraThread*> m_tcams;
  vector<int> m_masterCamList;
  //The current camrea that is being used for drawing
  Camera* m_currentCam;
  //The windows stored here for ease of updating
  vector<DGLWindow*> m_windows;
  // The default or user-specified debug viewport
  // The user can use this as a hint as to what viewport to use for heads up text, simulator jump mode, etc.
  DGLViewport *m_debug_viewport ;
};
#endif
