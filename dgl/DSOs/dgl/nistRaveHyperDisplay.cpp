// a desktop display to help develop and debug the NIST RAVE
// John Kelso, kelso@nist.gov, 4/2006

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


// how much smaller will we be than real size?
#define SCALE (0.5f) 

// geometry of the various walls in pixels
#define HEIGHT (1024.f)
#define WIDTH (1280.f)
#define HBLANK (111.f) 
#define FLOOR_HEIGHT (990.f)


static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
  
  //DGLViewport::TYPE type = DGLViewport::PERSPECTIVE ;
  DGLViewport::TYPE type = DGLViewport::SCREEN ;

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
  
  // This is set up for the NIST RAVE, where one DPF unit = 42"
  app->setMeters(1.0688);
  
  //One pipe for a display
  DGLPipe* pipe = display->getPipe(display->addPipe());
  
  //Set up the window for all three screens
  DGLWindow* window = new DGLWindow("nistRaveHyperDisplay");
  window->setPipe(pipe);
  // make hyper display pixels half the size of the RAVE
  // RAVE- each wall is 128x1024
  window->setWindowDimensions(0, 0, int(2*WIDTH*SCALE), int(2*HEIGHT*SCALE)) ;
  pipe->addWindow(window);

  {
    //Set up the screen within the window for the front wall
    DGLScreen* screen = new DGLScreen;
    screen->setWindow(window);
    window->addScreen(screen);
    dtkCoord offset;
    offset.zero();
    if (type == DGLViewport::SCREEN) 
      {
	printf("FRUSTA == SCREEN\n") ;
	offset.y = 1.0f;
	offset.z = -.03125f ;
      }
    else
	printf("FRUSTA == PERSPECTIVE\n") ;
    screen->setOffset(offset);
    screen->setHeight(2.f*HEIGHT/(WIDTH-(HBLANK*2)));
    
    //Set up the viewport in the screen to view the front wall
    DGLViewport* viewport = new DGLViewport;
    screen->addViewport(viewport);
    viewport->setScreen(screen);
    viewport->setGeometry(int((WIDTH+HBLANK)*SCALE), int(HEIGHT*SCALE), 
			  int((WIDTH-(HBLANK*2))*SCALE), int(HEIGHT*SCALE)) ;
    viewport->setViewportType(type) ;
  }

  {
    //Set up the screen within the window for the left wall
    DGLScreen* screen = new DGLScreen;
    screen->setWindow(window);
    window->addScreen(screen);
    dtkCoord offset;
    offset.zero();
    if (type == DGLViewport::SCREEN) 
      {
	offset.x = -1.0f;
	offset.z = -.03125f ;
      }
    offset.h = 90.f ;
    screen->setOffset(offset);
    screen->setHeight(2.f*HEIGHT/(WIDTH-(HBLANK*2)));
    
    //Set up the viewport in the screen to view the left wall
    DGLViewport* viewport = new DGLViewport;
    screen->addViewport(viewport);
    viewport->setScreen(screen);
    viewport->setGeometry(int(HBLANK*SCALE), int(HEIGHT*SCALE), 
			  int((WIDTH-(HBLANK*2))*SCALE), int(HEIGHT*SCALE)) ;
    viewport->setViewportType(type) ;

 }

  {
    //Set up the screen within the window for the floor
    DGLScreen* screen = new DGLScreen;
    screen->setWindow(window);
    window->addScreen(screen);
    dtkCoord offset;
    offset.zero();
    if (type == DGLViewport::SCREEN) 
      {
	offset.z = -1.f ;
      }
    offset.h = 90.f ;
    offset.p = -90.f ;
    screen->setOffset(offset);
    screen->setHeight(2.f*FLOOR_HEIGHT/(WIDTH-(HBLANK*2))) ;
    
    //Set up the viewport in the screen to view the floor
    DGLViewport* viewport = new DGLViewport;
    screen->addViewport(viewport);
    viewport->setScreen(screen);
    viewport->setGeometry(int(HBLANK*SCALE), 0, 
			  int((WIDTH-(HBLANK*2))*SCALE), int(FLOOR_HEIGHT*SCALE)) ;
    viewport->setViewportType(type) ;

  }

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

