#include <dtk.h>
#include <dgl.h>

extern "C"
{
	//=============================================================================
	/// @brief Causes preFrame, draw, and postFrame to be called for all services, thereby rendering each available display
	//=============================================================================
	void dglFrame()
	{
		DGL::getApp()->DGL::frame();
	}

	//=============================================================================
	/// @brief Returns true if DGL is running, otherwise returns false.
	//=============================================================================
	bool dglIsRunning()
	{
		return (DGL::getApp()->state & DTK_ISRUNNING);
	}

	//=============================================================================
	/// @brief Gets the pointer to CDglApp::m_data whch is a void pointer. Can be
	/// used anytime.
	//=============================================================================
	void* dglGetData()
	{
		return DGL::m_data;
	}

	//=============================================================================
	/// @brief Set the CDglApp::m_data to point to Data.
	/// @param Data - a void pointer which can be used to store a pointer to any object.
	//=============================================================================
	void dglSetData(void* Data)
	{
		DGL::m_data = Data;

	}

	//=============================================================================
	/// @brief Initializes DGL. Should be called prior to any other DGL function.
	/// @param argc - number of command line arguments.
	/// @param argv - the aurguments.
	//=============================================================================
    void dglInit()
    {
        DGL::init();
    }

	//=============================================================================
	/// @brief Starts DGL and runs one frame
	//=============================================================================
	void dglRun()
	{
		DGL::getApp()->DGL::run();
	}
	//=============================================================================
	/// @brief Starts DGL and runs continuously until all displays are closed.
	//=============================================================================
	void dglStart()
	{
		DGL::start();
	}

	//=============================================================================
	/// @brief Set the preConfig callback function.
	///
	/// A callback function to be used to have DGL call a function
	/// within your application during preConfig which occurs prior
	/// to creation of any display. Called only once prior to any
	/// rendering.
	/// @param cb - the callback function to be set.
	//=============================================================================
    void dglPreConfigCallback( dglPreConfigCB cb )
    {
        DGL::m_preconfig = cb;
    }
    
	//=============================================================================
	/// @brief Set the postConfig callback function.
	///
	/// A callback function to be used to have DGL call a function
	/// within your application during postConfig which occurs after
	/// the displays have been created and the OpenGL contexts have
	/// been created for each display. Called only once prior to any 
	/// rendering.
	/// @param cb - the callback function to be set.
	//=============================================================================
    void dglPostConfigCallback( dglPostConfigCB cb )
    {
        DGL::m_postconfig = cb;
    }
    
	//=============================================================================
	/// @brief Set the display callback function.
	///
	/// A callback function to be used to have DGL call a function for drawing within
	/// your application.
	/// @param cb - the callback function to be set.
	//=============================================================================
    void dglDisplayCallback( dglDisplayCB cb)
    {
        DGL::m_display = cb;
    }
    
	//=============================================================================
	/// @brief Set the preFrame callback function.
	///
	/// A callback function to be used to have DGL call a function
	/// within your application during preFrame which occurs before
	/// each frame is rendered. Called once every loop.
	/// @param cb - the callback function to be set.
	//=============================================================================
    void dglPreFrameCallback( dglPreFrameCB cb )
    {
        DGL::m_preframe = cb;
    }

	//=============================================================================
	/// @brief Set the postFrame callback function.
	///
	/// A callback function to be used to have DGL call a function
	/// within your application during postFrame which occurs after
	/// each frame is rendered. Called once every loop.
	/// @param cb - the callback function to be set.
	//=============================================================================
    void dglPostFrameCallback( dglPostFrameCB cb )
    {
        DGL::m_postframe = cb;
    }
    void dglPreNavCallback(dglPreNavCB cb)
    {
    	DGL::m_prenavcb = cb;
    }
    void dglOverlayCallback(dglOverlayCB cb)
    {
    	DGL::m_overlaycb = cb;
    }
    void dglSetupGLCallback(dglSetupGLCB cb)
    {
    	DGL::m_setupglcb = cb;
    }
    void dglInitGLCallback(dglInitGLCB cb)
    {
    	DGL::m_initglcb = cb;
    }
    void dglSetDiscreteStereo(bool discreteStereo)
    {
        DGL::m_discreteStereo = discreteStereo;
    }
	//=============================================================================
	/// @brief Set the display callback function for left display for discrete stereo.
	///
	/// A callback function to be used to have DGL call a function for drawing within
	/// your application.
	/// @param cb - the callback function to be set.
	//=============================================================================
    void dglDisplayLeftCallback( dglDisplayCB cb)
    {
        DGL::m_display_discrete_left = cb;
    }
    
	//=============================================================================
	/// @brief Set the display callback function for right display for discrete stereo.
	///
	/// A callback function to be used to have DGL call a function for drawing within
	/// your application.
	/// @param cb - the callback function to be set.
	//=============================================================================
    void dglDisplayRightCallback( dglDisplayCB cb)
    {
        DGL::m_display_discrete_right = cb;
    }
}

