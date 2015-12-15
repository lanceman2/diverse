// simply sets the channel clear color
// John Kelso, NIST, kelso@nist.gov, 6/06

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
  DGL::init();

  float r, g, b, a ;
  a=1.f;
  DGL::getApp()->getClearColor(r, g, b, a) ;
  char *s ;

  s = getenv("DGL_CLEAR_COLOR_R") ;
  if (s)
    {
      if (!DGLUtil::string_to_float(s, &r) && r<=1.f && r>=0.f)
	{
	  dtkMsg.add(DTKMSG_INFO,
		     "setClearColor:"
		     " setting RED = %f\n",r) ;
	}
      else
	{
	  dtkMsg.add(DTKMSG_ERROR,
		     "setClearColor failed:"
		     " invalid RED value, \"%s\"\n",s) ;
	  return NULL ;
	  
	}
    }
  
  s = getenv("DGL_CLEAR_COLOR_G") ;
  if (s)
    {
      if (!DGLUtil::string_to_float(s, &g) && g<=1.f && g>=0.f)
	{
	  dtkMsg.add(DTKMSG_INFO,
		     "setClearColor:"
		     " setting GREEN = %f\n",g) ;
	}
      else
	{
	  dtkMsg.add(DTKMSG_ERROR,
		     "setClearColor failed:"
		     " invalid GREEN value, \"%s\"\n",s) ;
	  return NULL ;
	  
	}
    }

  s = getenv("DGL_CLEAR_COLOR_B") ;
  if (s)
    {
      if (!DGLUtil::string_to_float(s, &b) && b<=1.f && b>=0.f)
	{
	  dtkMsg.add(DTKMSG_INFO,
		     "setClearColor:"
		     " setting BLUE = %f\n",b) ;
	}
      else
	{
	  dtkMsg.add(DTKMSG_ERROR,
		     "setClearColor failed:"
		     " invalid BLUE value, \"%s\"\n",s) ;
	  return NULL ;
	}
    }

  s = getenv("DGL_CLEAR_COLOR_A") ;
  if (s)
    {
      if (!DGLUtil::string_to_float(s, &a) && a<=1.f && a>=0.f)
	{
	  dtkMsg.add(DTKMSG_INFO,
		     "setClearColor:"
		     " setting ALPHA = %f\n",a) ;
	}
      else
	{
	  dtkMsg.add(DTKMSG_ERROR,
		     "setClearColor failed:"
		     " invalid ALPHA value, \"%s\"\n",s) ;
	  return NULL ;
	  
	}
    }

  dtkMsg.add(DTKMSG_INFO,
	     "setClearColor(%f, %f, %f, %f)\n",r,g,b,a) ;
  DGL::getApp()->setClearColor(r, g, b, a) ;
  return DTKDSO_LOAD_UNLOAD;
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}
