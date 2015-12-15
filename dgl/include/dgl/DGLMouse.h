#ifndef __DGL_MOUSE_H__
#define __DGL_MOUSE_H__
#include <map>

/*!
 * @class DGLMouse DGLMouse.h dgl.h
 *
 * @brief Reads and writes positional mouse data.  
 *
 * It contains callbacks which are called by Producer when a Mouse event
 * occurs, and methods DGL programs can use to access this data.
 *
 * Although this class can be used by anyone, in DGL it used to tie Producer
 * positional mouse events to the dtkInValuator "pointer"
 *
 * In addition to the standard dtkInValuator "pointer", the mouse position
 * is also written as two floats to the dtk shared memory file "dgl/mouse"
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class DGLAPI DGLMouse
{
public:
  // usual constructor
  DGLMouse(void) ;

  /*!
   * @brief Write new mouse data
   *
   * @param x horizontal mouse position, ranges from -1 to 1 
   *
   * @param y vertical mouse position, ranges from -1 to 1 
   *
   * @return zero on success, non-zero if an error occurred
   */
  int setMouse(float x, float y) ;

  /*!
   * @brief Read mouse, queued
   *
   * Similar to dtkSharedMem::qRead()
   *
   * @param x horizontal mouse position, ranges from -1 to 1 
   *
   * @param y vertical mouse position, ranges from -1 to 1 
   *
   * @return 0 if no new data, -1 on error, else number of items on queue
   */
  int readMouse(float *x, float *y) ;

  /*!
   * @brief Polls current mouse position
   *
   * Similar to dtkSharedMem::read()
   *
   * @param x horizontal mouse position, ranges from -1 to 1 
   *
   * @param y vertical mouse position, ranges from -1 to 1 
   *
   * @return zero on sucess, non-zero on error
   */
  int getMouse(float *x, float *y) ;
  
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
  dtkManager*        m_manager;
  dtkInValuator*     m_pointer;
  dtkSharedMem*      m_mouse;
  float              m_position[2];
  // the cursor can only be in one window at a time
  static DGLWindow*  m_window;
} ;

#endif
