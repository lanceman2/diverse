#ifndef __DGL_CAMERA_THREAD_H__
#define __DGL_CAMERA_THREAD_H__
#include <OpenThreads/Thread>

#ifdef OLDPRODUCER
#include <Producer/Camera>
#else
#include <Producer/Camera.h>
#endif

class DGLWindow;
class DGLCameraThread : public OpenThreads::Thread
{
	public:
		DGLCameraThread() : OpenThreads::Thread() {m_draw = false; m_master=false; m_clear= false; m_swap=false; m_init=false; m_setup=false;};
		virtual ~DGLCameraThread(){};
		virtual void run();
		void setMasterCam(const bool& val) { m_master =true;};
		bool getMasterCam() { return m_master;};

		void draw() {m_draw = true;};
		bool drew() {return !m_draw;};

		void clear() {m_clear = true;};
		bool cleared() { return !m_clear;};

		void swap() { m_swap = true;};
		bool swapped() { return !m_swap;};

		void setCamera(Producer::Camera* cam) { m_camera = cam;};
		Producer::Camera* getCamera() { return m_camera;};

		void setWindow(DGLWindow* window){m_win = window;};
		DGLWindow* getWindow() { return m_win;};

		//These don't need to be synchronized!
		void doInit(){m_init=true;};
		bool initDone() { return !m_init;};

		void doSetup(){m_setup=true;};
		bool setupDone() { return !m_setup;};
	private:
		Producer::Camera* m_camera;
		bool              m_draw;
		bool              m_clear;
		bool              m_swap;
		bool              m_master;
		bool		  m_setup;
		bool		  m_init;
		DGLWindow*        m_win;
};
#endif
