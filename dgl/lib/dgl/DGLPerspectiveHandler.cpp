#include <dtk.h>
#include <dgl.h>

#ifdef OLDPRODUCER
#include <Producer/Camera>
#else
#include <Producer/Camera.h>
#endif

#include <GL/glu.h>

using namespace Producer;

DGLPerspectiveHandler::DGLPerspectiveHandler()
{
    m_manager = (dtkManager*) DGL::getApp();
    if (m_manager == NULL || m_manager->isInvalid())
    {
	printf("DGLPerspectiveHandler::DGLPerspectiveHandler: Bad manager :(\n");
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
	    dtkMsg.add(DTKMSG_ERROR,"DGLPerspectiveHandler::DGLPerspectiveHandler: failed to get dtkInLocator(\"head\").\n");
	if (m_manager->add(m_head)) return;
    }
  
    memset(m_headpos, 0, sizeof(float)*6);
    m_head->write(m_headpos) ;

    m_width = m_height = 0 ;
    m_aspect = DGLScreen::VERTICAL ;
    m_fov = 60 ;
    m_x = m_y = -1 ;
    m_w = m_h = 0 ;
}
DGLPerspectiveHandler::~DGLPerspectiveHandler()
{
    if (m_head !=NULL)
	delete m_head;
}

void DGLPerspectiveHandler::draw(Camera& cam)
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
    
    if (cam.getRenderSurface()->getWindowWidth() != m_width ) 
    {
	m_width = cam.getRenderSurface()->getWindowWidth() ;
	m_recalc_yfov = true ;
    }
  
    if (cam.getRenderSurface()->getWindowHeight() != m_height ) 
    {
	m_height = cam.getRenderSurface()->getWindowHeight() ;
	m_recalc_yfov = true ;
    }
  
    {
	int Vx, Vy ;
	unsigned int Vw, Vh;
	m_viewport->getGeometry(Vx, Vy, Vw, Vh);
	if (Vx != m_x) 
	{
	    m_x = Vx ;
	    m_recalc_yfov = true ;
	} 
	if (Vy != m_y) 
	{
	    m_y = Vy ;
	    m_recalc_yfov = true ;
	} 
	if (Vw != m_w) 
	{
	    m_w = Vw ;
	    m_recalc_yfov = true ;
	} 
	if (Vh != m_h) 
	{
	    m_h = Vh ;
	    m_recalc_yfov = true ;
	} 
    }
  
    if (m_fov != m_viewport->getScreen()->getFov())
    {
	m_fov = m_viewport->getScreen()->getFov() ;
	m_recalc_yfov = true ;
    }
  
    if (m_aspect != m_viewport->getScreen()->getAutoAspect())
    {
	m_aspect = m_viewport->getScreen()->getAutoAspect() ;
	m_recalc_yfov = true ;
    }
  
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
  
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // recalculate fov for horizontal ausot aspect
    if (m_recalc_yfov) 
    {
	// fun with trig...
	m_yfov = 2.f * RAD2DEG( atan( (tan( (DEG2RAD(m_fov/2.f)) )/m_aspect_ratio) ) ) ;
	//printf("m_yfov = %f\n",m_yfov) ;
	m_recalc_yfov = false ;
    }
  
    if (m_aspect==DGLScreen::VERTICAL)
    {
	gluPerspective(m_fov, m_aspect_ratio, m_viewport->getScreen()->getNear(), m_viewport->getScreen()->getFar());
    }
    else if (m_aspect==DGLScreen::HORIZONTAL)
    {
	gluPerspective(m_yfov, m_aspect_ratio, m_viewport->getScreen()->getNear(), m_viewport->getScreen()->getFar());
    }
    else // should be just NONE
    {
	gluPerspective(m_viewport->getScreen()->getFov(), 1.f, m_viewport->getScreen()->getNear(), m_viewport->getScreen()->getFar());
    }
  

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
	//printf("DGLPerspectiveHandler::draw: Using user-specified frusta") ;
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

    if( DGL::getApp()->getDiscreteStereo() )
                DGL::setDiscreteStereoLeft();

    //Call all draw functions
    DGL::getApp()->draw();
}

void DGLPerspectiveHandler::clear(Camera& cam)
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
   void DGLPerspectiveHandler::cull(Camera& cam)
   {
   return;
   }
*/

bool DGLPerspectiveHandler::frame(Camera& cam)
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
