#include <dtk.h>
#include <dgl.h>
//Here because of DTK magic numbers
#include <dtk/__manager_magicNums.h>

#ifdef OLDPRODUCER
#include <Producer/CameraConfig>
#include <Producer/CameraGroup>
#else
#include <Producer/CameraConfig.h>
#include <Producer/CameraGroup.h>
#endif


#ifdef Linux
	#include <values.h>
#endif

//#define CAMERA_GROUP
Producer::CameraGroup* cgroup;

DGLDisplay::DGLDisplay(DGL* app_in, const char* name): dtkDisplay(name)
{
	m_dgl = app_in;
	m_name = (char*)name;
	validate();
}
DGLDisplay::~DGLDisplay()
{
	if (DGL::getApp() != NULL && DGL::getApp()->getMultiThreaded())
	{
		for (unsigned int i=0; i<m_cameras.size(); i++)
			m_cameras[i]->cancel();
	}
	invalidate();
}

int DGLDisplay::addDisplayScreenPipe(unsigned int displayNumber, unsigned int screenNumber)
{
	DGLPipe* newPipe = new DGLPipe();
	newPipe->setScreenNum(screenNumber);
	newPipe->setDisplayNum(displayNumber);
	m_pipes.push_back(newPipe);
	return m_pipes.size()-1;
}

int DGLDisplay::addPipe(void)
{
	DGLPipe* newPipe = new DGLPipe();
	m_pipes.push_back(newPipe);
	return m_pipes.size()-1;
}

int DGLDisplay::addEnvPipe(char* display)
{
	char** hostName = new char*[1];
	unsigned int screenNum=0;
	unsigned int displayNum=0;

	DGLUtil::parseDisplay(display, hostName, &displayNum ,&screenNum);

	DGLPipe* newPipe = new DGLPipe();
	newPipe->setDisplayNum(displayNum);
	newPipe->setScreenNum(screenNum);
	newPipe->setHostName(hostName[0]);

	m_pipes.push_back(newPipe);
	return m_pipes.size()-1;
}

DGLPipe* DGLDisplay::getDisplayScreenPipe(unsigned int displayNumber, unsigned int screenNumber)
{
	for (unsigned int i=0; i< m_pipes.size(); i++)
	{
		if (m_pipes[i]->getDisplayNum() == displayNumber && m_pipes[i]->getScreenNum() == screenNumber)
			return m_pipes[i];
	}
	return NULL;
}
DGLPipe* DGLDisplay::getPipe(unsigned int pipe)
{
	if (pipe < m_pipes.size() )
		return m_pipes[pipe];
	return NULL;
}

DGLPipe* DGLDisplay::getEnvPipe(const char* display)
{
  char* hostName ;
  unsigned int screenNum=0;
  unsigned int displayNum=0;
  
  DGLUtil::parseDisplay(display, &hostName, &displayNum ,&screenNum);
  for (unsigned int i=0; i<m_pipes.size(); i++)
    {
      if (m_pipes[i]->getDisplayNum() == displayNum && 
	  m_pipes[i]->getScreenNum() == screenNum &&
	  ((m_pipes[i]->getHostName() == NULL && hostName == NULL) ||
	   (m_pipes[i]->getHostName() != NULL && hostName != NULL &&
	    !(strcmp(m_pipes[i]->getHostName(), hostName)))))
	{
	  return m_pipes[i] ;
	}
    }
  return NULL;
  
}

int DGLDisplay::config()
{
	//If we have shared contexts, disable multi threaded drawing otherwise we
	//will crash.
	for (unsigned int i=0; i< m_pipes.size(); i++)
	{
		for (unsigned int j=0; j< m_pipes[i]->getNumWindows(); j++)
		{
			if (m_pipes[i]->getWindow(j)->getSharedParent())
			{
				DGL::getApp()->setMultiThreaded(false);
				dtkMsg.add(DTKMSG_DEBUG, "DGLDisplay::config() Warning, cannot multi-thread with shared contexts, switching to single threaded mode.\n");
			}
		}
	}
	for (unsigned int i=0; i< m_pipes.size(); i++)
	{
		for (unsigned int j=0; j< m_pipes[i]->getNumWindows(); j++)
		{
			m_pipes[i]->getWindow(j)->create();
		}
	}
	//now create viewports
	for (unsigned int i=0; i< m_pipes.size(); i++)
	{
		for (unsigned int j=0; j< m_pipes[i]->getNumWindows(); j++)
		{
				//Create the viewports that are inside of each screen that is inside of each window (Holy nested loops (batman))
				for (unsigned int k=0; k < m_pipes[i]->getWindow(j)->getNumScreens(); k++)
				{
					for (unsigned int l=0; l < m_pipes[i]->getWindow(j)->getScreen(k)->getNumViewports(); l++)
					{
						m_pipes[i]->getWindow(j)->getScreen(k)->getViewport(l)->create();
					}
				}
		}
	}
	//Now check to see if there is more than one viewport per window (forces single threaded drawing also)
	for (unsigned int i=0; i< m_pipes.size(); i++)
	{
		for (unsigned int j=0; j< m_pipes[i]->getNumWindows(); j++)
		{
			//Each screen has at least a viewport, so if we have more than one screen, die
			if (m_pipes[i]->getWindow(j)->getNumScreens() > 1)
			{
				DGL::getApp()->setMultiThreaded(false);
				dtkMsg.add(DTKMSG_WARN, "DGLDisplay::config() %d Warning, cannot multi-thread with shared contexts, switching to single threaded mode.\n",__LINE__);
			}
				//Create the viewports that are inside of each screen that is inside of each window (Holy nested loops (batman))
				for (unsigned int k=0; k < m_pipes[i]->getWindow(j)->getNumScreens(); k++)
				{
					if ( m_pipes[i]->getWindow(j)->getScreen(k)->getNumViewports() > 1)
					{
						DGL::getApp()->setMultiThreaded(false);
						dtkMsg.add(DTKMSG_WARN, "DGLDisplay::config() %d Warning, cannot multi-thread with shared contexts, switching to single threaded mode.\n",__LINE__);
					}
				}
		}
	}
	// initialize to the first viewport we have
	// and make sure we at least have one pipe!
	if (m_pipes.size() > 0)
		m_debug_viewport = m_pipes[0]->getWindow(0)->getScreen(0)->getViewport(0) ;

	//To allow threading!
	if (DGL::getApp()->getMultiThreaded())
	{
		#ifndef CAMERA_GROUP
		OpenThreads::Thread::Init();
		//Now add threaded cameras to the mix
		static size_t stackSize=1024*8;
		for (unsigned int i=0; i< m_cameras.size(); i++)
		{
			DGLCameraThread* tmpCam = new DGLCameraThread;
			tmpCam->setStackSize(stackSize);
			tmpCam->setCamera(m_cameras[i]);
			m_tcams.push_back(tmpCam);
			//Do not start threads here b/c not all threads are going to be used!
			//Only one thread per RenderSurface works!
			//tmpCam->start();
		}

		//Now go assign masterlevel cameras (one for each render surface) to handle clearing and swapping of buffers.
		int lastCam=0;
		for (unsigned int i=0; i< m_pipes.size(); i++)
		{
			for (unsigned int j=0; j< m_pipes[i]->getNumWindows(); j++)
			{
				//Create the viewports that are inside of each screen that is inside of each window (Holy nested loops (batman))
				m_tcams[lastCam]->setMasterCam(true);
				m_tcams[lastCam]->setWindow(m_pipes[i]->getWindow(j));
				if (DGL::getApp()->getMultiThreaded())
				{
					m_tcams[lastCam]->start();
				}
				for (unsigned int k=0; k < m_pipes[i]->getWindow(j)->getNumScreens(); k++)
				{
					lastCam+=m_pipes[i]->getWindow(j)->getScreen(k)->getNumViewports(); 
				}
			}
		}
		#else
			Producer::CameraConfig* cconfig = new Producer::CameraConfig;
			for (unsigned int i=0; i < m_cameras.size(); i++)
			{
				char* buff = new char[80];
				sprintf(buff, "Window%d", i);
				cconfig->addCamera(string(buff), m_cameras[i]);
				delete [] buff;
			}
			cgroup = new Producer::CameraGroup(cconfig);
			cgroup->setInstrumentationMode( true);
			cgroup->realize(Producer::CameraGroup::ThreadPerCamera);
			//Make sure the threads are waiting before we frame
			cgroup->sync();
		#endif
	}
	else
	{
		for (unsigned int i=0; i< m_pipes.size(); i++)
		{
			for (unsigned int j=0; j< m_pipes[i]->getNumWindows(); j++)
			{
				m_pipes[i]->getWindow(j)->realize();
			}
		}
	}
	//Now do the post realization configuration step
	for (unsigned int i=0; i< m_pipes.size(); i++)
	{
		for (unsigned int j=0; j< m_pipes[i]->getNumWindows(); j++)
		{
			m_pipes[i]->getWindow(j)->postRealize();
		}
	}
	return dtkAugment::CONTINUE;
}
int DGLDisplay::postConfig()
{
	return dtkAugment::CONTINUE;
}
int DGLDisplay::preFrame()
{
	return dtkAugment::CONTINUE;
}
int DGLDisplay::sync()
{
	return dtkAugment::CONTINUE;
}

unsigned int DGLDisplay::getNumPipes()
{
	return m_pipes.size();
}

int DGLDisplay::preConfig()
{
	return dtkAugment::CONTINUE;
}

int DGLDisplay::frame()
{
	if (DGL::getApp()->getMultiThreaded() ==false)
	{
		//Do init across shared parent windows and windows that have no children
		//This is used if you want to only load textures once
		if (DGL::getApp()->getSetupGL())
		{
			DGL::getApp()->setSetupGL(false);
			for (unsigned int i=0; i< m_pipes.size(); i++)
			{
				for (unsigned int j=0; j< (unsigned)m_pipes[i]->getNumWindows(); j++)
				{
					if (m_pipes[i]->getWindow(j)->getSharedParent() || (!m_pipes[i]->getWindow(j)->getSharedParent() && !m_pipes[i]->getWindow(j)->getSharedChild()))
					{
							m_pipes[i]->getWindow(j)->getRenderSurface()->makeCurrent();
							Producer::RenderSurface* temp = m_pipes[i]->getWindow(j)->getRenderSurface();
							for (unsigned int k=0; k< m_cameras.size(); k++)
								if (temp == m_cameras[k]->getRenderSurface())
									m_currentCam = m_cameras[k];
							DGL::getApp()->setupGL();
					}
				}
			}
		}
		//Now do common OpenGL initialization across all render surfaces (windows)
		if (DGL::getApp()->getInitGL())
		{
			for (unsigned int i=0; i< m_pipes.size(); i++)
			{
				for (unsigned int j=0; j< (unsigned)m_pipes[i]->getNumWindows(); j++)
				{
						m_pipes[i]->getWindow(j)->getRenderSurface()->makeCurrent();
						Producer::RenderSurface* temp = m_pipes[i]->getWindow(j)->getRenderSurface();
						for (unsigned int k=0; k< m_cameras.size(); k++)
							if (temp == m_cameras[k]->getRenderSurface())
								m_currentCam = m_cameras[k];
						//Set the camrea that is currently drawing the scene
						DGL::getApp()->initGL();
				}
			}
			DGL::getApp()->setInitGL(false);
		}
		for (unsigned int i=0; i< m_pipes.size(); i++)
		{
			for (unsigned int j=0; j< (unsigned)m_pipes[i]->getNumWindows(); j++)
			{
				m_pipes[i]->getWindow(j)->getRenderSurface()->makeCurrent();
				for (unsigned int k=0; k< (unsigned)m_pipes[i]->getWindow(j)->getNumScreens(); k++)
						m_pipes[i]->getWindow(j)->getScreen(k)->clear();
				for (unsigned int k=0; k< (unsigned)m_pipes[i]->getWindow(j)->getNumScreens(); k++)
						m_pipes[i]->getWindow(j)->getScreen(k)->frame();
				glFinish();
			}
		}
		for (unsigned int i=0; i< m_pipes.size(); i++)
		{
			for (unsigned int j=0; j< (unsigned)m_pipes[i]->getNumWindows(); j++)
			{
				m_pipes[i]->getWindow(j)->getRenderSurface()->swapBuffers();
			}
		}
		/*
		for (unsigned int i=0; i<m_cameras.size(); i++)
		{
			m_currentCam = m_cameras[i];
			m_cameras[i]->getRenderSurface()->makeCurrent();
			m_cameras[i]->frame();
		}
		*/
	}
	else
	{
		#ifndef CAMERA_GROUP
		//Call the DGL setup function that is designed to load textures / display lists
		//for parent windows or 
		DGL::lock("CALLBACK_LOCK");
		if (DGL::getApp()->getSetupGL())
		{
			for (unsigned int i=0; i<m_tcams.size(); i++)
			{
				if (m_tcams[i]->getMasterCam())
				{
					m_tcams[i]->doSetup();
				}
			}
			DGL::getApp()->setSetupGL(false);

			static bool allSetup= false;
			//printf("Waiting until all are drawn\n");
			allSetup= false;
			while (!allSetup)
			{
				allSetup = true;
				for (unsigned int i=0; i<m_tcams.size(); i++)
				{
					if (m_tcams[i]->getMasterCam() && m_tcams[i]->setupDone() == false)
						allSetup= false;
				}
				if (!allSetup)
				{
					//printf("Not all drawn, sleeping\n");
					OpenThreads::Thread::YieldCurrentThread();
					//usleep(10000);
				}
			}
		}
		//Now do common OpenGL initialization if asked to
		if (DGL::getApp()->getInitGL())
		{
			for (unsigned int i=0; i<m_tcams.size(); i++)
			{
				if (m_tcams[i]->getMasterCam())
				{
					m_tcams[i]->doInit();
				}
			}
			static bool allInit= false;
			//printf("Waiting until all are drawn\n");
			allInit= false;
			while (!allInit)
			{
				allInit = true;
				for (unsigned int i=0; i<m_tcams.size(); i++)
				{
					if (m_tcams[i]->getMasterCam() && m_tcams[i]->initDone() == false)
						allInit= false;
				}
				if (!allInit)
				{
					//printf("Not all drawn, sleeping\n");
					OpenThreads::Thread::YieldCurrentThread();
					//usleep(10000);
				}
			}
			DGL::getApp()->setInitGL(false);
		}
		for (unsigned int i=0; i<m_tcams.size(); i++)
		{
			if (m_tcams[i]->getMasterCam())
				m_tcams[i]->clear();
		}

		static bool allClear= false;
		//printf("Waiting until all are drawn\n");
		allClear= false;
		while (!allClear)
		{
			allClear = true;
			for (unsigned int i=0; i<m_tcams.size(); i++)
			{
				if (m_tcams[i]->getMasterCam() && m_tcams[i]->cleared() == false)
					allClear= false;
			}
			if (!allClear)
			{
				//printf("Not all drawn, sleeping\n");
				OpenThreads::Thread::YieldCurrentThread();
				//usleep(10000);
			}
		}

		//Tell everything to draw
		//printf("Telling threads to draw!\n");
		for (unsigned int i=0; i< m_tcams.size(); i++)
		{
			m_tcams[i]->draw();
		}

		static bool allDrawn = false;
		//printf("Waiting until all are drawn\n");
		allDrawn = false;
		while (!allDrawn)
		{
			allDrawn = true;
			for (unsigned int i=0; i<m_tcams.size(); i++)
			{
				if (m_tcams[i]->getMasterCam() && m_tcams[i]->drew() == false)
					allDrawn = false;
			}
			if (!allDrawn)
			{
				//printf("Not all drawn, sleeping\n");
				OpenThreads::Thread::YieldCurrentThread();
				//usleep(10000);
			}
		}
		for (unsigned int i=0; i<m_tcams.size(); i++)
		{
			if (m_tcams[i]->getMasterCam())
				m_tcams[i]->swap();
		}

		static bool allSwapped= false;
		//printf("Waiting until all are drawn\n");
		allSwapped= false;
		while (!allSwapped)
		{
			allSwapped= true;
			for (unsigned int i=0; i<m_tcams.size(); i++)
			{
				if (m_tcams[i]->getMasterCam() && m_tcams[i]->swapped() == false)
					allSwapped= false;
			}
			if (!allSwapped)
			{
				//printf("Not all drawn, sleeping\n");
				OpenThreads::Thread::YieldCurrentThread();
				//usleep(10000);
			}
		}
		DGL::unlock("CALLBACK_LOCK");
		#else
			DGL::lock("CALLBACK_LOCK");
			cgroup->frame();
			if (DGL::getApp()->getInitGL() || DGL::getApp()->getSetupGL())
			{
				//Necessary for everything to come up
				//properly.  Synching voodoo.
				sleep(1);
				if (DGL::getApp()->getInitGL())
					DGL::getApp()->setInitGL(false);
				if (DGL::getApp()->getSetupGL())
					DGL::getApp()->setSetupGL(false);
			}
			DGL::unlock("CALLBACK_LOCK");
		#endif
	}

	//Get events
	for (unsigned int i=0; i<m_windows.size(); i++)
	{
		m_windows[i]->updateKM();
	}
	m_currentCam = NULL;
	return dtkAugment::CONTINUE;
}

DGLWindow* DGLDisplay::getWFromR(Producer::RenderSurface* rs)
{
	for (unsigned int i=0; i< m_pipes.size(); i++)
	{
		for (unsigned int j=0; j< m_pipes[i]->getNumWindows(); j++)
		{
			if ( m_pipes[i]->getWindow(j)->getRenderSurface() == rs)
				return m_pipes[i]->getWindow(j);
		}
	}
	return NULL;
}
Producer::Camera* DGLDisplay::getCamera()
{
	if (DGL::getApp()->getMultiThreaded())
	{
		 /*
		 OpenThreads::Thread* currThread = OpenThreads::Thread::CurrentThread();
		 if (currThread !=NULL)
		 {
			DGLCameraThread* cThread = (DGLCameraThread*)currThread;
			if (cThread !=NULL)
				return cThread->getCamera();
		 }
		 */
		 OpenThreads::Thread* currThread = OpenThreads::Thread::CurrentThread();
		 if (currThread !=NULL)
		 {
			Producer::Camera* cThread = (Producer::Camera*)currThread;
			if (cThread == NULL)
			{
				printf("Error in DGL::getCamera - Wrong type of thread being used\n");
			}
			return cThread;
		}
	}
	else
	{
		return m_currentCam;
	}
	return NULL;
}
