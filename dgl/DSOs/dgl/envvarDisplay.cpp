// a desktop display that 
// lets you set most parameters with envvars
// John Kelso, kelso@nist.gov, 4/2006

// added envvars for overriding defaults- kelso 3/05
// ENVVAR_DISPLAY_FOV sets screen->fov()
// ENVVAR_DISPLAY_METERS sets display->meters()
// ENVVAR_DISPLAY_ORIGIN_X sets pwin->setOriginSize()
// ENVVAR_DISPLAY_ORIGIN_Y sets pwin->setOriginSize()
// ENVVAR_DISPLAY_SIZE_X sets pwin->setOriginSize()
// ENVVAR_DISPLAY_SIZE_Y sets pwin->setOriginSize()
// ENVVAR_DISPLAY_NAME sets pwin->name()
// ENVVAR_DISPLAY_ASPECT sets pwin->aspect()- set to vertical if set to "v" or "V"

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
  
  float f ;
  if (!(DGLUtil::string_to_float(getenv("ENVVAR_DISPLAY_METERS"),&f))) 
    {
      dtkMsg.add(DTKMSG_INFO,
		 "envvarDisplay: setting meters to %f\n",f) ;
      app->setMeters(f);
    } 
  else
    // This is set up for the NIST RAVE, where one DPF unit = 42"
    app->setMeters(1.0688);
  
  //One pipe for a display
  DGLPipe* pipe = display->getPipe(display->addPipe());
  
  int xo, yo, xs, ys ;
  if (!(DGLUtil::string_to_float(getenv("ENVVAR_DISPLAY_ORIGIN_X"),&f)))
    {
      dtkMsg.add(DTKMSG_INFO,
		 "envvarDisplay: setting x origin to %d\n",(int) f) ;
      xo= (int) f ;
    } 
  else
    xo=300 ;
  
  if (!(DGLUtil::string_to_float(getenv("ENVVAR_DISPLAY_ORIGIN_Y"),&f))) 
    {
      dtkMsg.add(DTKMSG_INFO,
		 "envvarDisplay: setting y origin to %d\n",(int) f) ;
      yo= (int) f ;
    } 
  else
    yo=200 ;
  
  if (!(DGLUtil::string_to_float(getenv("ENVVAR_DISPLAY_SIZE_X"),&f))) 
    {
      dtkMsg.add(DTKMSG_INFO,
		 "envvarDisplay: setting x size to %d\n",(int) f) ;
      xs= (int) f ;
    } 
  else
    xs=700 ;
  
  if (!(DGLUtil::string_to_float(getenv("ENVVAR_DISPLAY_SIZE_Y"),&f))) 
    {
      dtkMsg.add(DTKMSG_INFO,
		 "envvarDisplay: setting y size to %d\n",(int) f) ;
      ys= (int) f ;
    } 
  else
    ys=700 ;
  
  char *name = getenv("ENVVAR_DISPLAY_NAME") ;
  if (name) 
    {
      dtkMsg.add(DTKMSG_INFO,
		 "envvarDisplay: setting name to \"%s\"\n",name) ;
    } 
  else
    name=strdup("ENVVAR Display") ;
  
  
  //Set up the window
  DGLWindow* window = new DGLWindow(name);
  window->setPipe(pipe);
  window->setStereo(false);
  window->setFullScreen(false);
  window->setBorder(true);
  window->setCursor(true);
  window->setResizeable(true);
  window->setDepth(24);
  window->setWindowDimensions(xo, yo, xs, ys) ;
  pipe->addWindow(window);
  
  //Set up the screen within the window
  DGLScreen* screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.1f);
  screen->setFar(1000.f);
  dtkCoord offset;
  offset.zero();
  screen->setOffset(offset);
  if (!(DGLUtil::string_to_float(getenv("ENVVAR_DISPLAY_FOV"),&f))) 
    {
      dtkMsg.add(DTKMSG_INFO,
		 "envvarDisplay: setting fov to %f\n",f) ;
      screen->setFov(f);
    } 
  else
    screen->setFov(60.0f);
  
  if (getenv("ENVVAR_DISPLAY_ASPECT"))
    {
      if (!strncasecmp(getenv("ENVVAR_DISPLAY_ASPECT"), "v", 1)) 
	{
	  dtkMsg.add(DTKMSG_INFO,
		     "envvarDisplay: setting aspect to \"DGLScreen::VERTICAL\"\n") ;
	  screen->setAutoAspect(DGLScreen::VERTICAL) ;
	} 
      else if (!strncasecmp(getenv("ENVVAR_DISPLAY_ASPECT"), "h", 1))  
	{
	  dtkMsg.add(DTKMSG_INFO,
		     "envvarDisplay: setting aspect to \"DGLScreen::HORIZONTAL\"\n") ;
	  screen->setAutoAspect(DGLScreen::HORIZONTAL) ;
	} 
      else if (!strncasecmp(getenv("ENVVAR_DISPLAY_ASPECT"), "n", 1))  
	{
	  dtkMsg.add(DTKMSG_INFO,
		     "envvarDisplay: setting aspect to \"DGLScreen::NONE\"\n") ;
	  screen->setAutoAspect(DGLScreen::NONE) ;
	} 
    }

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::PERSPECTIVE);

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

