// simply sets the clear node's color
// John Kelso, NIST, kelso@nist.gov, 6/06

#include <dtk.h>
#include <dgl.h>
#include <dtk/dtkDSO_loader.h>
#include <dosg.h>

class dosgSetClearColor : public dtkAugment
{
public:
    dosgSetClearColor(dtkManager *m) ;
    virtual int postFrame(void) ;
    
private:
    float r, g, b, a ;
} ;

dosgSetClearColor::dosgSetClearColor(dtkManager *m):
    dtkAugment("dosgSetClearColor")
{

    setDescription("sets the color of the clear node") ;
    char *s ;
    
    s = getenv("DOSG_CLEAR_COLOR_R") ;
    if (s)
    {
	if (!DGLUtil::string_to_float(s, &r) && r<=1.f && r>=0.f)
	{
	    dtkMsg.add(DTKMSG_INFO,
		       "dosgSetClearColor:"
		       " setting RED = %f\n",r) ;
	}
	else
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "dosgSetClearColor failed:"
		       " invalid RED value, \"%s\"\n",s) ;
	    return ;
	    
	}
    }
    
    s = getenv("DOSG_CLEAR_COLOR_G") ;
    if (s)
    {
	if (!DGLUtil::string_to_float(s, &g) && g<=1.f && g>=0.f)
	{
	    dtkMsg.add(DTKMSG_INFO,
		       "dosgSetClearColor:"
		       " setting GREEN = %f\n",g) ;
	}
	else
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "dosgSetClearColor failed:"
		       " invalid GREEN value, \"%s\"\n",s) ;
	    return ;
	    
	}
    }
    
    s = getenv("DOSG_CLEAR_COLOR_B") ;
    if (s)
    {
	if (!DGLUtil::string_to_float(s, &b) && b<=1.f && b>=0.f)
	{
	    dtkMsg.add(DTKMSG_INFO,
		       "dosgSetClearColor:"
		       " setting BLUE = %f\n",b) ;
	}
	else
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "dosgSetClearColor failed:"
		       " invalid BLUE value, \"%s\"\n",s) ;
	    return ;
	}
    }
    
    s = getenv("DOSG_CLEAR_COLOR_A") ;
    if (s)
    {
	if (!DGLUtil::string_to_float(s, &a) && a<=1.f && a>=0.f)
	{
	    dtkMsg.add(DTKMSG_INFO,
		       "dosgSetClearColor:"
		       " setting ALPHA = %f\n",a) ;
	}
	else
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "dosgSetClearColor failed:"
		       " invalid ALPHA value, \"%s\"\n",s) ;
	    return ;
	    
	}
    }
    
    validate() ;
}


int dosgSetClearColor::postFrame(void)
{
    dtkMsg.add(DTKMSG_INFO,
	       "dosgSetClearColor::postFrame: setting clear node color to (%f, %f, %f, %f)\n",r,g,b,a) ;
  
    DOSG::getClearNode()->setClearColor(osg::Vec4(r, g, b, a)) ;
    
    return REMOVE_OBJECT ;
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
    return new dosgSetClearColor(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}
