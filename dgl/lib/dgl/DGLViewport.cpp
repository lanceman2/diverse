#include <dtk.h>
#include <dgl.h>

int DGLViewport::getGeometry(int& x, int& y, unsigned int& w, unsigned int& h)
{
    x = m_x;
    y = m_y;
    w = m_w;
    h = m_h;
    return 0;
}

int DGLViewport::setGeometry(int x, int y, unsigned int w, unsigned int h)
{
    m_x = x; 
    m_y = y;
    m_w = w;
    m_h = h;
    return 0;
}

DGLViewport::~DGLViewport()
{
    //Cannot delete the camera or handler
    //because of Producer protecting them.
}

int DGLViewport::create()
{
    if (m_type == PERSPECTIVE)
    {
	m_perspective_handler = new DGLPerspectiveHandler;
	m_perspective_handler->setViewport(this);
	m_handler = m_perspective_handler;
    }
    else if (m_type == ORTHO)
    {
	m_ortho_handler = new DGLOrthoHandler;
	m_ortho_handler->setViewport(this);
	m_handler = m_ortho_handler;
    }
    else if (m_type == SCREEN)
    {
	m_screen_handler = new DGLScreenHandler;
	m_screen_handler->setViewport(this);
	m_handler = m_screen_handler;
    }
    else
    {
	dtkMsg.add(DTKMSG_ERROR, "DGLViewport::create called when the type of viewport was not specified.\n");
	return 1;
    }
    m_handler->ref();
    m_camera = new Producer::Camera;
    m_camera->setRenderSurface(m_screen->getWindow()->getRenderSurface());
    m_camera->setSceneHandler(m_handler);

#ifndef OLDPRODUCER
    m_camera->getLens()->setAutoNearFar(false) ;
#endif

    //Add it to the DGL display draw list!
    DGL::getApp()->display()->addCamera(m_camera);
    return 0;
}

int DGLViewport::setViewportType(TYPE intype)
{

    // did we already have a handler, and we're changing types?
    if (m_handler && intype != m_type)
    {
	if (intype == PERSPECTIVE)
	{
	    if (!m_perspective_handler)
	    {
		m_perspective_handler = new DGLPerspectiveHandler;
		m_perspective_handler->ref() ;
		m_perspective_handler->setViewport(this);
	    }
	    m_handler = m_perspective_handler;	
	    m_camera->setSceneHandler(m_handler);
	}
	else if (intype == ORTHO)
	{
	    if (!m_ortho_handler)
	    {
		m_ortho_handler = new DGLOrthoHandler;
		m_ortho_handler->ref() ;
		m_ortho_handler->setViewport(this);
	    }
	    m_handler = m_ortho_handler;	
	    m_camera->setSceneHandler(m_handler);
	}
	else if (intype == SCREEN)
	{
	    if (!m_screen_handler)
	    {
		m_screen_handler = new DGLScreenHandler;
		m_screen_handler->ref() ;
		m_screen_handler->setViewport(this);
	    }
	    m_handler = m_screen_handler;	
	    m_camera->setSceneHandler(m_handler);
	}
    }
    m_type = intype; 

    return 0;
}

void DGLViewport::frame()
{
    m_camera->frame();
}
void DGLViewport::clear()
{
    m_handler->clear(*m_camera);
}
