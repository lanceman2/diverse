// when the shift key is pressed, reads the cursor position in the graphics
// window and writes the XYZ (always xero) and HPR (R's zero too) that's
// under the cursor to shared memory.

// it only uses viewports in PERSPECTIVE screens, with offsets of 0,0,0 0,0,0

// the DOSGDESKTOPWANDHPR_SHM envvar can set the shared memory file to use.
// If not set, it uses wand

// if DOSGDESKTOPWANDHPR_NOCURSOR is set, no cursor will be visible

// if DOSGDESKTOPWANDHPR_NOTEXT is set, no text will be visible

#include <dtk.h>
#include <dgl.h>
#include <dosg.h>
#include <dtk/dtkDSO_loader.h>

#include <osgDB/ReadFile>
#include <osgText/Text>
#include <osg/Projection>
#include <osg/MatrixTransform>

class dosgDesktopWandHPR : public dtkAugment
{
public:
    dosgDesktopWandHPR(dtkManager *);
    virtual int postConfig(void) ;
    virtual int postFrame(void) ;

private:
    DGLMouse *_mouse ;
    DGLViewport *_viewport ;
    DGLScreen *_screen ;
    DGLWindow *_window ;
    dtkSharedMem *_wandShm ;
    float _wandData[6] ;
    float _oldWandData[6] ;
    osg::PositionAttitudeTransform *_cursorDCS ;
    osgText::Text *_text  ;
    osg::Projection* _projection ;
    float _mouseXY[2] ;
    int _viewportXY[2] ;
    unsigned _viewportWH[2] ;
    int _windowXY[2] ;
    unsigned int _windowWH[2] ;
    float _viewportMouseXY[2] ;
    bool _notext ;
    bool _nocursor ;
} ;


dosgDesktopWandHPR::dosgDesktopWandHPR(dtkManager *m) : dtkAugment("dosgDesktopWandHPR")
{
    setDescription("writes wand position based on cursor position in window") ;
  
    // the name of the shared memory to write to
    std::string _wandShmName ;
    if (getenv("DOSGDESKTOPWANDHPR_SHM")) _wandShmName = getenv("DOSGDESKTOPWANDHPR_SHM") ;
    else _wandShmName = "wand" ;

    _wandShm = new dtkSharedMem(sizeof(_wandData), _wandShmName.c_str()) ;
    if (_wandShm->isInvalid())
    {
	dtkMsg.add(DTKMSG_ERROR,"dosgDesktopWandHPR:: failed to get shared memory %s.\n", _wandShmName.c_str());
	return ;
    }

    memset(_wandData, 0, sizeof(_wandData)) ;
    memset(_oldWandData, 0, sizeof(_wandData)) ;
    _wandShm->write(_wandData) ;

    if (getenv("DOSGDESKTOPWANDHPR_NOTEXT")) 
    {
	_notext = true ;
    }
    else
    {
	_notext = false ;
    }

    if (getenv("DOSGDESKTOPWANDHPR_NOCURSOR")) 
    {
	_nocursor = true ;
    }
    else
    {
	_nocursor = false ;
    }


    // dtkAugment::dtkAugment() will not validate the object
    validate() ;
}

int dosgDesktopWandHPR::postConfig(void)
{
    _mouse = new DGLMouse ;

    // load cursor
    if (!_nocursor)
    {
	std::string cursorName ;
	if (getenv("DOSGDESKTOPWANDHPR_CURSOR")) cursorName = getenv("DOSGDESKTOPWANDHPR_CURSOR") ;
	else  cursorName = "dosgDesktopWandHPRcursor.osg" ;
	
	_cursorDCS = new osg::PositionAttitudeTransform ;
	_cursorDCS->ref() ;
	_cursorDCS->addChild(osgDB::readNodeFile(cursorName)) ;
	_cursorDCS->setNodeMask(0) ;
	DOSG::getScene()->addChild(_cursorDCS) ;
    }

    _projection = new osg::Projection;
    _projection->setMatrix(osg::Matrix::ortho2D(-1,1,-1,1));
    _projection->setNodeMask(0) ;
    DOSG::getScene()->addChild(_projection) ;

    // create HUD text
    if (!_notext)
    {
	osg::MatrixTransform *modelview_abs = new osg::MatrixTransform;
	modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	modelview_abs->setMatrix(osg::Matrix::identity());
	_projection->addChild(modelview_abs) ;
	
	osg::Geode *geode = new osg::Geode ;
	modelview_abs->addChild(geode) ;
	
	_text = new osgText::Text;
	_text->setFont("fonts/arial.ttf" );
	_text->setAlignment( osgText::Text::CENTER_CENTER );
	_text->setFontResolution(100,100);
	_text->setCharacterSize( 0.1 );
	_text->setColor( osg::Vec4(1,1,0,1));
	_text->setAxisAlignment( osgText::Text::SCREEN );
	_text->setPosition(osg::Vec3(0,-.95,1));
	geode->addDrawable(_text) ;
    }
    
    return  CONTINUE ;
}

int dosgDesktopWandHPR::postFrame(void)
{
    if (!_nocursor) _cursorDCS->setNodeMask(0) ;
    if (!_notext) _projection->setNodeMask(0) ;

    DGLKeyboard::ACTION sl =  DGLKeyboard::getState(KeyChar_Shift_L) ; 
    DGLKeyboard::ACTION sr =  DGLKeyboard::getState(KeyChar_Shift_R) ; 
    bool shift = (sl==DGLKeyboard::PRESSED || sr==DGLKeyboard::PRESSED) ;

    if (!shift) return CONTINUE ; 

    // poll mouse position, -1 to 1
    _mouse->getMouse(&_mouseXY[0], &_mouseXY[1]) ;
    
    _window = _mouse->getWindow() ;
    _window->getWindowDimensions(_windowXY[0], _windowXY[1], _windowWH[0], _windowWH[1]) ;

    for (unsigned int k=0; k < _window->getNumScreens(); k++)
    {
	_screen = _window->getScreen(k) ;

	// skip if screen offset is non-zero
	dtkCoord offset = _screen->getOffset() ;
	if (offset.x != 0.f || offset.y != 0.f || offset.z != 0.f || offset.h != 0.f || offset.p != 0.f || offset.r != 0.f) continue ;

	for (unsigned int l=0; l < _screen->getNumViewports(); l++)
	{
	    _viewport = _screen->getViewport(l) ;

	    if (_viewport->getViewportType() != DGLViewport::PERSPECTIVE) continue ;

	    _viewport->getGeometry(_viewportXY[0], _viewportXY[1], _viewportWH[0], _viewportWH[1]);

	    // is mouse in this viewport?
	    // extra lifting if the viewport doesn't fill the screen
	    if (_viewportWH[0] !=0 && _viewportWH[1] !=0) 
	    {
#if 0		
		// skip if the screen is smaller then the viewport's base
		if ((int)_windowWH[0]<=_viewportXY[0] || int(_windowWH[1])<=_viewportXY[1]) 
		{
		    continue ;
		}
#endif		
		// get mouse to go 0->1 in window
		for (unsigned int i=0; i<2; i++)
		{
		    _viewportMouseXY[i] = _mouseXY[i]/2.f + .5f ;
		    
		    // get mouse to go 0->1 in viewport
		    _viewportMouseXY[i] = (_viewportMouseXY[i]*(float)_windowWH[i] - float(_viewportXY[i]))/float(_viewportWH[i]) ;
		    
		    // get back -1 to 1 in viewport
		    _viewportMouseXY[i] = 2.f*_viewportMouseXY[i] - 1.f ;
		    
		}
		
		if (_viewportMouseXY[0]<-1.f || _viewportMouseXY[1]<-1.f || 
		    _viewportMouseXY[0]>1.f || _viewportMouseXY[1]>1.f)
		{
		    continue ;
		}
	    }
	    else //full screen
	    {
		memcpy(_viewportMouseXY, _mouseXY, sizeof(_mouseXY)) ;
	    }
	    
	    // to get here you have pressed shift in the managed area of a
	    // DGLViewport::PERSPECTIVE viewport with offset of zero
	    
	    // turn on cursor and label
	    if (!_nocursor) _cursorDCS->setNodeMask(~0) ;
	    if (!_notext) _projection->setNodeMask(~0) ;
	    
	    // the fun begins!!
	    
	    float fov[2] ;
	    fov[0] = _screen->getFov() ;
	    fov[1] = fov[0] ;
	    
	    float ratio ;
	    // screen or viewport?
	    if (_viewportWH[0] !=0 && _viewportWH[1] !=0) 
	    {
		// use viewport
		ratio = float(_viewportWH[0])/float(_viewportWH[1]) ;
	    }
	    else
	    {
		// use window size
		ratio = float(_windowWH[0])/float(_windowWH[1]) ;
	    }
	    
	    DGLScreen::AUTOASPECT aspect = _screen->getAutoAspect() ;
	    if (aspect == DGLScreen::VERTICAL) 
	    {
		// need to adjust horizontal fov for non-square screens
		_viewportMouseXY[0] *= ratio ;
		_projection->setMatrix(osg::Matrix::ortho2D(-ratio,ratio,-1,1));
	    }
	    
	    else if (aspect == DGLScreen::HORIZONTAL)
	    {
		// need to adjust vertical fov for non-square screens
		_viewportMouseXY[1] /= ratio ;
		_projection->setMatrix(osg::Matrix::ortho2D(-1.f/ratio, 1.f/ratio, -1, 1));
	    }
	    
	    else if (aspect == DGLScreen::NONE)
	    {
		_projection->setMatrix(osg::Matrix::ortho2D(-1,1,-1,1));
	    }
	    
	    
	    // zero it out- we only set H and P
	    memset(_wandData, 0, sizeof(_wandData)) ; 
	    
	    // for x, assume a triangle a, b, c.  
	    // a=adjacent side, distance from eye to cursor, always 1
	    // b=opposite side, distance to cursor, based on fov
	    // c=hypot
	    
	    float b = _viewportMouseXY[0]*tan((fov[0]/2.f)*(M_PI/180.f)) ;
	    _wandData[3] = -atan(b)*180.f/M_PI ;
	    
	    float c = sqrt(1.f+(b*b)) ;
	    
	    // for y, assume a triangle with sides c, d, e
	    // c is the same c as above triangle, but is not adjacent side
	    // d is oppostite side, distance to cursor, based on fov 
	    // e is the hypot, but we don't need it
	    float d = _viewportMouseXY[1]*tan((fov[1]/2.f)*(M_PI/180.f)) ;
	    
	    _wandData[4] = atan(d/c)*180.f/M_PI ;
	    if (memcmp(_oldWandData, _wandData, sizeof(_wandData)))
	    {
		_wandShm->write(_wandData) ;
		memcpy(_oldWandData, _wandData, sizeof(_wandData)) ;
	    }
	    
	    if (!_nocursor) _cursorDCS->setAttitude(DOSGUtil::euler_to_quat(_wandData[3], _wandData[4], 0)) ;
	    
	    if (!_notext) 
	    {
		char h[10], p[10] ;
		sprintf(h,"%+3.3d",int(_wandData[3])) ;
		sprintf(p,"%+3.3d",int(_wandData[4])) ;
		_text->setText("H = " + std::string(h) + " P = " + std::string(p)) ;
	    }
	    
	    // only do one per window
	    return  CONTINUE ;
	    
	}
    }

    return  CONTINUE ;

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
    return new dosgDesktopWandHPR(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}
