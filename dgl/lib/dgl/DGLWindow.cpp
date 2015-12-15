#include <dtk.h>
#include <dgl.h>

#ifdef OLDPRODUCER
#include <Producer/RenderSurface>
#include <Producer/VisualChooser>
#include <Producer/KeyboardMouse>
#else
#include <Producer/RenderSurface.h>
#include <Producer/VisualChooser.h>
#include <Producer/KeyboardMouse.h>
#endif

#include <GL/glu.h>

int DGLWindow::m_context =0;

int DGLWindow::create()
{
	m_context++;
	m_cNum = m_context;
	m_chooser = new Producer::VisualChooser();
	if (m_chooser == NULL)
	{
		printf("ERROR:  Could not create a Producer::VisualChooser in DGLWindow::realize(), this program will probably segfault\n");
	}

	m_chooser->setSimpleConfiguration(true);
	m_chooser->setDepthSize(m_depth);

	if (m_stereo)
		m_chooser->useStereo();

	//Make sure this option is disabled because it breaks everything!
	m_chooser->setStrictAdherence(false);

	/*
	//SGI Hack...proprietary machines...
	unsigned int numMultiSamples = 0;
	#ifdef __sgi
		numMultiSamples = 4;
	#endif

	if (numMultiSamples)
	{
		#if defined( GLX_SAMPLES_SGIS )
			m_chooser->addExtendedAttribute( GLX_SAMPLES_SGIS,  numMultiSamples);
		#endif

		#if defined( GLX_SAMPLES_BUFFER_SGIS )
			m_chooser->addExtendedAttribute( GLX_SAMPLES_BUFFER_SGIS,  1);
		#endif
	}
	*/


	m_surface = new Producer::RenderSurface;
	m_surface->setWindowName(m_name);
	if (m_pipe->setDisplayNum())
		m_surface->setDisplayNum(m_pipe->getDisplayNum());
	if (m_pipe->setScreenNum())
		m_surface->setScreenNum(m_pipe->getScreenNum());
	if (m_pipe->setHostName())
		m_surface->setHostName(m_pipe->getHostName());

	m_surface->setVisualChooser(m_chooser);
	m_surface->setWindowRectangle(m_left, m_bottom, m_width, m_height, m_resizeable);
	m_surface->useCursor(m_cursor);
	m_surface->useBorder(m_border);

	if (m_fullscreen)
		m_surface->useBorder(false);

	#ifdef _X11_IMPLEMENTATION
		if (DGL::getApp()->getMultiThreaded())
			RenderSurface::initThreads();
	#endif

	#ifndef WIN32
		if (!DGL::getApp()->getMultiThreaded())
		    m_surface->useConfigEventThread(false);
	#endif
	DGL::getApp()->display()->addWindow(this);
	return 0;
}

int DGLWindow::realize()
{
	//m_surface->realize();
	//m_surface->waitForRealize();
	if (!m_child)
	{
		m_surface->realize();
		m_surface->waitForRealize();
	}
	else
	{
		m_surface->realize(m_chooser, m_parentWindow->getRenderSurface()->getGLContext());
		m_surface->waitForRealize();
	}
	return 0;
}
int DGLWindow::postRealize()
{
	if (!m_surface->isRealized())
		m_surface->waitForRealize();
	//Now the configuration options
	if (m_fullscreen)
		m_surface->fullScreen(m_fullscreen);

	//Producer events -> dtk events association code

	//If someone hasn't set this variable make the default one
	if (m_eventTranslator == NULL)
	{
		DMKCallback* temp= new DMKCallback;
		temp->setup();
		temp->setRS(m_surface);
		m_eventTranslator = (Producer::KeyboardMouseCallback*)temp;
	}
	m_km = new Producer::KeyboardMouse(m_surface);
	m_km->setCallback(m_eventTranslator);
	//We do not want events to be repeated, just sent once
	m_km->setAutoRepeatMode(false);

	return 0;
}

void DGLWindow::updateKM()
{
	m_km->update(*m_eventTranslator, false);
}

int DGLWindow::getWindowDimensions(int &left, int &bottom, unsigned int &width, unsigned int &height)
{
    if (!m_surface) return 1 ;

    m_surface->getWindowRectangle(left, bottom, width, height) ;

    return 0 ;
}

