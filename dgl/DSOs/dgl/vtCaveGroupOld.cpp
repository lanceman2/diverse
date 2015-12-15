// This loads a group of DGL DSOs that are for CAVE apps in the
// Virginia Tech CAVE.

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

  DGL* app = static_cast<DGL*>(manager);

  DGLDisplay* display = app->display();
  if (display == NULL)
  {
	printf("Error could not load display\n");
  }
  //One pipe for a display
  display->addPipe(); //0
  display->addPipe(); //1
  display->addPipe(); //2
  DGLPipe* pipe = display->getPipe(2);
  DGLWindow* parent = NULL;

  //Set up the window
  DGLWindow* window = new DGLWindow("Left");
  parent = window;
  window->setPipe(pipe);
  window->setStereo(true);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(false);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(0,0, 1280,1024);
  window->setSharedParent(true);
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
  screen->setInterOccular(0.15);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);

  window = new DGLWindow("Right");
  window->setSharedChild(true);
  window->setSharedParentWindow(parent);
  window->setPipe(pipe);
  window->setStereo(true);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(false);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(1280,0, 1280,1024);
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
  screen->setInterOccular(0.15);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);


  window = new DGLWindow("Front");
  window->setSharedParent(true);
  parent = window;
  window->setPipe(pipe);
  window->setStereo(true);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(false);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(0,0, 1280,1024);
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
  screen->setInterOccular(0.15);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);

  window = new DGLWindow("Floor");
  window->setSharedChild(true);
  window->setSharedParent(window);
  parent = window;
  window->setPipe(pipe);
  window->setStereo(true);
  window->setFullScreen(false);
  window->setBorder(false);
  window->setCursor(false);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(1280,0, 1280,1024);
  pipe->addWindow(window);

  //Set up the screen within the window
  screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  offset.zero();
  offset.z=-1.0f;
  offset.p=-90.0f;
  screen->setWidth(2.0f);
  screen->setHeight(2.0f);
  screen->setInterOccular(0.15);
  screen->setOffset(offset);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);

  //Now the desktop display finally!
  pipe = display->getPipe(0);
  window = new DGLWindow("DesktopDisplay");
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
  screen->setNear(.1f);
  screen->setFar(1000.f);
  offset.zero();
  screen->setOffset(offset);
  screen->setFov(35);

  //Set up the viewport in the screen to view
  viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::PERSPECTIVE);
  return DTKDSO_LOAD_UNLOAD;
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}


