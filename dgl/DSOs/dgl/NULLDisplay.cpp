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
 * @class NULLDisplay
 *
 * @brief DSO to avoid having to use a display
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class NULLDisplay : public dglAugment
{
public:
  NULLDisplay(dtkManager *manager) : 
    dglAugment("NULLDisplay") {
    DGL::init();
    DGL* app = static_cast<DGL*>(manager); 
    
    if (!app)
      printf("HUH\n");
    if (app->isInvalid())
      printf("App is invalid, not good\n");
    
    app->setHeadTracking(false);
    DGLDisplay* display = app->display();
    if (display == NULL)
      {
	printf("Error could not load display\n");
      }
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
  return new NULLDisplay(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}

