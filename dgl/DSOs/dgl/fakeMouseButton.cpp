// send fake mouse button events to your window manager

// this is a separate file, as X11/extensions/XTest.h doesn't seem to want
// to peacefully co-exist with OSG.

// John Kelso, kelso@nist.gov, 5/06, gets the blame

#include <stdio.h>

// button presses are only supported in X11.  Anyone want to do OS X and Windows?
#ifdef __linux__
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#endif

int fakeMouseButton(void *display, unsigned int button, bool state)
{
    //printf("fakeMouseButton: display = %p, button = %d, state = %d\n", display, button, state) ;
    if (!display)
	return 1 ;
#ifdef __linux__
    if(!XTestFakeButtonEvent((::Display*)display, button, state, CurrentTime))
    {
	printf("fakeMouseButton::postFrame: XTestFakeButtonEvent failed.\n");
	return 1 ;
    }
#endif
    return 0 ;
}

  
