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

/*!
 * @class desktopDisplay
 *
 * @brief DSO to create a simple desktop display
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class desktopDisplay : public dtkAugment
{
public:
  desktopDisplay(dtkManager *manager) : 
    dtkAugment("desktopDisplay") {
    DGL::init();
    DGL* app = static_cast<DGL*>(manager); 
    
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
    if (pipe == NULL)
    	printf("Bad pipe, program will segfault\n");
    
    //Set up the window
    DGLWindow* window = new DGLWindow("DGLDesktopDisplay");
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
    DGLScreen* screen = new DGLScreen;
    screen->setWindow(window);
    window->addScreen(screen);
    screen->setNear(.0001f);
    screen->setFar(1000.f);
    dtkCoord offset;
    offset.zero();
    screen->setOffset(offset);
    screen->setFov(35);
    
  //Set up the viewport in the screen to view
    DGLViewport* viewport = new DGLViewport;
    screen->addViewport(viewport);
    viewport->setScreen(screen);
    viewport->setViewportType(DGLViewport::PERSPECTIVE);
    
    // dtkAugment::dtkAugment() will not validate the object
    validate() ;
  } ;
} ;

/************ DTK C++ dtkAugment loader/unloader functions ***************
 *
 * All DTK dtkAugment DSO files are required to declare these two
 * functions.  These function are called by the loading program to get
 * your C++ objects loaded.
 *
 *************************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
  return new desktopDisplay(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}

