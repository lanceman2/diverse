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
	display->addDisplayScreenPipe(0,0);
  display->addDisplayScreenPipe(0,1);
  //One pipe for a display
  DGLPipe* top = display->getDisplayScreenPipe(0,0);
  top->setPCIBusID( 6 );
  top->setPCIDeviceID( 0 );
  DGLPipe* bottom = display->getDisplayScreenPipe(0,1);
  top->setPCIBusID( 5 );
  top->setPCIDeviceID( 0 );

  //Set up the window
  DGLWindow* window = new DGLWindow("DGL Screen Display - top");
  window->setPipe(top);
  window->setStereo(true);
  window->setFullScreen(true);
  window->setBorder(false);
  window->setCursor(false);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(0,0,1920,1080);
  top->addWindow(window);

  //Set up the screen within the window
  DGLScreen* screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(0.01f);
  screen->setFar(1000.f);
  dtkCoord offset;
  offset.zero();
  offset.x=-1.0f;
  offset.z=-0.4375;
  offset.h=90;
  screen->setWidth(2.0f);
  screen->setHeight(1.125f);
  screen->setOffset(offset);
  screen->setInterOccular(0.03);

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,1920,1080);

  //Set up the window
  window = new DGLWindow("DGL Screen Display - bottom");
  window->setPipe(bottom);
  window->setStereo(true);
  window->setFullScreen(true);
  window->setBorder(false);
  window->setCursor(false);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(0,0,1920,1080);
  bottom->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(0.01f);
  screen->setFar(1000.f);
  offset.zero();
  offset.x=-1.0f;
  offset.z=0.4375;
  offset.h=90;
  screen->setWidth(2.0f);
  screen->setHeight(1.125f);
  screen->setOffset(offset);
  screen->setInterOccular(0.03);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,1920,1080);

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
