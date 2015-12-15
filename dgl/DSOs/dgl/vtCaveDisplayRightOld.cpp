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

  app->setMeters(1.524f);

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
  window->setStereo(true);
  window->setFullScreen(true);
  window->setBorder(false);
  window->setCursor(false);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(0,0, 1280,1024);
  pipe->addWindow(window);

  //Set up the screen within the window
  DGLScreen* screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  screen->setInterOccular(0.03);
  dtkCoord offset;
  offset.zero();
  offset.x=1.0f;
  offset.h=-90;
  screen->setWidth(2.0f);
  screen->setHeight(2.0f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,1280,1024);

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
