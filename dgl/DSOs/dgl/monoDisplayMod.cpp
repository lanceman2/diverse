#include <dgl.h>
#include <GL/glu.h>
#include <dtk/dtkDSO_loader.h>

class monoDisplayMod : public dtkAugment
{
public:
    monoDisplayMod(dtkManager *manager) : 
	dtkAugment("monoDisplayMod")
    {
	setDescription("sets windows to mono.  it does them all if you put it last, or only the ones so far created if you putit somewhere else\n") ;
	
	// dtkAugment::dtkAugment() will not validate the object

	validate() ;
	DGLDisplay *d = DGL::getApp()->display() ;
	for (int pn=0; pn<d->getNumPipes(); pn++)
	{
	    DGLPipe *p = d->getPipe(pn) ;
	    for (int wn=0; wn<p->getNumWindows(); wn++)
	    {
		DGLWindow *w = p->getWindow(wn) ;
		w->setStereo(false) ;
	    }
	}
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
    return new monoDisplayMod(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}

