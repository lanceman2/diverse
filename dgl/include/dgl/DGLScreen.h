#ifndef _dgl_screen_h_
#define _dgl_screen_h_
class DGLWindow;
class DGLViewport;
#include <vector>
#include <dtk.h>
using namespace std;

/*!
 * @class DGLScreen DGLScreen.h dgl.h
 *
 * @brief DGL Screen class.  
 *
 * A "screen" object describes the viewing frustum parameters, and the
 * position of the viewing frustum in the virtual world.  There is one
 * object of this class for each screen.  A screen contains viewports, or
 * mapped areas of a window, as described by the DGLViewport class, one
 * DGLViewport object per viewport.
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */

class DGLAPI DGLScreen: public dtkBase
{
 public:
  
  /*!
   * If TYPE is PERSPECTIVE, specifes which axis has its field of view
   * modifed when the aspect ratio of the window is modified.
   *
   * If TYPE is ORTHO, specifes which side of the ortho box is
   * modifed when the aspect ratio of the window is modified.
   *
   */
  enum AUTOASPECT { VERTICAL,   /*!< the PERSPECTIVE field of view of the
				  vertical axis remains constant, or the
				  ORTHO top and bottom values remain
				  unchanged.  This is the default */
		    HORIZONTAL, /*!< the PERSPECTIVE the field of view of
				  the horizontal axis remains constant, or
				  the ORTHO top and bottom values remain
				  unchanged. */
		    NONE        /*!< neither axis has its field of view
				  modified, and the ortho sides remain
				  unchanged.  Changes to the window's aspect
				  ratio will distort the geometry.  */
  };

  // generic constructior
  DGLScreen()
    {
      validate(); 
      m_fov = 60; 
      m_left = -1.f;
      m_right = 1.f;
      m_bottom = -1.f;
      m_top = 1.f;
      m_autoAspect = VERTICAL; 
      m_near = .1f ;
      m_far = 1000.f ;
      // interoccular distance is in meters
      m_interoccular = .07f ;
      m_offset.zero() ;
      m_width = 2.f ;
      m_height = 2.f ;
    };
  
  // rit up!
  virtual ~DGLScreen(){m_viewport.erase(m_viewport.begin(), m_viewport.end());};
  
  /*!
   * @brief Add a DGLViewport to the std::vector of DGLViewport objects
   *
   * @param vp a pointer to the DGLViewport object to be added to the
   * std::vector of DGLViewport objects
   */
  virtual int addViewport(DGLViewport* vp)
    {m_viewport.push_back(vp); return 0;};
  
  /*!
   * @brief Return a pointer the specified DGLViewport
   *
   * @param num an index into the std::vector of DGLViewport objects
   *
   * @return a pointer the specified DGLViewport
   */
  virtual DGLViewport* getViewport(unsigned int num)
    { if (num < m_viewport.size()) return m_viewport[num]; else return NULL;};
  
  /*!
   * @brief Return the number of DGLViewport objects in this screen
   *
   * @return the number of DGLViewport objects in this screen
   */
  virtual unsigned int getNumViewports()
    {return m_viewport.size();};

  /*!
   * @brief Set the near clipping plane distance in DIVERSE coordinates
   *
   * @param n the near clipping plane distance in DIVERSE coordinates
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setNear(float n)
    {m_near=n; return 0;};

  /*! 
   * @brief Return the near clipping plane distance in DIVERSE coordinates 
   *
   * @return the near clipping plane distance in DIVERSE coordinates
   */
  virtual float getNear()
    {return m_near;};

  /*!
   * @brief Set the far clipping plane distance in DIVERSE coordinates
   *
   * @param n the far clipping plane distance in DIVERSE coordinates
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setFar(float n)
    {m_far = n; return 0;};
  
  /*! 
   * @brief Return the far clipping plane distance in DIVERSE coordinates 
   *
   * @return the far clipping plane distance in DIVERSE coordinates
   */
  virtual float getFar()
    {return m_far;};
  
  /*!
   * @brief Set the interoccular distance in meters
   *
   * The interoccular distance is the distance between the eyes, and is used
   * to determine the stereo parallax.  It's given in meters, so a good
   * value should work for any sized virtual world.  The default value is
   * .07 meters (2.75 inches), the interoccular distance of Dr. Fernando das
   * Neves.
   *
   * @param v the interoccular distance in meters
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setInterOccular(float v)
    {m_interoccular = v; return 0;};

  /*! 
   * @brief Return the interoccular distance in meters
   *
   * See DGLScreen::setInterOccular for more details.
   *
   * @return the interoccular distance in meters
   */
  virtual float getInterOccular()
    {return m_interoccular;};
  
  /*!
   * @brief Specify the offset of the screen from the origin
   * 
   * The offset is used differently, depending on the value of the
   * DGLViewport::TYPE.
   *
   * If the DGLViewport::TYPE is DGLViewport::SCREEN, then the offset
   * specifies the position and orientation of the center of the base of the
   * viewing frustum, which usually corresponds to a physical display surface.
   *
   * If the DGLViewport::TYPE is DGLViewport::PERSPECTIVE, then the offset
   * specifies the position and orientation of the virtual camera in the
   * virtual world.
   *
   * @param offset specifies the offset postion and orientation
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setOffset(dtkCoord offset)
    {m_offset = offset; return 0;};

  /*! 
   * @brief Return the offset of the screen from the origin
   *
   * See DGLScreen::setOffset for more details.
   *
   * @return the offset of the screen from the origin
   */
  virtual dtkCoord getOffset()
    {return m_offset;};

  /*!
   * @brief Set the sides of the orthonormal projection, in diverse coordinates
   *
   * @param left specifies the left edge of the orthonormal projection.  The default is -1.
   *
   * @param right specifies the right edge of the orthonormal projection.  The default is 1.
   *
   * @param bottom specifies the bottom edge of the orthonormal projection.  The default is -1.
   *
   * @param top specifies the top edge of the orthonormal projection.  The default is 1.
   *
   * The orthoSides are only used if the DGLViewport::TYPE is DGLViewport::ORTHO
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int   setOrthoSides(float left, float right, float bottom, float top )
    {m_left = left; m_right = right; m_bottom = bottom; m_top = top; return 0;};

  /*!
   * @brief Get the sides of the orthonormal projection, in diverse coordinates
   *
   * @param left is set to the left edge of the orthonormal projection.
   *
   * @param right is set to the right edge of the orthonormal projection.
   *
   * @param bottom is set to the bottom edge of the orthonormal projection.
   *
   * @param top is set to the top edge of the orthonormal projection.
   *
   * The orthoSides are only used if the DGLViewport::TYPE is DGLViewport::ORTHO
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int   getOrthoSides(float &left, float &right, float &bottom, float &top )
    {left = m_left; right = m_right; bottom = m_bottom; top = m_top; return 0;};

  /*!
   * @brief Set the field of view in degrees
   *
   * @param fov the field of view in degrees.  The deafult is 60.
   *
   * The fov is only used if the DGLViewport::TYPE is DGLViewport::PERSPECTIVE 
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int   setFov(float fov)
    {m_fov = fov; return 0;};

  /*! 
   * @brief Return the field of view in degrees
   *
   * The fov is only used if the DGLViewport::TYPE is DGLViewport::PERSPECTIVE 
   *
   * @return the field of view in degrees
   */
  virtual float getFov()
    {return m_fov;};

  /*!
   * @brief Set the auto-aspect axis
   *
   * See DGLScreen::AUTOASPECT for more details
   *
   * Auto-aspect is only used if the DGLViewport::TYPE is
   * DGLViewport::PERSPECTIVE or DGLViewport::ORTHO
   *
   * @param autoAspect the axis whose field of view remains constant when
   * the containing window is resized
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int   setAutoAspect(AUTOASPECT autoAspect)
    {m_autoAspect = autoAspect; return 0;};

  /*! 
   * @brief Return the auto-aspect axis
   *
   * See DGLScreen::AUTOASPECT for more details
   *
   * Auto-aspect is only used if the DGLViewport::TYPE is DGLViewport::PERSPECTIVE 
   *
   * @return the auto-aspect axis
   */
  virtual AUTOASPECT getAutoAspect()
    {return m_autoAspect;};

  /*!
   * @brief Return the DGLWindow object containing this DGLScreen object
   *
   * @return the DGLWindow object containing this DGLScreen object
   */
  virtual DGLWindow* getWindow() 
    {return m_window;};

  /*!
   * @brief specify which DGLWindow contains this DGLScreen
   *
   * @param input the DGLWindow which contains this DGLScreen
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setWindow(DGLWindow* input)
    {m_window = input; return 0;};

  /*!
   * @brief Set the width of this DGLScreen object in DIVERSE units.
   *
   * @param width the width of this DGLScreen object in DIVERSE units.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setWidth(float width)
    { m_width = width; return 0;};

  /*! 
   * @brief Return the width of this DGLScreen object in DIVERSE units.
   *
   * @return the width of this DGLScreen object in DIVERSE units
   */
  virtual float getWidth()
    { return m_width;};
  
  /*!
   * @brief Set the height of this DGLScreen object in DIVERSE units.
   *
   * @param height the height of this DGLScreen object in DIVERSE units.
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setHeight(float height)
    { m_height = height; return 0;};

  /*! 
   * @brief Return the height of this DGLScreen object in DIVERSE units.
   *
   * @return the height of this DGLScreen object in DIVERSE units
   */
  virtual float  getHeight()
    { return m_height;};
  
  /*!
   * @brief Calls DGLScreen::frame for every DGLViewport in this DGLScreen
   */
  virtual void frame();

  /*!
   * @brief Calls DGLScreen::clear for every DGLViewport in this DGLScreen
   */
  virtual void clear();
  
 private:
  float         m_near;
  float         m_far;
  float         m_left;
  float         m_right;
  float         m_bottom;
  float         m_top;
  float         m_interoccular;
  float         m_fov;
  AUTOASPECT    m_autoAspect;
  float         m_width;
  float         m_height;
  dtkCoord      m_offset;
  DGLWindow*    m_window;
  vector<DGLViewport*>  m_viewport;

};
#endif
