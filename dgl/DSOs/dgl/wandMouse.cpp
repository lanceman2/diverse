// wandMouse- the wand's xyz hpr intersects a screen, and the
// rendersurface's pointer is moved to the intersection point

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

// how small can t get before we toss it?
#define SMALLEST_T (-10.f)

// put up an object to track intersection?
#ifdef CURSOR
#include <dosg.h>
#include<osg/PositionAttitudeTransform>
#define DSONAME dosgWandMouse
#define DSONAMECHAR "dosgWandMouse"
#else
#define DSONAME wandMouse
#define DSONAMECHAR "wandMouse"
#endif

#define WANDMOUSE_PIPES_ENABLED

// we support up to 4 buttons
#define NUM_BUTTONS (4) 

int fakeMouseButton(void *display, unsigned int button, bool state) ;

/*!
 * @class wandMouse wandMouse.cpp
 *
 * @brief DSO which moves the window system's cursor to track the wand's
 * intersection of immersive screens
 *
 * By default all pipes are inspected for wand intersection, but the pipes
 * can be listed in the WAND_PIPES envvar as a space separated list of X11
 * DISPLAY names, each corresponding to a pipe.
 *
 * Button presses are passed to the fakeMouseButton program
 *
 * For best results, turn on auto-focus and turn off auto-raise in your
 * window manager.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */

/*!
 * @class dosgWandMouse wandMouse.cpp
 *
 * @brief DSO which moves the window system's cursor to track the wand's
 * intersection of immersive screens
 *
 * By default all pipes are inspected for wand intersection, but the pipes
 * can be listed in the WAND_PIPES envvar as a space separated list of X11
 * DISPLAY names, each corresponding to a pipe.
 *
 * Button presses are passed to the fakeMouseButton program
 *
 * A cursor gnomon is moved in the scenegraph to be at the position of the
 * window system's cursor
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class DSONAME : public dtkAugment
{
public:
    DSONAME(dtkManager *m) ;
    virtual int postFrame(void) ;

private:
    dtkManager   *manager;
    dtkSharedMem *wandMouse_shm ;
    bool          wandMouse_active ;
    dtkInLocator *wand_loc ;
    float         wand_xyzhpr[6] ;
    float         old_wand_xyzhpr[6] ;
    DGLDisplay   *display ;
    DGLWindow    *prev_window ;
    bool          prev_window_cursor ; 
    dtkSharedMem *shm[NUM_BUTTONS] ;
#ifdef WANDMOUSE_PIPES_ENABLED
    std::vector<DGLPipe*>   pipes ;
#endif
#ifdef CURSOR
    osg::PositionAttitudeTransform *cursor ;
    osg::Switch *foo ;
    osg::Node *bar ;
#endif
    // set squared to distance cursor is from center of viewport
    bool cursorInsideWindow ;
    float distance2FromCursorToCenterOfViewport ;
#ifdef CURSOR
    dtkVec3 closest3Dpt ;
#endif
    int closest2Dpt[2] ;
    DGLWindow *closestWindow ;
} ;

DSONAME::DSONAME(dtkManager *m):
    dtkAugment(DSONAMECHAR)
{

    setDescription("use the wand xyz hpr to move the pointer in the intersected render surface");
  

    manager = (dtkManager*) DGL::getApp();
    if (manager == NULL || manager->isInvalid())
    {
	dtkMsg.add(DTKMSG_ERROR,DSONAMECHAR": Bad manager :(\n");
	return;
    }
    wandMouse_active = false ;
    prev_window = NULL ;
    display = DGL::getApp()->display() ;
  
    validate() ;
}

int DSONAME::postFrame(void) 
{
    static bool first = true ;

    if (first) 
    {
	first = false ;
      
#ifdef WANDMOUSE_PIPES_ENABLED
	// what pipes should we look at for intesections?
	if (getenv("WANDMOUSE_PIPES"))
	{
	    std::vector<std::string> v = DGLUtil::parseLine(getenv("WANDMOUSE_PIPES")) ;
	    for (unsigned int i=0; i<v.size(); i++)
	    {
		dtkMsg.add(DTKMSG_INFO,"Looking for pipe corresponding to display \"%s\"\n",(v[i]).c_str()) ;
		DGLPipe *p = display->getEnvPipe((v[i]).c_str()) ;
		if (p)
		{
		    pipes.push_back(p) ;
		}
		else
		{
		    dtkMsg.add(DTKMSG_ERROR,
			       DSONAMECHAR" failed:"
			       " couldn't find pipe for DISPLAY \"%s\"\n",
			       (v[i]).c_str()) ;
		    return ERROR_ ;
		}
	    }
	  
	}
	else
	{
	    for (unsigned int i = 0; i<display->getNumPipes(); i++)
	    {
		DGLPipe *p = display->getPipe(i) ;
		if (p)
		{
		    pipes.push_back(p) ;
		}
		else
		{
		    dtkMsg.add(DTKMSG_ERROR,
			       DSONAMECHAR" failed:"
			       " couldn't find pipe number %d\n",i) ;
		    return ERROR_ ;
		}
	    }
	}
#endif
	char *envvarname = strdup("WANDMOUSE_0") ;
	int l = strlen(envvarname) ;
	char zero ;
	zero = 0 ;
	for (int i=0; i<NUM_BUTTONS; i++)
	{
	    envvarname[l-1] = i+'0' ;
	    char *v = getenv(envvarname) ;
	    if (v)
	    {
		shm[i] = new dtkSharedMem(1, v, &zero) ;
		if (!shm[i] || shm[i]->isInvalid()) 
		{
		    dtkMsg.add(DTKMSG_ERROR,
			       DSONAMECHAR" failed:"
			       " couldn't open shared memory segment\"%s\"\n",
			       v) ; 
		    return ERROR_ ;
		}
		shm[i]->queue() ;
		shm[i]->flush() ;
		dtkMsg.add(DTKMSG_INFO, DSONAMECHAR"::postFrame: using shared memory \"%s\" to toggle wand button %d\n",
			   v, i) ;
	    }
	    else
	    {
		shm[i] = NULL ;
	    }
	}

#ifdef __APPLE__
#ifdef check
#undef check
#endif // check
#endif // __APPLE__
	if (!(wand_loc = (dtkInLocator*)manager->check("wand")))
	{
	    wand_loc = new dtkInLocator(manager->record(), "wand");
	    if (!wand_loc || wand_loc->isInvalid())
		dtkMsg.add(DTKMSG_ERROR, DSONAMECHAR":::"DSONAMECHAR": failed to get dtkInLocator(\"wand\").\n");
	    return ERROR_;
	}
      
#ifdef DGL_ARCH_WIN32
	wandMouse_shm =  new dtkSharedMem(1, "dgl\\wandMouse", 
					  &wandMouse_active) ; 
#else
	wandMouse_shm =  new dtkSharedMem(1, "dgl/wandMouse", 
					  &wandMouse_active) ; 
#endif
	if (!wandMouse_shm || wandMouse_shm->isInvalid()) 
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       DSONAMECHAR"::postFrame() failed:"
		       " couldn't open shared memory segment\"dgl/wandMouse\"\n") ;
	    return ERROR_ ;
	}
	
	wand_xyzhpr[0] = wand_xyzhpr[1] = wand_xyzhpr[2] = 
	    wand_xyzhpr[3] = wand_xyzhpr[4] = wand_xyzhpr[5] = 0.f ;
	memcpy(old_wand_xyzhpr, wand_xyzhpr, sizeof(float)*6) ;
	
#ifdef CURSOR
	// hang an object under where we think the wand intersects the wall
	cursor = new osg::PositionAttitudeTransform ;
	DOSG::getScene()->addChild(cursor) ;
	foo = new osg::Switch ;
	bar = osgDB::readNodeFile("cursor.osg") ;
	if (!bar)
	    dtkMsg.add(DTKMSG_WARN,
		       DSONAMECHAR"::postFrame():"
		       " couldn't load cursor object\n") ;
	cursor->addChild(foo) ;
	foo->addChild(bar) ;
	for (int i=0; i<6; i++)
	{
	    foo->getOrCreateStateSet()->
		setMode(GL_CLIP_PLANE0+i, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
	}
#endif

    } //if (first) 
  
    // do this every frame
#if 0
    static int foocount = 0 ;
#endif

    cursorInsideWindow = false ;
    distance2FromCursorToCenterOfViewport = FLT_MAX ;

    // are we active?
    wandMouse_shm->read(&wandMouse_active) ;
    if (wandMouse_active)
    {
	// if so read the wand
	wand_loc->read(wand_xyzhpr) ;
      
	// is it different?
	if (memcmp(wand_xyzhpr, old_wand_xyzhpr, sizeof(float)*6))
	{
	    // figure out line from wand
	    // parametric line is pt0 + t*pt
	    dtkVec3 pt0(wand_xyzhpr[0], wand_xyzhpr[1], wand_xyzhpr[2]) ;
	    // pt1 is 0,1,0 rotated by wand 
	    dtkVec3 pt = DGLUtil::eulerToXyz(wand_xyzhpr[3], wand_xyzhpr[4]) ;
	    // translate by wand xyz
	    //printf("\n\n\n================\n") ;
	    //printf("pt0: %+3.3f %+3.3f %+3.3f\n", pt0.x, pt0.y, pt0.z) ;
	    //printf("pt:  %+3.3f %+3.3f %+3.3f\n", pt.x, pt.y, pt.z) ;

	    // loop through all the screens we need to intersect- stop at first one that we hit
#ifdef WANDMOUSE_PIPES_ENABLED
	    for (unsigned int p=0; !cursorInsideWindow && p<pipes.size(); p++) 
	    {
		DGLPipe *pipe = pipes[p] ;
#else
	    for (unsigned int p=0; !cursorInsideWindow && p< display->getNumPipes(); p++) 
	    {
		DGLPipe *pipe = display->getPipe(p) ;
#endif
		for (unsigned int w=0; !cursorInsideWindow && w<pipe->getNumWindows(); w++) 
		{
		    DGLWindow *window = pipe->getWindow(w) ;
#if defined(_X11_IMPLEMENTATION)
		    ::Display *dpy =  window->getRenderSurface()->getDisplay() ;
#else
		    void *dpy = NULL ;
#endif		  
#if 0
		    if (foocount < 10)
		    {
			printf("p = %d, w = %d\n",p,w) ;
			printf("getDisplay = %p\n",dpy) ;
			printf("getDisplayNum = %d\n",
			       window->getRenderSurface()->getDisplayNum()) ;
			printf("getScreenNum = %d\n",
			       window->getRenderSurface()->getScreenNum()) ;
			foocount++ ;
		    }
#endif
		    // any button presses?
		    for (int i=0; i<NUM_BUTTONS; i++)
		    {
			if (shm[i])
			{
			    char c ;
			    if (shm[i]->qRead(&c))
			    {
				dtkMsg.add(DTKMSG_INFO,DSONAMECHAR": button %d = %d\n",i,c) ;
				// only send the button press if the focus is
				// NOT in one of our graphics windows 
				::Window fwin ;
#ifdef DGL_ARCH_WIN32
				fwin = GetFocus();
#else 
#if defined(_X11_IMPLEMENTATION)
				int rev ;
				XGetInputFocus(dpy, &fwin, &rev) ;
#endif
#endif
				if (fwin == window->getRenderSurface()->getWindow())
				{
				    dtkMsg.add(DTKMSG_NOTICE,DSONAMECHAR": ignoring button press in graphics window\n") ;
				    // next two commented out- you might need
				    // to do this depending on your immersive
				    // environmetn and window mnager
				    //printf("wandMouse: lowering graphics window\n") ;
				    //XLowerWindow(dpy, fwin) ;
				}
				else
				    fakeMouseButton(dpy, i, c) ;
			    }
			}
		    }
		    
		    for (unsigned int s=0; !cursorInsideWindow && s<window->getNumScreens(); s++) 
		    {
			DGLScreen *screen = window->getScreen(s) ;
			for (unsigned int v=0; !cursorInsideWindow && v<screen->getNumViewports(); v++) 
			{
			    DGLViewport *viewport = screen->getViewport(v) ;
			    if (viewport->getViewportType() == DGLViewport::SCREEN)
			    {
				dtkCoord offset = screen->getOffset() ;
				//offset.print() ;
				// get parametric form of plane- Ax +By +Cz +D = 0
			      
				// for A,B,C, calculate xyz normal- rotate xyz = 0,1,0 through hpr
				// of screen
				dtkVec3 ABC = DGLUtil::eulerToXyz(offset.h, offset.p) ;
				// D is -distance
				float D = -(ABC.x*offset.x + ABC.y*offset.y + ABC.z*offset.z) ;

				float denom = (ABC.x * pt.x) + (ABC.y * pt.y) +(ABC.z * pt.z) ;

			      
				if (denom!=0) // parallel to plane if == 0
				{
				    float t = (ABC.x*pt0.x + ABC.y*pt0.y + ABC.z*pt0.z + D)/denom ;
				    if (t<0.f && t>SMALLEST_T) //wand intersects screen
				    {
					// intersection point - we subtract since plane is pointing the wrong way
					dtkVec3 ipt ;
					ipt.x = pt0.x - t*pt.x ;
					ipt.y = pt0.y - t*pt.y ;
					ipt.z = pt0.z - t*pt.z ;
					//printf("intersection point") ;
					//ipt.print() ;
#ifdef CURSOR
					// ipt get stepped on in next set of transformations
					dtkVec3 saveIpt(ipt);
#endif
					// unrotate and translate the intersection point so we can compare with size of screen
					dtkMatrix rotMat ;
					rotMat.rotateHPR(offset.h, offset.p, offset.r) ;
					rotMat.translate(offset.x, offset.y, offset.z) ;

					dtkMatrix rotMatInv(rotMat) ;
					rotMatInv.invert() ;

					dtkMatrix mat ;
					mat.translate(ipt.x, ipt.y, ipt.z) ;
					mat.mult(&rotMatInv) ;
					mat.translate(&ipt) ;

					float width = screen->getWidth() ;
					float height = screen->getHeight() ;
					float width2 = width/2.f ;
					float height2 = height/2.f ;
					float newDist2 ;

					if (ipt.x < width2 && ipt.x > -width2 &&
					    ipt.z < height2 && ipt.z > -height2)
					{
					    newDist2 = 0.f ;
					    
					    // quit while you're ahead
					    cursorInsideWindow = true ;
					}
					else
					{
					    float newDist2 = ipt.x*ipt.x + ipt.z*ipt.z ;

					    // need to modify ipt and saveIpt so they're on the edge of the window

					    // clamp ipt, 3d positionn XZ plane , to screen width
					    if (ipt.x > width2) ipt.x = width2 ;
					    else if (ipt.x < -width2) ipt.x = -width2 ;
					    if (ipt.z > height2) ipt.z = height2 ;
					    else if (ipt.z < -height2) ipt.z = -height2 ;

#ifdef CURSOR
					    // need to unrotate and untranslate to get back to world 3d position
					    dtkMatrix newIptMat ;
					    newIptMat.translate(ipt) ;
					    newIptMat.mult(&rotMat) ;
					    newIptMat.translate(&saveIpt) ;
#endif
					    
					} // if (ipt.x < width2 && ipt.x > -width2 &&
					
					if (newDist2 < distance2FromCursorToCenterOfViewport) 
					{
					    distance2FromCursorToCenterOfViewport = newDist2 ; 
					    closestWindow = window ;
#ifdef CURSOR
					    closest3Dpt = saveIpt ;
#endif					    
					    // to move cursor- you need pixel coordinates!
					    int v_x, v_y ;
					    unsigned int v_w, v_h ;
					    viewport->getGeometry(v_x, v_y, v_w, v_h) ;
					    //printf("viewport: x = %d, y = %d, w = %d, h = %d\n", v_x, v_y, v_w, v_h) ;
					    closest2Dpt[0] = int(((ipt.x+width2)/width)*float(v_w)) + (v_x);
					    closest2Dpt[1] = int(((ipt.z+height2)/height)*float(v_h)) + (v_y) ;
					    
					    
					} // if (newDist2 < distance2FromCursorToCenterOfViewport) 

				    } // if (t<0.f && t>SMALLEST_T)
				} // if (denom!=0)
			    } // if (viewport->getViewportType() == DGLViewport::SCREEN)
			} // for (unsigned int v=0; v<screen->getNumViewports(); v++)
		    } // for (unsigned int s=0; s<window->getNumScreens(); s++)
		} // for (unsigned int w=0; w<pipe->getNumWindows(); w++)
	    } // for (unsigned int p=0; p< display->getNumPipes(); p++) 
	    

	    // move cursor to edge of closet window if not inside any window
	    if (distance2FromCursorToCenterOfViewport < FLT_MAX)
	    {
		//printf("distnce2 = %f\n",distance2FromCursorToCenterOfViewport) ;
		// restore cursor to previous state and turn on cursor in this window
		if (prev_window != closestWindow)
		{
		    if (prev_window)
		    {
			prev_window->setCursor(prev_window_cursor) ;
		    }
		    prev_window = closestWindow ;
		    prev_window_cursor = closestWindow->getCursor() ;
		    closestWindow->setCursor(true) ;
		}
#ifdef CURSOR
		cursor->setPosition(osg::Vec3(closest3Dpt.x,closest3Dpt.y,closest3Dpt.z)) ;
		foo->setAllChildrenOn() ;
#endif
		//printf(DSONAMECHAR": move pointer to: = %+5d %+5d\n",closest2Dpt[0], closest2Dpt[1]) ;
		closestWindow->getRenderSurface()->positionPointer(closest2Dpt[0], closest2Dpt[1]) ;
		
	    } // if (distance2FromCursorToCenterOfViewport < FLT_MAX)
	    
	// save for next time
	memcpy(old_wand_xyzhpr, wand_xyzhpr, sizeof(float)*6) ;
	} // if (memcmp(wand_xyzhpr, old_wand_xyzhpr, sizeof(float)*6))
	
    } // if (wandMouse_active)
#ifdef CURSOR
    else
    {
	//printf("cursor off\n") ;
	foo->setAllChildrenOff() ;
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
    return new DSONAME(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}



