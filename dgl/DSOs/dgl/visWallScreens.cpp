#include <dtk.h>
#include <dgl.h>
#include <dtk/dtkDSO_loader.h>

// default DSO to set a simple display graphics state

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
  //One pipe for a display
  display->addDisplayScreenPipe(0,1);
  display->addDisplayScreenPipe(0,2);
  DGLPipe* left_wall = display->getDisplayScreenPipe(0,1);
  DGLPipe* front_wall = display->getDisplayScreenPipe(0,2);

  //Set up the window
  DGLWindow* window = new DGLWindow("DGL Screen Display");
  window->setPipe(front_wall);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(0,0, 1400,1050);
  //window->setSharedParent(true);
  front_wall->addWindow(window);
  //DGLWindow* parent = window;

  //Set up the screen within the window
  DGLScreen* screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.01f);
  screen->setFar(1000.f);
  screen->setInterOccular(0.05);
  dtkCoord offset;
  offset.zero();
  offset.y=1.0f;
  screen->setWidth(2.0f);
  screen->setHeight(1.5f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setGeometry(0,0,1400,1050);
  viewport->setViewportType(DGLViewport::SCREEN);

  window = new DGLWindow("DGL Screen Display2");
  window->setPipe(front_wall);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(1400,0, 1400,1050);
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  front_wall->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.01f);
  screen->setFar(1000.f);
  screen->setInterOccular(0.05);
  offset.zero();
  offset.y=1.0f;
  screen->setWidth(2.0f);
  screen->setHeight(1.5f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setGeometry(0,0,1400,1050);
  viewport->setViewportType(DGLViewport::SCREEN);

  window = new DGLWindow("DGL Screen Display3");
  window->setPipe(left_wall);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(0,0, 1400,1050);
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  left_wall->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.01f);
  screen->setFar(1000.f);
  screen->setInterOccular(0.05);
  offset.zero();
  offset.x=-1.0f;
  offset.h=90;
  screen->setWidth(2.0f);
  screen->setHeight(1.5f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setGeometry(0,0,1400,1050);
  viewport->setViewportType(DGLViewport::SCREEN);

  window = new DGLWindow("DGL Screen Display4");
  window->setPipe(left_wall);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(1400,0, 1400,1050);
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  left_wall->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.01f);
  screen->setFar(1000.f);
  screen->setInterOccular(0.05);
  offset.zero();
  offset.x=-1.0f;
  offset.h=90;
  screen->setWidth(2.0f);
  screen->setHeight(1.5f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setGeometry(0,0,1400,1050);
  viewport->setViewportType(DGLViewport::SCREEN);
/*
#define DEBUG 
    #ifdef DEBUG
    display->addDisplayScreenPipe(0,0);
    DGLPipe* pipe = display->getDisplayScreenPipe(0,0);
    
    //Set up the window
    window = new DGLWindow("DGLDesktopDisplay");
    window->setPipe(pipe);
    window->setStereo(false);
    window->setFullScreen(false);
    window->setBorder(true);
    window->setCursor(true);
    window->setResizeable(true);
    window->setDepth(24);
    window->setWindowDimensions(0,0, 640,480);
    pipe->addWindow(window);
    
    //Set up the screen within the window
    screen = new DGLScreen;
    screen->setWindow(window);
    window->addScreen(screen);
    screen->setNear(.01f);
    screen->setFar(1000.f);
    offset.zero();

    offset.y=1.0f;
    screen->setWidth(2.0f);
    screen->setHeight(2.0f);
    screen->setOffset(offset);
    
  //Set up the viewport in the screen to view
    viewport = new DGLViewport;
    screen->addViewport(viewport);
    viewport->setScreen(screen);
    viewport->setViewportType(DGLViewport::SCREEN);
    #endif
*/
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

