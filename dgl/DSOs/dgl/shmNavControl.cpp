/*!
 * @class shmNavControl
 *
 * @brief DSO which modifies the current navigation based on input from dtk
 * shared memory
 *
 *  This class reads DTK shared memory and switches the current
 *  navigation object, transferring data from one navigation to the next
 *  
 *  Data is in the shared memory segment named "dgl/shmNavControl" and consists of
 *  ASCII commands:
 *  
 *  all coordinates are in DIVERSE units
 * 
 *  this tells the DSO to stop reading from the queued memory- the next read
 *  will be the next frame 
 *  - next
 * 
 *  these turn the current navigation on and off
 *  -  activate
 *  -  deactivate
 * 
 *  these set the public variables
 *  -  set location x y z h p r
 *  -  set scale s
 *  -  set response r
 *  
 *  these replace the default variables with a local copy set to the specified
 *  value
 *  -  set velocity x y z h p r 
 *  -  set pivot x y z h p r 
 *    
 *  use the default navigation variables
 *  -  default velocity
 *  -  default pivot
 *  -  default matrix
 *    
 *  these write the current values into the shared memory segment
 *  "dgl/shmNavControlQuery"
 *
 *  -  query location
 *  -  query scale
 *  -  query response
 *  -  query velocity
 *  -  query pivot
 *  -  query matrix
 *  -  query navigation
 * 
 *  these switch between navigations:
 *  -  navigation + | next
 *  -  navigation - | previous
 *  -  navigation "name"
 * 
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */

#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

#include <string.h>
#ifndef DGL_ARCH_WIN32
#include <strings.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include <osg/Vec3>

#ifdef DGL_ARCH_WIN32
#define SHM_NAV_NAME "dgl\\shmNavControl"
#define SHM_NAV_NAME_QUERY "dgl\\shmNavControlQuery"
#else
#define SHM_NAV_NAME "dgl/shmNavControl"
#define SHM_NAV_NAME_QUERY "dgl/shmNavControlQuery"
#endif
#define SHM_NAV_SIZE (500)

static int navcpy(dtkNav *to, dtkNav *from) ;


class shmNavControl : public dtkAugment
{
public:
  shmNavControl(dtkManager *);
  
  int postFrame(void);
  
private:
  
  dtkManager *mgr ;
  dtkSharedMem *shm ;
  dtkSharedMem *shmQuery ;
  char line[SHM_NAV_SIZE] ;
  
  dtkNavList *navList ;
  
  dtkCoord nondefault_velocity ;
  dtkCoord nondefault_pivot ;
  dtkMatrix nondefault_matrix ;
  int processLine(char *) ;

};


shmNavControl::shmNavControl(dtkManager *m) :
  dtkAugment("shmNavControl")
{
  mgr = m ;
  navList = NULL ;
  nondefault_velocity.set(0.f, 0.f, 0.f, 0.f, 0.f, 0.f) ; ;
  nondefault_pivot.set(0.f, 0.f, 0.f, 0.f, 0.f, 0.f) ;
  nondefault_matrix.identity() ;
  shm = NULL ;

  // see if shared memory already exists
  dtkSharedMem *old_shm = new dtkSharedMem(SHM_NAV_NAME, 0) ;

  // it does, but is of wrong size
  if (old_shm && old_shm->isValid() && (old_shm->getSize()!=SHM_NAV_SIZE)) {
    dtkMsg.add(DTKMSG_ERROR,"shmNavControl: shared "
	       "memory \"%s\" already exists, but wrong size.\n", SHM_NAV_NAME) ;
    return ;
  }

  // it doesn't, so create a new one
  if ((!old_shm) || 
      (old_shm && old_shm->isInvalid())) { // have to create a new segment
    shm = new dtkSharedMem(SHM_NAV_SIZE, SHM_NAV_NAME) ;
    if(!shm || shm->isInvalid()) {
      dtkMsg.add(DTKMSG_ERROR,"shmNavControl: unable to get valid shared "
		 "memory \"%s\".\n", SHM_NAV_NAME) ;
      return ;
    }
  } else { // reuse old segment
    shm = old_shm ;
  }
  
  if (shm->queue())  {
    dtkMsg.add(DTKMSG_ERROR,"shmNavControl: unable to set queued read\n") ;
    return ;
  }

  shm->flush() ;
  
  if (shm->read(line)) {
    dtkMsg.add(DTKMSG_ERROR,"shmNavControl: unable to read shared "
	       "memory \"%s\".\n", SHM_NAV_NAME) ;
    return ;
  }

  shmQuery = new dtkSharedMem(SHM_NAV_SIZE, SHM_NAV_NAME_QUERY) ;
  if(!shmQuery || shmQuery->isInvalid()) {
    dtkMsg.add(DTKMSG_ERROR,"shmNavControl: unable to get valid shared "
	       "memory \"%s\".\n", SHM_NAV_NAME_QUERY) ;
    return ;
  }

  setDescription("Controls navigation via shared memory commands") ;
  
  // dtkAugment::dtkAugment() will not validate the object
  validate() ;
}

int shmNavControl::postFrame(void) 
{
  static bool first = true ;
  if (first)
    {
      first = false ;

    #ifdef __APPLE__
     #ifdef check
      #undef check
     #endif // check
    #endif // __APPLE__
      navList = (dtkNavList *) mgr->check("dtkNavList", DTKNAVLIST_TYPE) ;
      if (!navList) {
	navList = new dtkNavList ;
	if (mgr->add(navList)) {
	  delete navList ;
	  navList = NULL ;
	  return ERROR_ ;
	}
      }
  
    }

  // do this every frame
  while (memset(line, 0, SHM_NAV_SIZE), shm->qread(line) ) {
    int ret = processLine(line) ;
    if (ret != CONTINUE)
      return ret ;
  }

  return CONTINUE ;
}

int shmNavControl::processLine(char *line) {
  std::vector<std::string> args = DGLUtil::parseLine(line) ;
  int argc = args.size() ;

  float x, y, z, h, p, r ;
  dtkNav *nav = navList->current() ;

  for (unsigned int i = 0 ; i<args.size() ; i++) {
    //~printf("%d: %s\n",i,args[i].c_str()) ;
    // how many characters to match
    int l0 = args[0].length() ;
    
    if (l0<3) l0=3 ;
    if (argc==1 && (!(strncasecmp(args[0].c_str(),"next",l0)))) {
      dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		 "next\n") ;
      return CONTINUE ;
      
    } else if (argc==1 && (!(strncasecmp(args[0].c_str(),"activate",l0)))) {
      dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		 "activate\n") ;
      nav->activate() ;

    } else if (argc==1 && (!(strncasecmp(args[0].c_str(),"deactivate",l0)))) {
      dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		 "deactivate\n") ;
      nav->deactivate() ;

    /////////////////////////////////////////////////////////////////      
    // set
    } else if (argc>1 && (!(strncasecmp(args[0].c_str(),"set",l0)))) {
      int l1 = args[1].length() ;

      if (argc==8 && (!(strncasecmp(args[1].c_str(),"location",l1))) &&
	  (!DGLUtil::string_to_float(args[2].c_str(), &x)) &&
	  (!DGLUtil::string_to_float(args[3].c_str(), &y)) &&
	  (!DGLUtil::string_to_float(args[4].c_str(), &z)) &&
	  (!DGLUtil::string_to_float(args[5].c_str(), &h)) &&
	  (!DGLUtil::string_to_float(args[6].c_str(), &p)) &&
	  (!DGLUtil::string_to_float(args[7].c_str(), &r))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "set location %f %f %f %f %f %f\n",
		   x,y,z,h,p,r) ;
	nav->location.set(x,y,z,h,p,r) ;
	
      } else if (argc==3 && (!(strncasecmp(args[1].c_str(),"scale",l1))) &&
		 (!DGLUtil::string_to_float(args[2].c_str(), &x))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "set scale %f\n",x) ;
	
	float scale = x ;

	float oldScale = nav->scale ;
	//printf("\nold scale = %f\n",oldScale) ;

	//printf("new scale = %f\n",scale) ;
	nav->scale = scale ;

	dtkCoord location = nav->location ;
	//printf("old location = %f %f %f\n",location.x, location.y, location.z) ;

	dtkCoord pivot ;
	if (nav->pivot())
	{
	    pivot = *(nav->pivot()) ;
	}
	else
	{
	    dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		       "*pivot is NULL, using the origin\n") ;
	}
	//printf("pivot = %f %f %f\n",pivot.x, pivot.y, pivot.z) ;
	
	// change location so the scale change seems to be centered around the pivot point

	// how far do we slide the location away from the pivot?
	float slide = (scale/oldScale -1.f) ;
	//printf("slide = %f\n",slide) ;

	// delta between pivot and location
	dtkCoord delta;
	delta.x = pivot.x - location.x ;
	delta.y = pivot.y - location.y ;
	delta.z = pivot.z - location.z ;
	//printf("delta =  %f %f %f\n",delta.x, delta.y, delta.z) ;

	location.x -= delta.x*slide ;
	location.y -= delta.y*slide ;
	location.z -= delta.z*slide ;

	//printf("new location =  %f %f %f\n",location.x, location.y, location.z) ;
	nav->location = location ;


      } else if (argc==3 && (!(strncasecmp(args[1].c_str(),"response",l1))) &&
		 (!DGLUtil::string_to_float(args[2].c_str(), &x))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "set response %f\n",x) ;
	nav->response(x) ;

      } else if (argc==8 && (!(strncasecmp(args[1].c_str(),"velocity",l1))) &&
	  (!DGLUtil::string_to_float(args[2].c_str(), &x)) &&
	  (!DGLUtil::string_to_float(args[3].c_str(), &y)) &&
	  (!DGLUtil::string_to_float(args[4].c_str(), &z)) &&
	  (!DGLUtil::string_to_float(args[5].c_str(), &h)) &&
	  (!DGLUtil::string_to_float(args[6].c_str(), &p)) &&
	  (!DGLUtil::string_to_float(args[7].c_str(), &r))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "set velocity %f %f %f %f %f %f\n",x,y,z,h,p,r) ;
	nondefault_velocity.set(x,y,z,h,p,r) ;
	nav->velocity(&nondefault_velocity) ;

      } else if (argc==8 && (!(strncasecmp(args[1].c_str(),"pivot",l1))) &&
	  (!DGLUtil::string_to_float(args[2].c_str(), &x)) &&
	  (!DGLUtil::string_to_float(args[3].c_str(), &y)) &&
	  (!DGLUtil::string_to_float(args[4].c_str(), &z)) &&
	  (!DGLUtil::string_to_float(args[5].c_str(), &h)) &&
	  (!DGLUtil::string_to_float(args[6].c_str(), &p)) &&
	  (!DGLUtil::string_to_float(args[7].c_str(), &r))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "set pivot %f %f %f %f %f %f\n",x,y,z,h,p,r) ;
	nondefault_pivot.set(x,y,z,h,p,r) ;
	nav->pivot(&nondefault_pivot) ;

      }
      
    /////////////////////////////////////////////////////////////////      
    // default
    } else if (argc>1 && (!(strncasecmp(args[0].c_str(),"default",l0)))) {
      int l1 = args[1].length() ;
      
      if (argc==2 && (!(strncasecmp(args[1].c_str(),"velocity",l1)))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "default velocity\n") ;
	nav->velocity(NULL) ;
	
      } else if (argc==2 && (!(strncasecmp(args[1].c_str(),"pivot",l1)))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "default pivot\n") ;
	nav->pivot(NULL) ;
	
      } else if (argc==2 && (!(strncasecmp(args[1].c_str(),"matrix",l1)))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "default matrix\n") ;
	nav->userMatrix(NULL) ;
	
      } else {
	dtkMsg.add(DTKMSG_ERROR,"shmNavControl: unrecognized command: %s\n",
		   line) ;
      }
      
    /////////////////////////////////////////////////////////////////      
    // query
    } else if (argc==2 && (!(strncasecmp(args[0].c_str(),"query",l0)))) {
      int l1 = args[1].length() ;
      char response[SHM_NAV_SIZE] ;

      if (!(strncasecmp(args[1].c_str(),"location",l1))) {
	sprintf(response,"location %f %f %f %f %f %f",
		nav->location.x, nav->location.y, nav->location.z, 
		nav->location.h, nav->location.p, nav->location.r) ;

      } else if (!(strncasecmp(args[1].c_str(),"scale",l1))) {
	sprintf(response,"scale %f",  nav->scale) ;

      } else if (!(strncasecmp(args[1].c_str(),"response",l1))) {
	sprintf(response,"response %f",  nav->response()) ;

      } else if (!(strncasecmp(args[1].c_str(),"velocity",l1))) {
	sprintf(response,"velocity %f %f %f %f %f %f",
		nav->velocity()->x, nav->velocity()->y, nav->velocity()->z, 
		nav->velocity()->h, nav->velocity()->p, nav->velocity()->r) ;

      } else if (!(strncasecmp(args[1].c_str(),"pivot",l1))) {
	sprintf(response,"pivot %f %f %f %f %f %f",
		nav->pivot()->x, nav->pivot()->y, nav->pivot()->z, 
		nav->pivot()->h, nav->pivot()->p, nav->pivot()->r) ;

      } else if (!(strncasecmp(args[1].c_str(),"matrix",l1))) {
	sprintf(response,"matrix %f %f %f %f " 
		                "%f %f %f %f "
		                "%f %f %f %f "
		                "%f %f %f %f",
		nav->matrix()->element(0,0), nav->matrix()->element(0,1), 
		nav->matrix()->element(0,2), nav->matrix()->element(0,3), 
		nav->matrix()->element(1,0), nav->matrix()->element(1,1), 
		nav->matrix()->element(1,2), nav->matrix()->element(1,3), 
		nav->matrix()->element(2,0), nav->matrix()->element(2,1), 
		nav->matrix()->element(2,2), nav->matrix()->element(2,3), 
		nav->matrix()->element(3,0), nav->matrix()->element(3,1), 
		nav->matrix()->element(3,2), nav->matrix()->element(3,3)) ;

      } else if (!(strncasecmp(args[1].c_str(),"navigation",l1))) {
	sprintf(response,"navigation %s",nav->getName()) ;

      } else {
	dtkMsg.add(DTKMSG_ERROR,"shmNavControl: unrecognized command: %s\n",
		   line) ;
	return CONTINUE ;
      }
      dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		 "query response = \"%s\"\n", response) ;
      shmQuery->write(response) ; 
      
    /////////////////////////////////////////////////////////////////      
    // navigation
    } else if (argc==2 && (!(strncasecmp(args[0].c_str(),"navigation",l0)))) {
      int l1 = args[1].length() ;
      if (!(strncasecmp(args[1].c_str(),"+",l1)) ||
	  !(strncasecmp(args[1].c_str(),"next",l1))) {
	dtkNav *next_nav = navList->next(nav);
	if (!next_nav)
	  next_nav = navList->first();
                                	if(next_nav != nav) {
	  navcpy(next_nav, nav) ;
	  navList->switch_(next_nav);
	  dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		     "navigation next, switching to %s\n",
		     next_nav->getName()) ;
	}

      } else if (!(strncasecmp(args[1].c_str(),"-",l1)) ||
	  !(strncasecmp(args[1].c_str(),"previous",l1))) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "navigation previous\n") ;
	dtkNav *prev_nav = NULL ;
	dtkNav *next_nav = nav ;
	do {
	  prev_nav = next_nav ;
	  next_nav = navList->next(prev_nav);
	  if (!next_nav)
	    next_nav = navList->first();
	} while (next_nav != nav) ;
	if(prev_nav != nav) {
	  navcpy(prev_nav, nav) ;
	  navList->switch_(prev_nav);
	}
	
      } else if (strcasecmp(args[1].c_str(), nav->getName())) {
	dtkMsg.add(DTKMSG_INFO, "shmNavControl: "
		   "navigation \"%s\"\n",args[1].c_str()) ;
	dtkNav *next_nav = nav ;
	do {
	  next_nav = navList->next(next_nav);
	  if (!next_nav)
	    next_nav = navList->first();
	  if (!strcasecmp(args[1].c_str(),next_nav->getName())) {
	    navcpy(next_nav, nav) ;
	    navList->switch_(next_nav);
	    return CONTINUE ;
	  }
	} while (nav != next_nav) ;
      }

    /////////////////////////////////////////////////////////////////      
    // huh??
    } else {
      dtkMsg.add(DTKMSG_ERROR,"shmNavControl: unrecognized command: %s\n",
		 line) ;
    }
    return CONTINUE ;
  } // for (int i = 0 ; i<argc ; i++) {
  return CONTINUE ;
}

static int navcpy(dtkNav *to, dtkNav *from) {
  if (!to || !from)
    return -1 ;
  to->location = from->location ;
  to->scale = from->scale ;
  to->response(from->response()) ;

  // userMatrix is copied only if current is not NULL
  dtkMatrix *up = from->userMatrix() ;
  if (!up) 
    to->userMatrix(up) ;
  
  // pivot is copied only if current pivot isn't the default
  // get current pivot pointer
  dtkCoord *cp = from->pivot() ;
  // set pivot to default
  from->pivot(NULL) ;
  // get current pivot back out
  dtkCoord *defcp = from->pivot() ;
  // is current not the same as the default?
  if (cp != defcp) {
    // set next to same non-default value
    to->pivot(cp) ;
  }
  
  // velocity is copied only if current velocity isn't the default
  // get current velocity pointer
  dtkCoord *vp = from->velocity() ;
  // set velocity to default
  from->velocity(NULL) ;
  // get current velocity back out
  dtkCoord *defvp = from->velocity() ;
  // is current not the same as the default?
  if (vp != defvp) {
    // set next to same non-default value
    to->velocity(vp) ;
  }
  
  to->preFrame();

  return 0 ;
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
  return new shmNavControl(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}


