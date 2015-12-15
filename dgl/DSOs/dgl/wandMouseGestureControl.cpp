

// John Kelso, kelso@nist.gov, 5/06

#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

#ifdef DGL_ARCH_WIN32
#  include <float.h>
#  define MAXFLOAT FLT_MAX
#elif !defined DGL_ARCH_DARWIN
#  include <values.h>
#endif

#define MIN_TIME (.5f) 
#define TOGGLE_ROLL_DEFLECTION (20) 
#define TOGGLE_PITCH_DEFLECTION (20) 

/*!
 * @class wandMouseGestureControl 
 *
 * @brief DSO which toggles dtk shared memory based on a wand gesture- used by
 * wandMouse and dosgWandMouse
 *
 * The gesture is that the wand is rolled upside down for at least a second,
 * while in a relatively horizontal position
 *
 * The shared memory file is named "dgl/wandMouse", a single byte
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class wandMouseGestureControl : public dtkAugment
{
 public:
  wandMouseGestureControl(dtkManager *m) ;
  virtual int postFrame(void) ;

private:
  dtkManager   *manager;
  dtkSharedMem *wandMouse_shm ;
  // true == wandMouse active
  bool          wandMouseState ;
  dtkInLocator *wand_loc ;
  float         wand_xyzhpr[6] ;
  int           roll, pitch ;
  // dtkTime is a gettimeofday() wrapper
  dtkTime t;
  float acc_t ;
} ;

wandMouseGestureControl::wandMouseGestureControl(dtkManager *m):
   dtkAugment("wandMouseGestureControl")
{

  setDescription("activates/deactivates wandMouse using wand gesture") ;
  
  manager = (dtkManager*) DGL::getApp();
  if (manager == NULL || manager->isInvalid())
    {
      printf("wandMouseGestureControl::wandMouseGestureControl: Bad manager :(\n");
      return;
    }
  wandMouseState = false ;
  validate() ;
}

int wandMouseGestureControl::postFrame(void) 
{
  static bool first = true ;

  if (first) 
    {
      first = false ;
      t.reset(0.0,1);
      acc_t = -MAXFLOAT ;

    #ifdef __APPLE__
     #ifdef check
      #undef check
     #endif // check
    #endif // __APPLE__
      if (!(wand_loc = (dtkInLocator*)manager->check("wand")))
	{
	  wand_loc = new dtkInLocator(manager->record(), "wand");
	  if (!wand_loc || wand_loc->isInvalid())
	    dtkMsg.add(DTKMSG_ERROR,"wandMouseGestureControl::wandMouseGestureControl: failed to get dtkInLocator(\"wand\").\n");
	  return ERROR_;
	}
      wandMouse_shm =  new dtkSharedMem(1, "dgl/wandMouse", 
					&wandMouseState) ; 
      if (!wandMouse_shm || wandMouse_shm->isInvalid()) 
	{
	  dtkMsg.add(DTKMSG_ERROR,
		     " wandMouseGestureControl::postFrame() failed:"
		     " couldn't open shared memory segment\"dgl/wandMouse\"\n") ;
	  return ERROR_ ;
	}
      
    }
  
  // do this every frame

  wand_loc->read(wand_xyzhpr) ;
  //printf("%s %d: wand hpr = %f %f %f\n",__FILE__,__LINE__,wand_xyzhpr[3], wand_xyzhpr[4], wand_xyzhpr[5]) ; 

  pitch = int(wand_xyzhpr[4])%360 ;
  if (pitch<0)
    pitch += 360 ;

  // ignore if pitch is too low or too high
  if (!(pitch < TOGGLE_PITCH_DEFLECTION ||
	pitch > 360-TOGGLE_PITCH_DEFLECTION))
    return  dtkAugment::CONTINUE ;
  
  roll = int(wand_xyzhpr[5])%360 ;
  if (roll<0)
    roll += 360 ;

  acc_t += float(t.delta()) ;

  // true if in flip gesture zone
  if (roll < 180+TOGGLE_ROLL_DEFLECTION &&
      roll > 180-TOGGLE_ROLL_DEFLECTION)
    {
      if (acc_t> MIN_TIME) // have we been in zone long enough to flip?
	{
	  acc_t = -MAXFLOAT ;
	  wandMouseState = !wandMouseState ; 
	  wandMouse_shm->write(&wandMouseState) ;
	  printf("wandMouseGestureControl::postFrame: setting state to %d\n",wandMouseState) ;
	}
    }
  else
    acc_t = 0.f ;
 
  return  dtkAugment::CONTINUE; 
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
  return new wandMouseGestureControl(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}



