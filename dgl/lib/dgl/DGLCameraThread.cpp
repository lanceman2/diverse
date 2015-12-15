#include <dgl.h>

void DGLCameraThread::run()
{
	if (!m_master)
		return;

	//If we need to create (realize) the window, create it
	if (m_win->getRenderSurface()->isRealized() == false)
	{
		m_win->realize();
		//Die if we are single threaded
		if (DGL::getApp()->getMultiThreaded() == false)
			return;
	}
	if (m_camera->getRenderSurface() != m_win->getRenderSurface())
	{
		printf("Warning, camera render surface not the same as the window render surface in DGLCameraThread!!!!\n");
	}

	while (1) //Commented out to stop segfaults! DGL::getApp()->state && DTK_ISRUNNING)
	{
		if (m_master && m_clear)
		{
			//printf("Thread %d clearing\n", getThreadId());
			m_camera->getRenderSurface()->makeCurrent();
			for (unsigned int i=0; i<m_win->getNumScreens(); i++)
			{
				m_win->getScreen(i)->clear();
			}
			glFinish();
			m_clear = false;
		}
		else if (m_draw)
		{
			m_camera->getRenderSurface()->makeCurrent();
			for (unsigned int i=0; i<m_win->getNumScreens(); i++)
			{
				m_win->getScreen(i)->frame();
			}
			//m_camera->frame();
			glFinish();
			m_draw = false;
		}
		else if (m_master && m_swap)
		{
			//glFinish();
			m_camera->getRenderSurface()->makeCurrent();
			m_camera->getRenderSurface()->swapBuffers();
			//glFinish();
			m_swap = false;
		}
		else if(m_master && m_setup)
		{
			DGL::lock("INITGL_LOCK");
			if (m_win->getSharedParent() || (!m_win->getSharedParent() && !m_win->getSharedChild()))
			{
				m_win->getRenderSurface()->makeCurrent();
				DGL::getApp()->setupGL();
			}
			m_setup =false;
			DGL::unlock("INITGL_LOCK");
		}
		else if(m_master && m_init)
		{
			DGL::lock("INITGL_LOCK");
			m_win->getRenderSurface()->makeCurrent();
			DGL::getApp()->initGL();
			m_init = false;
			DGL::unlock("INITGL_LOCK");
		}
		OpenThreads::Thread::YieldCurrentThread();
	}
}
