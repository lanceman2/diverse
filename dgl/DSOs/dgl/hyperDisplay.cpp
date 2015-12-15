// a desktop disply to show symmetric (DGLViewport::PERSPECTIVE) and
// asymmetric (DGLViewport::SCREEN) frusta, as a demo to get a feeling for
// how they work.

////////////////////////////////////////////////////////////////////////

#define SIMPLE
#define SYMMETRIC
#define ASYMMETRIC
#define ORTHONORMAL

#define SCREENS (6)
#define FLOOR   (0)
#define LEFT    (1)
#define FRONT   (2)
#define RIGHT   (3)
#define BACK    (4)
#define CEILING (5)

#define FLOOR_NAME   "floor"
#define LEFT_NAME    "left"
#define FRONT_NAME   "front"
#define RIGHT_NAME   "right"
#define BACK_NAME    "back"
#define CEILING_NAME "ceiling"

#define FLOOR_H   ( 00)
#define FLOOR_P   (-90)
#define FLOOR_R   ( 00)
#define FLOOR_CENTER_X ( 0.f)
#define FLOOR_CENTER_Y ( 0.f)
#define FLOOR_CENTER_Z (-1.f)

#define LEFT_H    ( 90)
#define LEFT_P    ( 00)
#define LEFT_R    ( 00)
#define LEFT_CENTER_X (-1.f)
#define LEFT_CENTER_Y ( 0.f)
#define LEFT_CENTER_Z ( 0.f)

#define FRONT_H   ( 00)
#define FRONT_P   ( 00)
#define FRONT_R   ( 00)
#define FRONT_CENTER_X ( 0.f)
#define FRONT_CENTER_Y ( 1.f)
#define FRONT_CENTER_Z ( 0.f)

#define RIGHT_H   (-90)
#define RIGHT_P   ( 00)
#define RIGHT_R   ( 00)
#define RIGHT_CENTER_X ( 1.f)
#define RIGHT_CENTER_Y ( 0.f)
#define RIGHT_CENTER_Z ( 0.f)

#define BACK_H    (180)
#define BACK_P    ( 00)
#define BACK_R    ( 00)
#define BACK_CENTER_X ( 0.f)
#define BACK_CENTER_Y (-1.f)
#define BACK_CENTER_Z ( 0.f)

#define CEILING_H ( 00)
#define CEILING_P ( 90)
#define CEILING_R ( 00)
#define CEILING_CENTER_X ( 0.f)
#define CEILING_CENTER_Y ( 0.f)
#define CEILING_CENTER_Z (1.f)

#include <dtk.h>
#include <dgl.h>
#include <dtk/dtkDSO_loader.h>

/*************** DTK C++ DSO loader/unloader functions ***********/
/*
 * All DTK DSO files are required to declare these two functions.
 * These function are called by the loading program to get your
 * C++ objects loaded.
 *
 *****************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
  
  DGL::init();
  DGL *app = static_cast<DGL *>(manager);
  
  if (!app)
    printf("HUH\n");
  if (app->isInvalid())
    printf("App is invalid, not good\n");
  
  DGLDisplay* display = app->display();
  if (display == NULL)
    {
      printf("Error could not load display\n");
    }
  
  //One pipe for all displays
  DGLPipe* pipe = display->getPipe(display->addPipe());

  DGLWindow* parent = NULL ;

#ifdef SIMPLE  
  {
    // first window, simple window
    DGLWindow* window = new DGLWindow("simple");
    window->setPipe(pipe);
    window->setWindowDimensions(552, 460, 512, 512) ;
    parent = window;
    //window->setSharedParent(true);
    pipe->addWindow(window);
    
    {
      // only screen/viewport in window
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      // this fov will put dpf -1, 1 at the window borders
      screen->setFov(90.0f);
      
      //Set up the viewport in the screen to view the front wall
      DGLViewport* viewport = new DGLViewport;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::PERSPECTIVE) ;
    }
  }
#endif

#ifdef SYMMETRIC
  {
    // second window, 6 symmetric frusta
    DGLWindow* window = new DGLWindow("symmetric");
    window->setPipe(pipe);
    window->setWindowDimensions(10, 10, 512, 384) ;
    //window->setSharedParentWindow(parent);
    pipe->addWindow(window);
    
    // 6 screens, one per cube face
    {
      // 1- floor
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, FLOOR_H, FLOOR_P, FLOOR_R) ;
      screen->setOffset(offset);
      screen->setFov(90.0f);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 0, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::PERSPECTIVE) ;
    }
    {
      // 2- left
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, LEFT_H, LEFT_P, LEFT_R) ;
      screen->setOffset(offset);
      screen->setFov(90.0f);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(0, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::PERSPECTIVE) ;
    }
    {
      // 3- front
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, FRONT_H, FRONT_P, FRONT_R) ;
      screen->setOffset(offset);
      screen->setFov(90.0f);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::PERSPECTIVE) ;
    }
    {
      // 4- right
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, RIGHT_H, RIGHT_P, RIGHT_R) ;
      screen->setOffset(offset);
      screen->setFov(90.0f);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(256, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::PERSPECTIVE) ;
    }
    {
      // 5- back
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, BACK_H, BACK_P, BACK_R) ;
      screen->setOffset(offset);
      screen->setFov(90.0f);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(384, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::PERSPECTIVE) ;
    }
    {
      // 6- ceiling
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, CEILING_H, CEILING_P, CEILING_R) ;
      screen->setOffset(offset);
      screen->setFov(90.0f);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 256, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::PERSPECTIVE) ;
    }
  }
#endif

#ifdef ORTHONORMAL
  {
    // second window, 6 ortho frusta
    DGLWindow* window = new DGLWindow("orthonormal");
    window->setPipe(pipe);
    window->setWindowDimensions(552, 10, 512, 384) ;
    //window->setSharedParentWindow(parent);
    pipe->addWindow(window);
    
    // 6 screens, one per cube face
    {
      // 1- floor
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, FLOOR_H, FLOOR_P, FLOOR_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 0, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::ORTHO) ;
    }
    {
      // 2- left
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, LEFT_H, LEFT_P, LEFT_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(0, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::ORTHO) ;
    }
    {
      // 3- front
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, FRONT_H, FRONT_P, FRONT_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::ORTHO) ;
    }
    {
      // 4- right
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, RIGHT_H, RIGHT_P, RIGHT_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(256, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::ORTHO) ;
    }
    {
      // 5- back
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, BACK_H, BACK_P, BACK_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(384, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::ORTHO) ;
    }
    {
      // 6- ceiling
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(0.f, 0.f, 0.f, CEILING_H, CEILING_P, CEILING_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 256, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::ORTHO) ;
    }
  }
#endif

#ifdef ASYMMETRIC
  {
    // third window, 6 symmetric frusta
    DGLWindow* window = new DGLWindow("asymmetric");
    window->setPipe(pipe);
    window->setWindowDimensions(10, 460, 512, 384) ;
    //window->setSharedParentWindow(parent);
    pipe->addWindow(window);
    
    // 6 screens, one per cube face
    {
      // 1- floor
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(FLOOR_CENTER_X, FLOOR_CENTER_Y, FLOOR_CENTER_Z,
		      FLOOR_H, FLOOR_P, FLOOR_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 0, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::SCREEN) ;
    }
    {
      // 2- left
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(LEFT_CENTER_X, LEFT_CENTER_Y, LEFT_CENTER_Z,
		      LEFT_H, LEFT_P, LEFT_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(0, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::SCREEN) ;
    }
    {
      // 3- front
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(FRONT_CENTER_X, FRONT_CENTER_Y,FRONT_CENTER_Z,
		      FRONT_H, FRONT_P, FRONT_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::SCREEN) ;
    }
    {
      // 4- right
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(RIGHT_CENTER_X, RIGHT_CENTER_Y, RIGHT_CENTER_Z,
		      RIGHT_H, RIGHT_P, RIGHT_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(256, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::SCREEN) ;
    }
    {
      // 5- back
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(BACK_CENTER_X, BACK_CENTER_Y, BACK_CENTER_Z, 
		      BACK_H, BACK_P, BACK_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(384, 128, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::SCREEN) ;
    }
    {
      // 6- ceiling
      DGLScreen* screen = new DGLScreen;
      screen->setWindow(window);
      window->addScreen(screen);
      dtkCoord offset(CEILING_CENTER_X, CEILING_CENTER_Y, CEILING_CENTER_Z,
		      CEILING_H, CEILING_P, CEILING_R) ;
      screen->setOffset(offset);
      DGLViewport* viewport = new DGLViewport;
      // viewport is x, y, w, h, in pixels
      viewport->setGeometry(128, 256, 128, 128) ;
      screen->addViewport(viewport);
      viewport->setScreen(screen);
      viewport->setViewportType(DGLViewport::SCREEN) ;
    }
  }
#endif

#ifdef DGL_ARCH_WIN32
  return DTKDSO_LOAD_CONTINUE;
#else
  return DTKDSO_LOAD_UNLOAD;
#endif
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}
