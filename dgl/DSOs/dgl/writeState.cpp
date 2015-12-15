#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

/*!
 * @class  writeState
 *
 * @brief DSO which writes dgl state data to shared memory every frame, if
 * changed
 *
 * The shared memory files are in the "dgl/state" subdirectory of $DTK_SHAREDMEM_DIR 
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class writeState : public dtkAugment
{
public:
  writeState(dtkManager *);
  virtual ~writeState(void) ;
  virtual int postFrame(void) ;

private:
  DGL *dgl ;

  dtkSharedMem *new_shm(unsigned int size, char *name, void *init) ;

  dtkSharedMem *frame_shm ;
  unsigned int frame ;

  dtkSharedMem *time_shm ;
  dtkTime time ;
  float time_elapsed ;

  dtkSharedMem *origin_shm ;
  dtkCoord origin, old_origin ;
  float origin_array[6] ;
  dtkSharedMem *scale_shm ;
  float scale, old_scale ;
  dtkSharedMem *meters_shm ;
  float meters, old_meters ;
  
  dtkNavList *navList ;
  dtkNav *nav ;
  dtkSharedMem *location_shm ;
  dtkCoord location, old_location ;
  float location_array[6] ;
  dtkSharedMem *pivot_shm ;
  dtkCoord pivot, old_pivot ;
  float pivot_array[6] ;
  dtkSharedMem *response_shm ;
  float response, old_response ;
  dtkSharedMem *navScale_shm ;
  float navScale, old_navScale ;
  dtkSharedMem *navName_shm ;
  char navName[100], old_navName[100] ;
};


writeState::writeState(dtkManager *m) :
  dtkAugment("writeState")
{
  setDescription("writes dgl state data to shared memory every frame, if changed\n") ;
  dgl = DGL::getApp() ;

  navList = NULL ;
  nav = NULL ;

  validate() ;
}

writeState::~writeState(void) 
{
  printf("%s %d\n",__FILE__,__LINE__) ;
  frame = 0 ;
  frame_shm->write(&frame) ;
}  

int writeState::postFrame(void)
{

  static bool first = true ;
  if (first)
    {
      first = false ;

      ////////////////
      time.reset(0.0,1);
      time_elapsed = 0.f ;
#ifdef DGL_ARCH_WIN32
      time_shm =  new_shm(sizeof(float), "dgl\\state\\time", &time_elapsed) ; 
#else
      time_shm =  new_shm(sizeof(float), "dgl/state/time", &time_elapsed) ; 
#endif

      ////////////////
      frame = 0 ;
#ifdef DGL_ARCH_WIN32
      frame_shm =  new_shm(sizeof(unsigned int), "dgl\\state\\frame", &frame) ; 
#else
      frame_shm =  new_shm(sizeof(unsigned int), "dgl/state/frame", &frame) ; 
#endif

      ////////////////
       old_origin = origin = dgl->getOrigin() ;

      // invert before writing out to make up for it getting flipped in DOSG,cpp
      dtkMatrix m = dtkMatrix(origin);
      m.invert();
      m.coord(&origin);

      origin.get(origin_array) ;  
#ifdef DGL_ARCH_WIN32
      origin_shm =  new_shm(sizeof(float)*6, "dgl\\state\\origin", origin_array) ; 
#else
      origin_shm =  new_shm(sizeof(float)*6, "dgl/state/origin", origin_array) ; 
#endif

      old_scale = scale = dgl->getScale() ;
#ifdef DGL_ARCH_WIN32
      scale_shm =  new_shm(sizeof(float), "dgl\\state\\scale", &scale) ;
#else
      scale_shm =  new_shm(sizeof(float), "dgl/state/scale", &scale) ;
#endif

      old_meters = meters = dgl->getMeters() ;
#ifdef DGL_ARCH_WIN32
      meters_shm =  new_shm(sizeof(float), "dgl\\state\\meters", &meters) ;
#else
      meters_shm =  new_shm(sizeof(float), "dgl/state/meters", &meters) ;
#endif

      ////////////////
    #ifdef __APPLE__
     #ifdef check
      #undef check
     #endif // check
    #endif // __APPLE__
      navList = (dtkNavList *) DGL::getApp()->check("dtkNavList", DTKNAVLIST_TYPE) ;
      if (!navList) 
	{
	  navList = new dtkNavList ;
	  if (DGL::getApp()->add(navList)) 
	    {
	      delete navList ;
	      navList = NULL ;
	      return ERROR_ ;
	    }
	}
      
      if (nav)
	{
	  old_location = location = nav->location ;
	  pivot = *(nav->pivot()) ; old_pivot = pivot ;
	  response = old_response = nav->response() ;
	  old_navScale = navScale = nav->scale ;
	  strcpy(navName, nav->getName()) ;
	  strcpy(old_navName, nav->getName()) ;
	}
      else
	{
	  location.zero(); old_location.zero() ;
	  pivot.zero(); old_pivot.zero() ;
	  response = old_response = 0.f ;
	  old_navScale = navScale = 1.f ;
	  strcpy(navName, "") ;
	  strcpy(old_navName, "") ;
	}
      location.get(location_array) ;
#ifdef DGL_ARCH_WIN32
      location_shm =  new_shm(sizeof(float)*6, "dgl\\state\\location", location_array) ; 
#else
      location_shm =  new_shm(sizeof(float)*6, "dgl/state/location", location_array) ; 
#endif
      pivot.get(pivot_array) ;
#ifdef DGL_ARCH_WIN32
      pivot_shm =  new_shm(sizeof(float)*6, "dgl\\state\\pivot", pivot_array) ; 
      response_shm =  new_shm(sizeof(float), "dgl\\state\\response", &response) ; 
      navScale_shm =  new_shm(sizeof(float), "dgl\\state\\navScale", &navScale) ;
      navName_shm =  new_shm(sizeof(navName), "dgl\\state\\navName", &navName) ;
#else
      pivot_shm =  new_shm(sizeof(float)*6, "dgl/state/pivot", pivot_array) ; 
      response_shm =  new_shm(sizeof(float), "dgl/state/response", &response) ; 
      navScale_shm =  new_shm(sizeof(float), "dgl/state/navScale", &navScale) ;
      navName_shm =  new_shm(sizeof(navName), "dgl/state/navName", &navName) ;
#endif

    }

  // do this every frame

  ////////////////
  if (frame_shm)
    {
      // note that first frame is frame 1, not frame 0
      frame++ ;
      frame_shm->write(&frame) ;
      //printf("%s %d: writing frame\n",__FILE__,__LINE__) ;
    }

  ////////////////
  if (time_shm)
    {
      time_elapsed = time.get() ;
      time_shm->write(&time_elapsed) ;
      //printf("%s %f: writing time_elapsed\n",__FILE__,__LINE__) ;
    }

  ////////////////
  if (origin_shm)
    {
      origin = dgl->getOrigin() ;
      if (old_origin.x != origin.x ||
	  old_origin.y != origin.y ||
	  old_origin.z != origin.z ||
	  old_origin.h != origin.h ||
	  old_origin.p != origin.p ||
	  old_origin.r != origin.r)
	{
	    // invert before writing out to make up for it getting flipped in DOSG,cpp
	    dtkMatrix m = dtkMatrix(origin);
	    m.invert();
	    m.coord(&origin);

	  old_origin = origin ;

	  origin.get(origin_array) ;
	  origin_shm->write(origin_array) ;
	  //printf("%s %d: writing origin\n",__FILE__,__LINE__) ;
	} 
    }

  if (scale_shm)
    {
      scale = dgl->getScale() ;
      if (old_scale != scale)
	{
	  old_scale = scale ;
	  scale_shm->write(&scale) ;
	  //printf("%s %d: writing scale\n",__FILE__,__LINE__) ;
	}
    }

  if (meters_shm)
    {
      meters = dgl->getMeters() ;
      if (old_meters != meters)
	{
	  old_meters = meters ;
	  meters_shm->write(&meters) ;
	  //printf("%s %d: writing meters\n",__FILE__,__LINE__) ;
	}
    }
  ////////////////
  nav = navList->current() ;
  if (nav)
    {
      if (location_shm)
	{
	  location = nav->location ;
	  if (old_location.x != location.x ||
	      old_location.y != location.y ||
	      old_location.z != location.z ||
	      old_location.h != location.h ||
	      old_location.p != location.p ||
	      old_location.r != location.r)
	    {
	      old_location = location ;
	      location.get(location_array) ;
	      location_shm->write(location_array) ;
	      //printf("%s %d: writing location\n",__FILE__,__LINE__) ;
	    } 
	}
      if (pivot_shm)
	{
	  pivot = *(nav->pivot()) ;
	  if (old_pivot.x != pivot.x ||
	      old_pivot.y != pivot.y ||
	      old_pivot.z != pivot.z ||
	      old_pivot.h != pivot.h ||
	      old_pivot.p != pivot.p ||
	      old_pivot.r != pivot.r)
	    {
	      old_pivot = pivot ;
	      pivot.get(pivot_array) ;
	      pivot_shm->write(pivot_array) ;
	      //printf("%s %d: writing pivot\n",__FILE__,__LINE__) ;
	    } 
	}
      if (response_shm)
	{
	  response = nav->response() ;
	  if (old_response != response)
	    {
	      old_response = response ;
	      response_shm->write(&response) ;
	      //printf("%s %d: writing response\n",__FILE__,__LINE__) ;
	    }
	}
      if (navScale_shm)
	{
	  navScale = nav->scale ;
	  if (old_navScale != navScale)
	    {
	      old_navScale = navScale ;
	      navScale_shm->write(&navScale) ;
	      //printf("%s %d: writing navScale\n",__FILE__,__LINE__) ;
	    }
	}
      if (navName_shm)
	{
	  strcpy(navName, nav->getName()) ;
	  if (strcmp(old_navName, navName))
	    {
	      strcpy(old_navName, navName) ;
	      navName_shm->write(navName) ;
	      //printf("%s %d: writing navName\n",__FILE__,__LINE__) ;
	    }
	}
    }
  
  return CONTINUE;
}

dtkSharedMem *writeState::new_shm(unsigned int size, char *name, void *init)
{
  dtkSharedMem *shm = NULL ;
  shm = new dtkSharedMem(size, name) ;
  if (!shm || shm->isInvalid()) 
    {
      dtkMsg.add(DTKMSG_ERROR,
		 "writeState:: new_shm failed:"
		 " couldn't open shared memory segment\"%s\"\n",name) ;
      shm = NULL ;
    }
  shm->write(init) ;

  return shm ;
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
  return new writeState(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}


