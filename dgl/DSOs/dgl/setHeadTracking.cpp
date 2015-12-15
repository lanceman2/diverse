#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

/*!
 * @class setHeadTracking
 *
 * @brief DSO which sets DGL::setHeadTracking(true)
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class setHeadTracking : public dtkAugment
{
public:
  setHeadTracking(dtkManager *);
  int postConfig(void);
private:
};

setHeadTracking::setHeadTracking(dtkManager *manager) :
  dtkAugment("setHeadTracking")
{
  validate() ;
}

int setHeadTracking::postConfig(void)
{
  DGL::getApp()->setHeadTracking(true);
  return DTKDSO_UNLOAD_CONTINUE;
}

/************ DTK C++ dtkAugment loader/unloader functions ***************
 *
 * All DTK dtkAugment DSO files are required to declare these two
 * functions.  These function are called by the loading program to get
 * your C++ objects loaded.
 *
 *************************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
  return new setHeadTracking(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}

