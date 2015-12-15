#include <dtk.h>
#include <dgl.h>
#include <dtk/dtkDSO_loader.h>
/*!
 * @class visWallEyeSetter
 *
 * @brief DSO which sets the first screen to be a left eye and the second
 * screen to be a right eye, and does this for screens :0.1 and :0.2
 *
 * Notice, this assumes the windows are set up the same way as in the
 * visWallScreens DSO. No error checking is done here to verify this!
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class visWallEyeSetter : public dglAugment
{
    public:
        visWallEyeSetter(char* Name);
	virtual int postConfig();
private:
	DGL*  m_app;
};

visWallEyeSetter::visWallEyeSetter( char* Name)
:dglAugment( Name)
{
    setDescription( "Sets the first screen to be a left eye and the second screen to be a right eye, twice." );
    m_app = DGL::getApp();
}

int visWallEyeSetter::postConfig()
{
	//Notice, this assumes how the windows are setup in the visWallScreens DSO
	//No error checking is here, so you will shoot yourself if you have DGL misconfigured and you try to use this DSO
	m_app->display()->getDisplayScreenPipe(0,1)->getWindow(0)->getScreen(0)->getViewport(0)->getScreenHandler()->setEye(DGLScreenHandler::LEFT_EYE);
	m_app->display()->getDisplayScreenPipe(0,1)->getWindow(1)->getScreen(0)->getViewport(0)->getScreenHandler()->setEye(DGLScreenHandler::RIGHT_EYE);
	m_app->display()->getDisplayScreenPipe(0,2)->getWindow(0)->getScreen(0)->getViewport(0)->getScreenHandler()->setEye(DGLScreenHandler::LEFT_EYE);
	m_app->display()->getDisplayScreenPipe(0,2)->getWindow(1)->getScreen(0)->getViewport(0)->getScreenHandler()->setEye(DGLScreenHandler::RIGHT_EYE);
	return dtkAugment::CONTINUE;
}
/*************** DTK C++ DSO loader/unloader functions ***********/
/*
 * All DTK DSO files are required to declare these two functions.
 * These function are called by the loading program to get your
 * C++ objects loaded.
 *
 *****************************************************************/
static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
	DGL::init();
	DGL* app = static_cast<DGL*>(manager); 
	visWallEyeSetter* temp = new visWallEyeSetter("visWallEyeSetter");
	app->addAugment(temp);
  
    return DTKDSO_LOAD_CONTINUE;
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}
