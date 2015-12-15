#ifndef __DGL_MOUSE_BUTTON_H__
#define __DGL_MOUSE_BUTTON_H__
#include <map>

/*!
 * @class DGLMouseButton DGLMouseButton.h dgl.h
 *
 * @brief Reads and writes positional mouse and button data.  
 *
 * It contains callbacks which are called by Producer when a MouseButton
 * event occurs, and methods DGL programs can use to access this data.
 *
 * Although this class can be used by anyone, in DGL it used to tie Producer
 * positional mouse button events to the dtkInValuator "pointer" and the
 * dtkInButton "buttons"
 *
 * In addition to the standard dtkInValuator "pointer", the mouse and button
 * data are written to the dtk shared memory file "dgl/mousebutton" as a
 * DGLMouseButton::RECORD
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class DGLAPI DGLMouseButton
{
public:
  /*!
   * Specifies the button action
   */
  enum ACTION {PRESSED=1,  /*!< a button press */
	       RELEASED=0, /*!< a button release */
	       UNKNOWN=-1  /*!< unknown button state */
  } ;

  /*!
   * @brief Data for a mouse button event
   *
   * Written to the dtk shared memory file "dgl/mousebutton"
   */
  struct RECORD
  {
    unsigned int b ; /*!< button number */
    float x ;        /*!< horizontal position in the window at the time of
		       the event, between -1 and 1 */
    float y ;        /*!< vertical position in the window at the time of
		       the event, between -1 and 1 */
    ACTION a ;       /*!< what type of button event */
  } ;

  /*!
   * a std::map between a button number (the key) and its state (the value)
   */
  typedef std::map<unsigned int, int> MOUSEBUTTONMAP ;

  /*!
   * @brief Return the specified button's state
   *
   * @param B button number
   *
   * @return the specified button's state
   */
  static ACTION getState(unsigned int B) ;
  DGLMouseButton(void) ;

  /*!
   * @brief Performs a read of the mouseButton queue.  
   *
   * @param a set with data from mouseButton event, if any occured
   *
   * @return 0 if no mouse button changed, else number of mouse buttons changed
   */
  unsigned int readMouseButton(ACTION *a) ;

  /*!
   * @brief Performs a read of the mouseButton queue.  
   *
   * @param x horizontal position in the window at the time of the event,
   * between -1 and 1
   *
   * @param y vertical position in the window at the time of the event,
   * between -1 and 1
   *
   * @param a set with data from mouseButton event, if any occured
   *
   * @return 0 if no mouse button changed, else number of mouse buttons changed
   */
  unsigned int readMouseButton(float *x, float *y, ACTION *a) ;

  /*!
   * @brief Writes mouse button data
   *
   * @param b button number
   *
   * @param x horizontal position in the window at the time of the event,
   * between -1 and 1
   *
   * @param y vertical position in the window at the time of the event,
   * between -1 and 1
   *
   * @param a set with data from mouseButton event, if any occured
   *
   * @return zero on success, non-zero if an error occurred
   */
  int writeMouseButton(unsigned int b, float x, float y, ACTION a) ;
  
  /*!
   * @brief Specify which window received the mouse input
   *
   * @param w pointer to a DGLWindow
   *
   * This method is called by the DProducerInputCallback object.
   */
  void setWindow(DGLWindow* w) { m_window = w ; } ;

  /*!
   * @brief Returns which window received the mouse input
   *
   * @return pointer to a DGLWindow
   */
  DGLWindow *getWindow() { return m_window ; } ;

 private:
  dtkManager*           m_manager;
  dtkInButton*          m_button;
  dtkSharedMem*         m_mousebutton;
  DGLMouse*             m_mouse ;
  static MOUSEBUTTONMAP state ;
  // the cursor can only be in one window at a time
  static DGLWindow*     m_window;
} ;

#endif
