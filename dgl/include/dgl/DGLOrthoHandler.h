#ifndef _dgl_ortho_handler_h_
#define _dgl_ortho_handler_h_

class DTKSharedMem;
class DGLViewport;

#ifdef OLDPRODUCER
#include <Producer/Camera>
#else
#include <Producer/Camera.h>
#endif

using namespace Producer;

/*!
 * @class DGLOrthoHandler DGLOrthoHandler.h dgl.h
 *
 * @brief DGL OrthoHandler class.  
 *
 * This class generates a symmetric frustum, as typically used in a desktop
 * window.  The basic idea is that the shape of the frustum is determined by
 * parameters such as field of view and aspect ratio, and the frustum is
 * moved to have it's tip at orientation at the specified offset.
 *
 * Currently desktop stereo is not implemented.
 *
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DGLAPI DGLOrthoHandler : public Producer::Camera::SceneHandler
{
 public:
  
  // generic constuctor
  DGLOrthoHandler();

  // and generic destructor
  ~DGLOrthoHandler();

  /*!
   * @brief Draws the graphics
   *
   * @param cam the Producer::Camera object containing the
   * Producer::Camera::RenderSurface
   */
  virtual void draw (Camera& cam);

  /*!
   * @brief Clears to the color returned by DGL::getClearColor
   *
   * @param cam the Producer::Camera object containing the
   * Producer::Camera::RenderSurface
   */
  virtual void clear(Camera& cam);

  /* commented out jtk 7/06
     virtual void cull (Camera& cam);
  */

  /*!
   * @brief Set up graphics and buffers, then call DGLScreenHandler::clear
   * and DGLScreenHandler::draw 
   *
   * @param cam the Producer::Camera object containing the
   * Producer::Camera::RenderSurface
   *
   * @return true on success, false if an error occurs
   */
  virtual bool frame(Camera& cam);
  
  /* commented out jtk 7/06
     bool useAutoView() { return false;};
  */

  /* commented out jtk 7/06
     void render();
  */

  /*!
   * @brief Set the DGLViewport that created this screen handler
   *
   * @return 0 on success, non-zero if an error occurs
   */
  int setViewport(DGLViewport* inport){m_viewport = inport; return 0;};

 private:
  DGLViewport*   m_viewport;
  dtkManager*    m_manager;
  dtkInLocator*  m_head;
  float          m_headpos[6];
  // window width & height in pixels
  unsigned int m_width ;
  unsigned int m_height ;
  // screen geometry in pixels
  int m_x ;
  int m_y ;
  unsigned int m_w ;
  unsigned int m_h ;
  DGLScreen::AUTOASPECT m_aspect ;
  float m_aspect_ratio ;
  // sides of ortho cube, in diverse coords
  float m_left ;
  float m_right ;
  float m_bottom ;
  float m_top ;
  float m_delta ;
  float m_center ;
};
#endif
