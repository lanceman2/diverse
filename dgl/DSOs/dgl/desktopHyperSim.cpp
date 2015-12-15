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
  DGLWindow* parent = NULL;

  //Set up the window
  DGLWindow* window = new DGLWindow("Left");
  parent = window;
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(200,400, 200,200);
  //window->setSharedParent(true);
  pipe->addWindow(window);

  //Set up the screen within the window
  DGLScreen* screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  dtkCoord offset;
  offset.zero();
  offset.x=-1.0f;
  offset.h=90;
  screen->setWidth(2.0f);
  screen->setHeight(2.0f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,200,200);

  window = new DGLWindow("Right");
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(600,400, 200,200);
  pipe->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  offset.zero();
  offset.x=1.0f;
  offset.h=-90;
  screen->setWidth(2.0f);
  screen->setHeight(2.0f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,200,200);

  window = new DGLWindow("Front");
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(400,400, 200,200);
  pipe->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
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
  viewport->setGeometry(0,0,200,200);

  window = new DGLWindow("Back");
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(0,400, 200,200);
  pipe->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  offset.zero();
  offset.y=-1.0f;
  offset.h=180;
  screen->setWidth(2.0f);
  screen->setHeight(2.0f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,200,200);

  window = new DGLWindow("Top");
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(400,600, 200,200);
  pipe->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  offset.zero();
  offset.z=1.0f;
  offset.p=90;
  screen->setWidth(2.0f);
  screen->setHeight(2.0f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,200,200);

  window = new DGLWindow("Bottom");
  //window->setSharedChild(true);
  //window->setSharedParentWindow(parent);
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(400,200, 200,200);
  pipe->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  offset.zero();
  offset.z=-1.0f;
  offset.p=-90;
  screen->setWidth(2.0f);
  screen->setHeight(2.0f);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,200,200);
#ifdef DTK_ARCH_DGL
  return DTKDSO_LOAD_CONTINUE;
#else
  return DTKDSO_LOAD_UNLOAD;
#endif
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}
