#ifndef _dgl_window_h_
#define _dgl_window_h_

#include <vector>

#ifdef OLDPRODUCER
#include <Producer/RenderSurface>
#include <Producer/KeyboardMouse>
#else
#include <Producer/RenderSurface.h>
#include <Producer/KeyboardMouse.h>
#endif


using namespace std;
class DGLPipe;
class DGLScreen;
class DMKCallback;

/*!
 * @class DGLWindow DGLWindow.h dgl.h
 *
 * @brief DGL Window class.  
 *
 * A "window" is a managed region in a pipe.  There is one object of this class
 * for each window.  A window contains screens, as described by the DGLScreen
 * class, one DGLScreen object per screen.
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */

class DGLAPI DGLWindow : public dtkBase
{
 public:
  /*!
   * Create a window with a specified name
   * 
   * @param name a text string passed to the window manager, and often
   * displayed in the window's decorations
   */
  DGLWindow(char* name="DGL Base Window") 
    {validate(); 
    m_name = strdup(name); 
    m_depth=24; 
    m_eventTranslator = NULL; 
    m_cursor=true; 
    m_parent=false; 
    m_child = false; 
    m_parentWindow = false; 
    m_border=true;
    m_fullscreen=false;
    m_resizeable=true;
    m_stereo=false;
    m_left=0; m_bottom=0; m_width=512; m_height=512;
    m_depth=24;
    m_pipe=NULL;
    m_parentWindow=NULL;
    m_surface=NULL;
    m_screen.empty();
    m_km=NULL;
    m_eventTranslator=NULL;
    };

  // tidy up!
  virtual ~DGLWindow()
    {m_screen.erase(m_screen.begin(), m_screen.end()); free(m_name);};

  /*!
   * @brief specify which DGLPipe object contains this DGLWindow object
   *
   * @param input the pipe which contains this window
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setPipe(DGLPipe* input)
    {m_pipe = input; return 0;};

  /*!
   * @brief Return a pointer to the Window's DGLPipe
   *
   * @return a pointer to the Window's DGLPipe
   */
  virtual DGLPipe* getPipe()
    {return m_pipe;};

  /*!
   * @brief Return the number of DGLScreen objects in this DGLWindow object
   *
   * @return the number of DGLScreen objects in this window
   */
  virtual unsigned int getNumScreens() {return m_screen.size();};
  
  /*!
   * @brief Return a pointer the specified DGLScreen
   *
   * @param screenNum an index into the std::vector of DGLScreen objects
   *
   * @return a pointer the specified DGLScreen
   */
  virtual DGLScreen* getScreen(unsigned int screenNum)
    {
      if (screenNum < m_screen.size()) 
	return m_screen[screenNum]; 
      else 
	return NULL;
    };

  /*!
   * @brief Add a DGLScreen to the std::vector of DGLScreen objects
   *
   * @param input a pointer to the DGLScreen object to be added to the
   * std::vector of DGLScreen objects
   */
  virtual int addScreen(DGLScreen* input) 
    {
      if (!input)
	return 1 ;
      m_screen.push_back(input); 
      return 0;
    };

  /*!
   * @brief Specify if this window is in stereo
   *
   * @param value true if the window is in streo, otherwise false
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setStereo(bool value)
    { m_stereo = value; return 0;};

  /*!
   * @brief Return true if this window is in stereo
   *
   * @return true if this window is in stereo
   */
  virtual bool getStereo() 
    {return m_stereo;};
  
  /*!
   * @brief Specify if this window should occupy the entire pipe
   *
   * @param value true if the window should occupy the entire pipe
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setFullScreen(bool value) 
    {m_fullscreen = value; return 0;};

  /*!
   * @brief Return true if this window occupies the entire pipe
   *
   * @return true if this window occupies the entire pipe
   */
  virtual bool getFullScreen()
    {return m_fullscreen;};
  
  /*!
   * @brief Specify if this window should have a border
   *
   * @param value true if the window should have a border
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setBorder(bool value)
    {m_border = value; return 0;};
  
  /*!
   * @brief Return true if this window has a border
   *
   * @return true if this window has a border
   */
  virtual bool getBorder()
    {return m_border;};

  /*!
   * @brief Specify if this window should have a cursor
   *
   * @param value true if the window should have a cursor
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setCursor(bool value)
    {m_cursor = value; return 0;};
  
  /*!
   * @brief Return true if this window has a cursor
   *
   * @return true if this window has a cursor
   */
  virtual bool getCursor()
    {return m_cursor;};
  
  /*!
   * @brief Specify if this window can be resized
   *
   * @param value true if the window can be resized
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setResizeable(bool value)
    {m_resizeable = value; return 0;};
  
  /*!
   * @brief Return true if this window can be resized
   *
   * @return true if this window can be resized
   */
  virtual bool getResizeable() 
    {return m_resizeable;};
  
  /*!
   * @brief Specify the number of bits of depth for this window
   * 
   * @param depth the number of bits of depth for this window
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setDepth(unsigned int depth)
    {
      m_depth = depth; 
      return 0;
    };
  
  /*!
   * Return the number of bits of depth for this window
   *
   * @return the number of bits of depth for this window
   */
  virtual unsigned int getDepth(){return m_depth;};
  
  /*!
   * @brief Set the dimensions of this window
   *
   * @param left the left-most pixel position of the window
   *
   * @param bottom the bottom-most pixel position of the window
   *
   * @param width the width of the window in pixels
   *
   * @param height the height of the window in pixels
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setWindowDimensions(int left, int bottom, unsigned int width, unsigned int height)
    {
      m_left = left;
      m_bottom = bottom;
      m_width = width;
      m_height = height;
      return 0;
    }
  
  /*!
   * @brief Get the dimensions of this window
   *
   * @param left the left-most pixel position of the window
   *
   * @param bottom the bottom-most pixel position of the window
   *
   * @param width the width of the window in pixels
   *
   * @param height the height of the window in pixels
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int getWindowDimensions(int &left, int &bottom, unsigned int &width, unsigned int &height) ;

  /*!
   * @brief Create the window but don't realize it
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int create();

  /*!
   * @brief Realize the window
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int realize();

  /*!
   * @brief Perform post realize commands (full screen , keyboard and mouse,
   * etc...)
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int postRealize();

  /*!
   * @brief Set the name of the window
   *
   * @param name the name of the window
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setName(char* name)
    {m_name = name; return 0;};

  /*! 
   * @brief Return the name of the window
   *
   * @return the name of the window
   */
  virtual char* getWindowName()
    {return m_name;};

  /*!
   * @brief Return the Producer::RenderSurface associated with this window
   *
   * @return the Producer::RenderSurface associated with this window
   */
  virtual Producer::RenderSurface* getRenderSurface()
    {return m_surface;};

  /*!
   * @brief Update keyboard and mouse data
   */
  virtual void updateKM();

  /*!
   * @brief set a callback for keyboard and mouse events.
   *
   * By default, the callback is an object of the DMKCallback class, or one
   * of its subclasses.
   *
   * @param cb an object of the Producer::KeyboardMouseCallback class
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setKMCallback(Producer::KeyboardMouseCallback* cb)
    { m_eventTranslator = cb; return 0;};

  /*!
   * @brief Return true if this window is the shared parent window.
   *
   * @return true if this window is the shared parent window
   */
  virtual bool getSharedParent() 
    {return m_parent;};

  /*!
   * @brief Specify if this window is to be the shared parent window.
   *
   * @param Parent true if this window is to be the shared parent window, otherwise false
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int  setSharedParent(bool Parent) 
    {m_parent=Parent; return 0;};

  /*!
   * @brief Return true if this window is a shared child window.
   *
   * @return true if this window is a shared child window
   */
  virtual bool getSharedChild() 
    {return m_child;};

  /*!
   * @brief Specify if this window is to be a shared child window.
   *
   * @param Child is true if this window is be a shared child window, otherwise false
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int  setSharedChild(bool Child) 
    {m_child = Child; return 0;};

  /*!
   *
   * @brief Set the shared parent window
   *
   * @param window the shared parent window
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setSharedParentWindow(DGLWindow* window)
    {
      // shortcut- set child based on parent's pointer
      if (window)
	m_child = true ;
      else
	m_child = false ;
      m_parentWindow = window; 
      return 0;
    };
  
  /*!
   * @brief Return the shared parent window
   *
   * @return the shared parent window
   */
  virtual DGLWindow* getSharedParentWindow()
    {return m_parentWindow;};

  /*!
   * @brief Return a number unique to this window
   *
   * @return a number unique to this window
   */
  int getContextNum() { return m_cNum;};

 private:
  bool m_parent;
  bool m_child;
  bool m_cursor;
  bool m_border;
  bool m_fullscreen;
  bool m_resizeable;
  bool m_stereo;
  int  m_left;
  int  m_bottom;
  unsigned int  m_width;
  unsigned int  m_height;
  unsigned int  m_depth;
  char* m_name;
  DGLPipe* m_pipe;
  DGLWindow* m_parentWindow;
  Producer::RenderSurface* m_surface;
  vector <DGLScreen*> m_screen;
  Producer::KeyboardMouse* m_km;
  Producer::KeyboardMouseCallback* m_eventTranslator;
  Producer::VisualChooser* m_chooser;
  //The context number for all windows.  1 per window
  static int m_context;
  //The number for this specific window
  int m_cNum;
};
#endif
