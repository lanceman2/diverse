#include <dtk.h>
#include <dgl.h>
#include <dtk/dtkDSO_loader.h>


/************ DTK C++ dtkAugment loader/unloader functions ***************
 *
 * All DTK dtkAugment DSO files are required to declare these two
 * functions.  These function are called by the loading program to get
 * your C++ objects loaded.
 *
 *************************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
  DGL::init();
  DGL* app = static_cast<DGL*>(manager);

#ifdef DGL_ARCH_WIN32
  if(app->load("caveDTKInput;desktopDisplay;vtWallDisplay1400x1050;wandJoystickNav;buttonNavControl"))
#else
  if(app->load("caveDTKInput:desktopDisplay:vtWallDisplay1400x1050:wandJoystickNav:buttonNavControl"))
#endif
  {
    return DTKDSO_LOAD_ERROR;
  }
  else
  {
    app->setHeadTracking(true);
    return DTKDSO_LOAD_UNLOAD;
  }
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}
