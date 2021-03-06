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
  DGLWindow* window = new DGLWindow("DGL Ortho Display");
  window->setPipe(pipe);
  window->setStereo(true);
  window->setFullScreen(false);
  window->setBorder(true);
  window->setCursor(true);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(0,0, 640,480);
  pipe->addWindow(window);

  //Set up the screen within the window
  DGLScreen* screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  dtkCoord offset;
  offset.zero();
  offset.y=1.0f;
  screen->setWidth(1.0f);
  screen->setHeight(1.0f);
  screen->setOffset(offset);
  screen->setInterOccular(0.15);

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::ORTHO);

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
