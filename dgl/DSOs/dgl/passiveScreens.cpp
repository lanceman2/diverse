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
  DGLPipe* pipe = display->getPipe(display->addPipe());

  //Set up the window
  DGLWindow* window = new DGLWindow("DGL Screen Display");
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(0,0, 1280,1024);
  //window->setSharedParent(true);
  pipe->addWindow(window);
  //DGLWindow* parent = window;

  //Set up the screen within the window
  DGLScreen* screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  screen->setInterOccular(0.04175);
  dtkCoord offset;
  offset.zero();
  offset.y=1.0f;
  screen->setWidth(1.0f);
  screen->setHeight(1.0f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setGeometry(0,0,1280,1024);
  viewport->setViewportType(DGLViewport::SCREEN);

  window = new DGLWindow("DGL Screen Display2");
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(1280,0, 1280,1024);
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  pipe->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  screen->setInterOccular(0.04175);
  offset.zero();
  offset.y=1.0f;
  screen->setWidth(1.0f);
  screen->setHeight(1.0f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setGeometry(0,0,1280,1024);
  viewport->setViewportType(DGLViewport::SCREEN);

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
