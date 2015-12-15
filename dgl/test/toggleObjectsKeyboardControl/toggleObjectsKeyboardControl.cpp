// reads the keyboard and sets dtk shared memory segments to control the
// toggle objects
// all this version currently does is parse the keyboard input and print the results
// John Kelso, kelso@nist.gov, 4/06
#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

class toggleObjectsKeyboardControl : public dtkAugment
{
public:
  toggleObjectsKeyboardControl(dtkManager *);
  
  int postFrame(void);
  
private:
  
  DGLKeyboard *kb ;
  
};


toggleObjectsKeyboardControl::toggleObjectsKeyboardControl(dtkManager *m) :
  dtkAugment("toggleObjectsKeyboardControl")
{
  setDescription("reads the keyboard and sets dtk shared memory segments to"
		 "control the toggleObjects") ;
  
  kb = new DGLKeyboard ;

  // dtkAugment::dtkAugment() will not validate the object
  validate() ;
}

int toggleObjectsKeyboardControl::postFrame(void)
{

  DGLKeyboard::ACTION s =  DGLKeyboard::getState(KeyChar_s) ; 
  DGLKeyboard::ACTION S =  DGLKeyboard::getState(KeyChar_S) ; 
  bool sim = (s==DGLKeyboard::PRESSED || S==DGLKeyboard::PRESSED) ;

  DGLKeyboard::ACTION e =  DGLKeyboard::getState(KeyChar_e) ; 
  DGLKeyboard::ACTION E =  DGLKeyboard::getState(KeyChar_E) ; 
  bool ether = (e==DGLKeyboard::PRESSED || E==DGLKeyboard::PRESSED) ;

  DGLKeyboard::ACTION d =  DGLKeyboard::getState(KeyChar_d) ; 
  DGLKeyboard::ACTION D =  DGLKeyboard::getState(KeyChar_D) ; 
  bool dgl = (d==DGLKeyboard::PRESSED || D==DGLKeyboard::PRESSED) ;

  DGLKeyboard::ACTION n =  DGLKeyboard::getState(KeyChar_n) ; 
  DGLKeyboard::ACTION N =  DGLKeyboard::getState(KeyChar_N) ; 
  bool nav = (n==DGLKeyboard::PRESSED || N==DGLKeyboard::PRESSED) ;

  DGLKeyboard::ACTION a ;
  KeyCharacter A = kb->readKeyCharacter(&a) ;

  // do actions caused by a button release

  if (A && a==DGLKeyboard::RELEASED)
    {
      if (A == KeyChar_BackSpace)
	{
	  printf("BackSpace- turn off all toggle objects\n") ;
	}
      else if (A == KeyChar_c || A == KeyChar_C)
	{
	  if (sim)
	    {
	      printf("Cycle Cube in Simulator coords\n") ;
	    }
	  else if (ether)
	    {
	      printf("Cycle Cube in Ether coords\n") ;
	    }
	  else if (dgl)
	    {
	      printf("Cycle Cube in DIVERSE coords\n") ;
	    }
	  else if (nav)
	    {
	      printf("Cycle Cube in Navigated world coords\n") ;
	    }
	}
      else if (A == KeyChar_f || A == KeyChar_F)
	{
	  if (sim)
	    {
	      printf("Toggle Frame in Simulator coords\n") ;
	    }
	  else if (ether)
	    {
	      printf("Toggle Frame in Ether coords\n") ;
	    }
	  else if (dgl)
	    {
	      printf("Toggle Frame in DIVERSE coords\n") ;
	    }
	  else if (nav)
	    {
	      printf("Toggle Frame in Navigated world coords\n") ;
	    }
	}
      else if (A == KeyChar_g || A == KeyChar_G)
	{
	  if (sim)
	    {
	      printf("Toggle Gmomon in Simulator coords\n") ;
	    }
	  else if (ether)
	    {
	      printf("Toggle Gmomon in Ether coords\n") ;
	    }
	  else if (dgl)
	    {
	      printf("Toggle Gmomon in DIVERSE coords\n") ;
	    }
	  else if (nav)
	    {
	      printf("Toggle Gmomon in Navigated world coords\n") ;
	    }
	}
      else if (A == KeyChar_a || A == KeyChar_A)
	{
	  if (sim)
	    {
	      printf("Toggle Axis in Simulator coords\n") ;
	    }
	  else if (ether)
	    {
	      printf("Toggle Axis in Ether coords\n") ;
	    }
	  else if (dgl)
	    {
	      printf("Toggle Axis in DIVERSE coords\n") ;
	    }
	  else if (nav)
	    {
	      printf("Toggle Axis in Navigated world coords\n") ;
	    }
	}
    }
#if 0
  if (A) 
    {
      if (a==DGLKeyboard::PRESSED)
	printf("Pressed: ") ;
      else if (a==DGLKeyboard::RELEASED)
	printf("Released: ") ;
      else
	printf("?????: ") ;
      printf("KeyCharacter = %x ",A) ;
      if (DGLKeyboard::isSpecial(A))
	printf("\n") ;
      else
	printf(", character \"%c\"\n", DGLKeyboard::getChar(A)) ;
    }
#endif

  return CONTINUE;
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
  return new toggleObjectsKeyboardControl(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}


