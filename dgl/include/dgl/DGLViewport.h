#ifndef _dgl_viewport_h_
#define _dgl_viewport_h_
#include <dtk.h>


#ifdef OLDPRODUCER
#include <Producer/Camera>
#else
#include <Producer/Camera.h>
#endif

class DGLScreen;
class DGLScreenHandler;
class DGLPerspectiveHandler;
class DGLOrthoHandler;

/*!
 * @class DGLViewport DGLViewport.h dgl.h
 *
 * @brief DGL Viewport class.  
 *
 * A "viewport" object describes the type of viewing frustum that is
 * generated for the parent DGLScreen, and the location and size in the
 * parent DGLWindow the resulting two-dimensional projection is located
 *
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */

class DGLAPI DGLViewport : public dtkBase
{
 public:
  /*!
   * Specifies the type of view frustum to be generated
   */
  enum TYPE { PERSPECTIVE, /*!< a right-angle frustum will be generated, as
			     usually used in desktop windows. */ 
	      ORTHO,       /*!< an orthonormal, or parallel projection, as
                             usually used in dsktop CAD- related windows */
	      SCREEN       /*!< an "off-axis" frustum will be generated, as
			     usually used by head-tracked immersive
			     displays */
  };
  
  // usual constructor
  DGLViewport() 
    { validate(); 
    m_x=-1; m_y=-1; m_w=0; m_h=0; 
    m_view_state=false; 
    m_view_coord.set(0.f, 0.f, 0.f, 0.f, 0.f, 0.f); 
    m_handler = NULL; 
    m_perspective_handler = NULL ;
    m_ortho_handler = NULL ;
    m_screen_handler = NULL ;
    m_camera = NULL;
    };
  
  // cleanliness is next to godessness
  ~DGLViewport();

  /*!
   * @brief Specify the size and position of the DGLViewport in the DGLWindow
   *
   * @param x the offset of the left edge of the viewport, in screen
   * coordinates, relative to the left edge of the parent DGLWindow
   *
   * @param y the offset of the bottom edge of the viewport, in screen
   * coordinates, relative to the bottom edge of the parent DGLWindow
   *
   * @param w the width of the viewport, in screen coordinates
   *
   * @param h the height of the viewport, in screen coordinates
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setGeometry(int x, int y, unsigned int w, unsigned int h);

  /*!
   * @brief Get the viewport's geometry in the passed parameters
   *
   * @param x the offset of the left edge of the viewport, in screen
   * coordinates, relative to the left edge of the parent DGLWindow
   *
   * @param y the offset of the bottom edge of the viewport, in screen
   * coordinates, relative to the bottom edge of the parent DGLWindow
   *
   * @param w the width of the viewport, in screen coordinates
   *
   * @param h the height of the viewport, in screen coordinates
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int getGeometry(int& x, int& y, unsigned int& w, unsigned int& h);

  /*!
   * @brief specify which DGLScreen contains this DGLViewport
   *
   * @param input the DGLScreen which contains this DGLViewport
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setScreen(DGLScreen* input)
    {m_screen = input; return 0;};

  /*!
   * @brief Return the DGLScreen object containing this DGLViewport object
   *
   * @return the DGLScreen object containing this DGLViewport object
   */
  virtual DGLScreen* getScreen()
    {return m_screen; return 0;};
  
  /*!
   * @brief Set the viewport type
   *
   * Specifies which type of viewing frustum is generated
   *
   * @param intype the type of frustum to be generated
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setViewportType(TYPE intype) ;

  /*!
   * @brief Return the type of viewing frustum that is generated
   *
   * @return the type of viewing frustum that is generated
   */
  TYPE getViewportType() { return m_type;};
  
  /*!
   * @brief Specify how to determine the tip of the view frustum
   *
   * @param v true if the tip of the frustum is specified in
   * DGLViewport::setViewCoord, false if the tip of the frustum is at the
   * head position (or the origin if not head tracking)
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setViewState(bool v)
    { m_view_state = v ;  return 0;};
  
  /*!
   * @brief Return how the tip of the view frustum is determined
   *
   * @return true if the tip of the frustum is specified in
   * DGLViewport::setViewCoord, false if the tip of the frustum is at the
   * head position (or the origin if not head tracking)
   */
  virtual bool getViewState(void)
    { return m_view_state;} ;

  /*!
   * @brief Specify the tip of the view frustum
   *
   * This position is only used if DGLViewport::getViewState returns true
   *
   * @param v the position and orientation of the tip of the view frustum
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setViewCoord(dtkCoord v)
    { m_view_coord = v ;  return 0;};
  
  /*!
   * @brief Specify the tip of the view frustum
   *
   * This position is only used if DGLViewport::getViewState returns true
   *
   * @param v the position (v[0]-v[2]) and orientation (v[3]-v[5]) of
   * the tip of the view frustum.  Orientation is an Euler angle.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setViewCoord(float v[6])
    { m_view_coord.set(v) ; return 0;};
  
  /*!
   * @brief Return the tip of the view frustum
   *
   * This position is only used if DGLViewport::getViewState returns true
   *
   * @return the tip of the view frustum
   */
  virtual dtkCoord getViewCoord(void)
    { return m_view_coord;} ;
  
  /*!
   * @brief Create screen handlers and Producer objects for this viewport
   *
   * A DGLPerspectiveHandler object is created if the DGLViewport::TYPE is
   * DGLViewport::PERSPECTIVE.  A DGLScreenHandler object is created if the
   * DGLViewport::TYPE is DGLViewport::Screen.  A Producer::Camera object is
   * also created, and its Producer::Camera::RenderSurface is set to the
   * parent window's Producer::Camera::RenderSurface.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int create();

  /*!
   * @brief Call Producer::Camera::frame
   */
  virtual void frame();

  /*!
   * @brief Call the screen handler's clear method, setting the clear color
   * to what is returned by DGL::getClearColor
   */
  virtual void clear();
  
  /*!
   * @brief Return a pointer to the screen handler object
   *
   * Will only return a valid viewport after the DGLViewport::create has been called
   *
   * @return a pointer to the screen handler object
   */
  DGLScreenHandler* getScreenHandler(){return m_screen_handler;};
  
  
 private:
  int                            m_x;
  int                            m_y;
  unsigned int                   m_w;
  unsigned int                   m_h;
  bool                           m_view_state;
  dtkCoord                       m_view_coord;
  
  DGLScreen* m_screen;
  DGLPerspectiveHandler*           m_perspective_handler;
  DGLScreenHandler*                m_screen_handler;
  DGLOrthoHandler*                 m_ortho_handler;
  TYPE                             m_type;
  Producer::Camera::SceneHandler*  m_handler;
  Producer::Camera*                m_camera;
};
#endif
