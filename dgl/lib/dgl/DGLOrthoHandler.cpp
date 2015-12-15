#include <dtk.h>
#include <dgl.h>

#ifdef OLDPRODUCER
#include <Producer/Camera>
#else
#include <Producer/Camera.h>
#endif

#include <GL/glu.h>

using namespace Producer;

DGLOrthoHandler::DGLOrthoHandler()
{
    m_manager = (dtkManager*) DGL::getApp();
    if (m_manager == NULL || m_manager->isInvalid())
    {
	printf("DGLOrthoHandler::DGLOrthoHandler: Bad manager :(\n");
	return;
    }

#ifdef __APPLE__
#ifdef check
#undef check
#endif // check
#endif // __APPLE__
    if (!(m_head = (dtkInLocator*)m_manager->check("head")))
    {
	m_head = new dtkInLocator(m_manager->record(), "head");
	if (!m_head || m_head->isInvalid())
	    dtkMsg.add(DTKMSG_ERROR,"DGLOrthoHandler::DGLOrthoHandler: failed to get dtkInLocator(\"head\").\n");
	if (m_manager->add(m_head)) return;
    }
  
    memset(m_headpos, 0, sizeof(float)*6);
    m_head->write(m_headpos) ;

    m_width = m_height = 0 ;
    m_x = m_y = -1 ;
    m_w = m_h = 0 ;
}
DGLOrthoHandler::~DGLOrthoHandler()
{
    if (m_head !=NULL)
	delete m_head;
}

void DGLOrthoHandler::draw(Camera& cam)
{

    DGL::getApp()->display()->setCamera(&cam);
    cam.getRenderSurface()->makeCurrent();
    //In case of multi threaded rendering, make sure we 
    //have allowed initialization to happen
    /*
    if (DGL::getApp()->getMultiThreaded())
    {
	DGL::lock("INITGL_LOCK");
	if (DGL::getApp()->getInitGL())
	    DGL::getApp()->initGL();
	if (DGL::getApp()->getSetupGL())
	    DGL::getApp()->setupGL();
	DGL::unlock("INITGL_LOCK");
    }
    */

    m_width = cam.getRenderSurface()->getWindowWidth() ;
  
    m_height = cam.getRenderSurface()->getWindowHeight() ;
  
    m_viewport->getGeometry(m_x, m_y, m_w, m_h);

    m_viewport->getScreen()->getOrthoSides(m_left, m_right, m_bottom, m_top) ;

    //~printf("\n\nleft, right, bottom, top = %f %f %f %f\n",m_left, m_right, m_bottom, m_top) ;
    // ignore screen geometry if not set
    if (m_x < 0 || m_y < 0 || m_h == 0 || m_w == 0) 
    {
	// set aspect ration based on window size
	m_aspect_ratio = float(m_width)/float(m_height) ;
	//Force the correct viewport?  Should be set to this already
	glViewport(0,0,(GLsizei)m_width,(GLsizei)m_height);
    }
    else 
    {
	// set aspect ration based on viewport (geometry) size
	glViewport(m_x,m_y,m_w,m_h);
	m_aspect_ratio = float(m_w)/float(m_h) ;
    }
    //~printf("aspect_ratio=%f\n",m_aspect_ratio) ;

    m_aspect = m_viewport->getScreen()->getAutoAspect() ;
    if (m_aspect == DGLScreen::VERTICAL) // change left and right
    {
	m_delta = ((m_top-m_bottom)*m_aspect_ratio)/2.f ;
	m_center = (m_right+m_left)/2.f ;
	//~printf("delta = %f, center = %f\n",m_delta,m_center) ;
	m_left = m_center - m_delta ;
	m_right = m_center + m_delta ;
    }
    else if (m_aspect == DGLScreen::HORIZONTAL) // change top and bottom
    {
	m_delta = (m_right-m_left)/(m_aspect_ratio*2.f) ;
	m_center = (m_top+m_bottom)/2.f ;
	m_bottom = m_center - m_delta ;
	m_top = m_center + m_delta ;
    }
    //~printf("left, right, bottom, top = %f %f %f %f\n",m_left, m_right, m_bottom, m_top) ;


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
  
    glOrtho(m_left, m_right, 
	    m_bottom, m_top, 
	    m_viewport->getScreen()->getNear(), m_viewport->getScreen()->getFar()) ;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Goto diverse coordinates if we are supposed to
    if (DGL::getApp()->getDiverseCoord())
	glRotatef(-90.f, 1.0f, 0.0f, 0.0f);
  
  
    // fun with math
    // we're really rotating the world into the frustum, not the other way around
    {
	dtkMatrix mat ;
	mat.coord(m_viewport->getScreen()->getOffset()) ;
	mat.invert() ;
	dtkCoord coord ;
	mat.coord(&coord) ;
	//coord.print() ;
	//Rotate to the camera offset
	glRotatef(coord.h, 0.0f, 0.0f, 1.0f);
	glRotatef(coord.p, 1.0f, 0.0f, 0.0f);
	glRotatef(coord.r, 0.0f, 1.0f, 0.0f);
	glTranslatef(coord.x, coord.y, coord.z);
    }

    bool update_head ;
    update_head = false ;
    if (m_viewport->getViewState())
    {
	//Get the user specified frusta tip
	dtkCoord view ;
	view = m_viewport->getViewCoord() ;
	//printf("DGLOrthoHandler::draw: Using user-specified frusta") ;
	//view.print() ;
	m_headpos[0] = view.x ;
	m_headpos[1] = view.y ;
	m_headpos[2] = view.z ;
	m_headpos[3] = view.h ;
	m_headpos[4] = view.p ;
	m_headpos[5] = view.r ;
	update_head = true ;
    }
    else
    {
	//Head tracking
	if (DGL::getApp()->getHeadTracking())
	{
	    m_head->read(m_headpos);
	    update_head = true ;
	}
    }
    if (update_head)
    {
	dtkMatrix headMat ;
	headMat.coord(dtkCoord(m_headpos));
	dtkCoord headCoord ;
	headMat.invert() ;
	headMat.coord(&headCoord) ;
	glTranslatef(headCoord.x, headCoord.y, headCoord.z);
	glRotatef(headCoord.h, 0.0f, 0.0f, 1.0f);
	glRotatef(headCoord.p, 1.0f, 0.0f, 0.0f);
	glRotatef(headCoord.r, 0.0f, 1.0f, 0.0f);
    }

    //Call all draw functions
    DGL::getApp()->draw();
}

void DGLOrthoHandler::clear(Camera& cam)
{
    //Set the clear color for the screen
    float r,g,b,a;
    DGL::getApp()->getClearColor(r,g,b,a);

    cam.setClearColor(r,g,b,a);
    Producer::Camera::SceneHandler::clear(cam);

    //glClearColor(r,g,b,a);
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

/* commented out jtk 7/06
   void DGLOrthoHandler::cull(Camera& cam)
   {
   return;
   }
*/

bool DGLOrthoHandler::frame(Camera& cam)
{
    if (!dglIsRunning())
	return false;
    //cam.getRenderSurface()->makeCurrent();
    if (DGL::getApp()->getMultiThreaded())
	clear(cam);
    draw(cam);
    //cam.getRenderSurface()->swapBuffers();
    return true;
}
