#define DEFAULT_SPEED 1.0f
#define SPEED_FACTOR 2.0f

// the max speed is the speed of light if the model units are meters.
#define MAX_SPEED 3.0e+8f
#define MIN_SPEED 3.0e-8f

#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

/*!
 * @class DGLkeyboardNavControl
 *
 * @brief DSO to reset and control the speed of the current navigation using
 * key presses:
 *
 * -  '+' or '='	        
 *   - increase response
 * -  '-'			
 *   - decrease response
 * -  '_'			
 *   - reset response
 * -  '0'			
 *   - reset location/scale
 * -  ')'			
 *   - reset pitch and roll
 * -  '1'			
 *   - set location/scale reset values to the current values
 * -  'p' or 'P'		
 *   - switch to next navigation DSO
 * 
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DGLkeyboardNavControl : public dtkAugment
{
public:
  DGLkeyboardNavControl(dtkManager *);
  
  int preFrame(void);
  int postFrame(void);
  
private:
  
  dtkManager *mgr ;
  dtkNavList *navList;
  DGLKeyboard *kb ;
  
  dtkCoord reset_location;
  float reset_scale;
  float reset_speed;  
};


DGLkeyboardNavControl::DGLkeyboardNavControl(dtkManager *m) :
  dtkAugment("DGLkeyboardNavControl")
{
  setDescription("adds speed and reset navigation control"
		 " to the current dtkNav navigation:"
		 " '+' or '-' = speed up or down, '0' = reset location,"
		 " '+' and '-' = reset speed, '9' = zero pitch and roll");
  mgr = m ;
  navList = NULL;
  kb = new DGLKeyboard ;
  // dtkAugment::dtkAugment() will not validate the object
  validate() ;
}

// This tries to find a reset location and scale
int DGLkeyboardNavControl::preFrame(void)
{
    #ifdef __APPLE__
     #ifdef check
      #undef check
     #endif // check
    #endif // __APPLE__
  navList = (dtkNavList *) mgr->check("dtkNavList", DTKNAVLIST_TYPE) ;
  if(!navList)
    {
      navList = new dtkNavList;
      if(mgr->add(navList))
	{
	  delete navList;
	  navList = NULL;
	  return dtkAugment::ERROR_;
	}
    }
  
  if(navList->current())
    {
      reset_location = navList->current()->location ;
      reset_scale = navList->current()->scale;
      reset_speed = navList->current()->response();
    }
  else
    {
      //reset_location.zero() ; // constructor zeros it
      reset_scale = 1.0f;
      reset_speed = DEFAULT_SPEED;
    }
  
  return dtkAugment::REMOVE_CALLBACK ;
}

int DGLkeyboardNavControl::postFrame(void)
{
  DGLKeyboard::ACTION a ;
  Producer::KeyCharacter k = kb->readKeyCharacter(&a) ;
  if (k && a==DGLKeyboard::RELEASED) 
    {
      dtkNav *currentNav = navList->current();
      if (currentNav )
	{
	  // speed up or slow down
	  if(k == Producer::KeyChar_plus || k == Producer::KeyChar_equal ||
	     k == Producer::KeyChar_minus)
	    {
	      float speed = currentNav->response();
	      if(k == Producer::KeyChar_minus) // slow down
		{
		  if(speed > MIN_SPEED)
		    {
		      speed /= SPEED_FACTOR;
		      if(speed > MIN_SPEED)
			currentNav->response(speed);
		    }
		}
	      else // key == plus
		{
		  if(speed < MAX_SPEED)
		    {
		      speed *= SPEED_FACTOR;
		      if(speed < MAX_SPEED)
			currentNav->response(speed);
		    }
		}
	      dtkMsg.add(DTKMSG_NOTICE, "DGLkeyboardNavControl::postFrame(): "
			 "set response (speed) in dtkNav named "
			 "\"%s\" to %f\n",
			 currentNav->dtkAugment::getName(),
			 currentNav->response());
	    }
	  else if(k == Producer::KeyChar_underscore) // speed reset
	    {
	      currentNav->response(reset_speed);
	      dtkMsg.add(DTKMSG_NOTICE, "DGLkeyboardNavControl::postFrame(): "
			 "reset response (speed) to %f in dtkNav named "
			 "\"%s\"\n",
			 currentNav->response(),
			 currentNav->dtkAugment::getName()); 
	    }
	  else if(k == Producer::KeyChar_0) // location reset
	    {
	      currentNav->location = reset_location ;
	      currentNav->scale = reset_scale;
	      dtkMsg.add(DTKMSG_NOTICE, "DGLkeyboardNavControl::postFrame(): "
			 "reset to initial location in "
			 "dtkNav named \"%s\"\n",
			 currentNav->dtkAugment::getName());
	    }
	  else if(k == Producer::KeyChar_parenright) // location pitch and roll reset
	    {
	      
	      currentNav->location.p = 0.f;
	      currentNav->location.r = 0.f;
	      
	      dtkMsg.add(DTKMSG_NOTICE, "DGLkeyboardNavControl::postFrame(): "
			 "set location pitch and roll to zero with "
			 "dtkNav named \"%s\"\n",
			 currentNav->dtkAugment::getName());
	    }
	  else if(k == Producer::KeyChar_1) // set reset value to current value
	    {
	      reset_location = currentNav->location ;
	      reset_scale = currentNav->scale;
	      dtkMsg.add(DTKMSG_NOTICE, "DGLkeyboardNavControl::postFrame(): "
			 "set navigation reset location "
			 "to the current location in"
			 "dtkNav named \"%s\"\n",
			 currentNav->dtkAugment::getName());
	    }
	  else if(k == Producer::KeyChar_p) // switch navigation
	    {
	      dtkNav *nav = currentNav;
	      dtkMsg.add(DTKMSG_NOTICE,"DGLKeyboardNavSwitcher::postFrame():"
			 " switching navigation from \"%s%s%s\" to ",
			 dtkMsg.color.vil, nav->getName(), dtkMsg.color.end);
	      // Go to the next naviagtion in the list.
	      float response = nav->response();
	      dtkNav *next_nav = navList->next(nav);
	      if(!next_nav)
		next_nav = navList->first();
	      if(!next_nav) {
		dtkMsg.append("\"%s%s%s\".\n",dtkMsg.color.vil,
			      "a non-existant navigation", dtkMsg.color.end);
		return ERROR_ ;
	      }
	      
	      dtkMsg.append("\"%s%s%s\".\n",dtkMsg.color.vil,
			    next_nav->getName(), dtkMsg.color.end);
	      if(next_nav != nav)
		{
		  // move to where it left off
		  next_nav->location = nav->location ;
		  next_nav->scale = nav->scale;
		  next_nav->response(response);
		  
		  // userMatrix is copied only if both are not NULL
		  dtkMatrix *up = nav->userMatrix() ;
		  dtkMatrix *nup = next_nav->userMatrix() ;
		  if (up && nup) 
		    *nup = *up ;
		  
		  // pivot is copied only if both are not NULL
		  dtkCoord *cp = nav->pivot() ;
		  dtkCoord *ncp = next_nav->pivot() ;
		  if (cp && ncp) 
		    //*ncp = *cp ;
		    next_nav->pivot(nav->pivot()) ;
		  
		  // velocity is copied only if both are not NULL
		  dtkCoord *vp = nav->velocity() ;
		  dtkCoord *nvp = next_nav->velocity() ;
		  if (vp && nvp) 
		    *nvp = *vp ;
		  
		  navList->switch_(next_nav);
		}
	    }
	}
    }
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
  return new DGLkeyboardNavControl(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}


