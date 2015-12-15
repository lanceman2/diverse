#ifndef _dgl_screen_handler_h_
#define _dgl_screen_handler_h_

#ifdef OLDPRODUCER
#include <Producer/Camera>
#else
#include <Producer/Camera.h>
#endif

#include <dtk.h>
using namespace Producer;
class DGLViewport;

/*!
 * @class DGLScreenHandler DGLScreenHandler.h dgl.h
 *
 * @brief DGL ScreenHandler class.  
 *
 * This class generates an off-axis frustum, as typically used in
 * head-tracked immersive systems.  The basic idea is that the base of the
 * frustum is at a fixed position, usually the position of the physical
 * screen in the immersive system, and the tip of the frustum moves, usually
 * with the head tracker.
 *
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DGLAPI DGLScreenHandler : public Producer::Camera::SceneHandler
{
 public:
  
  // generic constuctor
  DGLScreenHandler();

  // and generic destructor
  virtual ~DGLScreenHandler();

  /*!
   * Specify whether we are drawing at the left eye position, right eye
   * position, or are in mono and using the bridge of the nose.
   * DGLScreen::setInterOccular determines the distance from the bridge of
   * the nose to the left and right eyes.
   */
  enum VIEW
    {
      MONO,      /*!< draw in mono */
      LEFT_EYE,  /*!< draw the left eye */
      RIGHT_EYE  /*!< draw the right eye */
    };

  /* commented out jtk 7/06
     int setOffset(dtkCoord off)
     {m_offset = off; return 0;};
  */

  /* commented out jtk 7/06
   int setAspectRatio( float height_over_width)
   {m_aspect = height_over_width; return 0;};
   
   float getAspectRatio()
   {return m_aspect;};
  */

  /*!
   * @brief Set up graphics and buffers, then call DGLScreenHandler::clear
   * and DGLScreenHandler::draw for each eye
   *
   * @param cam the Producer::Camera object containing the
   * Producer::Camera::RenderSurface
   *
   * @return true on success, false if an error occurs
   */
  virtual bool frame (Camera& cam);

  /*!
   * @brief Clears to the color returned by DGL::getClearColor
   *
   * Called for every eye
   *
   * @param cam the Producer::Camera object containing the
   * Producer::Camera::RenderSurface
   */
  virtual void clear( Camera& cam);

  /* commented out jtk 7/06
     virtual bool useAutoView() { return false;};
  */

  /*!
   * @brief Draws the graphics
   *
   * Called for every eye
   *
   * @param cam the Producer::Camera object containing the
   * Producer::Camera::RenderSurface
   */
  virtual void draw(Camera& cam);

  /*!
   * @brief Set the DGLViewport that created this screen handler
   *
   * @return 0 on success, non-zero if an error occurs
   */
  int setViewport(DGLViewport* port) 
    { m_viewport = port; return 0;};

  /*!
   * @brief Return the DGLViewport that created this screen handler
   *
   * @return the DGLViewport that created this screen handler
   */
  DGLViewport* getViewport() 
    {return m_viewport;};
	
  /*!
   * @brief Set which eye this screen is displaying
   *
   * This is used to allow for passive stereo walls to be supported
   */
  void setEye(VIEW eye){m_eye = eye;};

  /*! 
   * @brief Return which eye this screen is displaying
   *
   * @return which eye this screen is displaying
   */
  VIEW getEye(){ return m_eye;};

 private:
  float             m_aspect;
  Producer::Camera* m_camera;
  DGLViewport*      m_viewport;
  dtkCoord          m_offset;
  dglVec3<float>    m_eyevec;
  dglVec4<float>    m_up;
  dglVec4<float>    m_projection;
  dglMatrix         m_rot_mat;
  dglMatrix         m_inv_rot_mat;
  dtkManager*       m_manager;
  dtkInLocator*     m_head;
  float             m_headpos[6];
  VIEW              m_eye;
};
#endif
