#ifndef _DProducerInputCallback_h_
#define _DProducerInputCallback_h_

#ifdef OLDPRODUCER
#include <Producer/KeyboardMouse>
#else
#include <Producer/KeyboardMouse.h>
#endif

#include <dgl.h>

using namespace Producer;

class dtkManager;
class dtkInButton;
class dtkInValuator;

/*!
 * @class DMKCallback DProducerInputCallback.h dgl.h
 *
 * @brief Push events from producer render surfaces to
 * the DIVERSE event system.
 *
 * The render surface and the KeyboardMouse object for this class must be
 * set in order to function properly.
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DGLAPI DMKCallback : public KeyboardMouseCallback
{
 public:
  //Set functions because I need to be able
  //To compute pixel coords for the system

  /*! 
   * @brief Set the Producer::RenderSurface which will be used for this
   * object
   *
   * This must be set for this class to work
   *
   * @param rs the Producer::RenderSurface which will be used for this
   * object
   */
  void setRS(RenderSurface* rs);

  //Standard callback functions
  /*!
   * @brief mouseScroll callback, currently a no-op
   */
  virtual void mouseScroll( ScrollingMotion ){};

  /*!
   * @brief mouseMotion callback, sends data to the DGLMouse object
   *
   * @param A horizontal mouse position, ranges from -1 to 1 within the
   * render surface
   *
   * @param B vertical mouse position, ranges from -1 to 1 within the
   * render surface
   */
  virtual void mouseMotion( float A, float B) ;
  
  /*!
   * @brief passiveMouseMotion callback, sends data to the DGLMouse object
   *
   * @param A horizontal mouse position, ranges from -1 to 1 within the
   * render surface
   *
   * @param B vertical mouse position, ranges from -1 to 1 within the
   * render surface
   */
  virtual void passiveMouseMotion( float A, float B);

  /*!
   * @brief buttonPress callback, sends data to the DGLMouseButton object
   *
   * @param A horizontal mouse position, ranges from -1 to 1 within the
   * render surface
   *
   * @param B vertical mouse position, ranges from -1 to 1 within the
   * render surface
   *
   * @param Button the number of the button that was pressed in the render
   * surface
   */
  virtual void buttonPress( float A, float B, unsigned int Button);

  /*!
   * @brief doubleButtonPress callback, currently a no-op
   *
   * @param A horizontal mouse position, ranges from -1 to 1 within the
   * render surface
   *
   * @param B vertical mouse position, ranges from -1 to 1 within the
   * render surface
   *
   * @param Button the number of the button that was pressed in the render
   * surface
   */
  virtual void doubleButtonPress( float A, float B, unsigned int  Button){};
  
  /*!
   * @brief buttonRelease callback, sends data to the DGLMouseButton object
   *
   * @param A horizontal mouse position, ranges from -1 to 1 within the
   * render surface
   *
   * @param B vertical mouse position, ranges from -1 to 1 within the
   * render surface
   *
   * @param Button the number of the button that was pressed in the render
   * surface
   */
  virtual void buttonRelease( float A, float B, unsigned int Button);

  /*!
   * @brief keyPress callback, sends data to the DGLKeyboard object
   *
   * @param A the key that was pressed
   */
  virtual void keyPress( KeyCharacter A);

  /*!
   * @brief keyRelease callback, sends data to the DGLKeyboard object
   *
   * @param A the key that was releaseed
   */
  virtual void keyRelease( KeyCharacter A);

  /*!
   * @brief specialKeyPress callback, sends data to the DGLKeyboard object
   *
   * @param A the key that was pressed
   */
  virtual void specialKeyPress( KeyCharacter A);

  /*!
   * @brief specialKeyRelease callback, sends data to the DGLKeyboard object
   *
   * @param A the key that was releaseed
   */
  virtual void specialKeyRelease( KeyCharacter A);


  /*!
   * @brief clears the dtkManager::state dtkmanager::DTK_ISRUNNING bit
   *
   * This is called when the escape key is pressed, unless
   * DGL::setShutdownOnEscape is passed false
   */
  virtual void shutdown();

  /*!
   * @brief idle callback, currently a no-op
   *
   * @return false
   */
  virtual bool idle(){return false;};

  /*!
    @brief Creates new DGLMouse DGLKeyboard and DGLMouseButton objects
  */
  void setup();

 private:
  RenderSurface*  m_rs;
  DGLWindow*      m_win ;
  DGLKeyboard*    m_kb ;
  DGLMouse*       m_mouse ;
  DGLMouseButton* m_mousebutton;
};
#endif
