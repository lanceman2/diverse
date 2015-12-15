#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>


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
  DGL* app = static_cast<DGL*>(manager); 
  app->setHeadTracking(false);
  
#ifdef DGL_ARCH_WIN32
  if(app->load("DGLkeyboardNavControl;trackballNav;desktopHoverNav;desktopFlyNav"))
#else
  if(app->load("DGLkeyboardNavControl:trackballNav:desktopHoverNav:desktopFlyNav"))
#endif
  {
    return DTKDSO_LOAD_ERROR;
  }
  else
  {
    return DTKDSO_LOAD_UNLOAD;
  }
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}
