#include <dtk.h>
#include <dgl.h>
#include <GL/glu.h>
#include <GL/gl.h>

#ifdef OLDPRODUCER
#include <Producer/Camera>
#else
#include <Producer/Camera.h>
#endif

using namespace Producer;

bool DGLScreenHandler::frame(Camera& cam)
{
	//This is due to threads going crazy and not getting shut down right
	if (!dglIsRunning())
		return false;
	//Necessary for single threaded rendering to tell who is drawing
	DGL::getApp()->display()->setCamera(&cam);
	cam.getRenderSurface()->makeCurrent();
	//Now call init GL and setup GL if needbe (multi-threaded only b/c of structural limitations
	/*
	if (DGL::getApp()->getMultiThreaded())
	{
		if (DGL::getApp()->getInitGL() || DGL::getApp()->getSetupGL())
		{
			DGL::lock("INITGL_LOCK");
			if (DGL::getApp()->getInitGL())
			{
				DGL::getApp()->initGL();
			}
			if (DGL::getApp()->getSetupGL())
			{
				DGL::getApp()->setupGL();
			}
			DGL::unlock("INITGL_LOCK");
			return true;
		}
	}
	*/

	if (m_viewport->getViewState())
	  {
	    //Get the user specified frusta tip
	    dtkCoord view ;
	    view = m_viewport->getViewCoord() ;
	    //printf(" DGLScreenHandler::frame: Using user-specified frusta") ;
	    //view.print() ;
	    m_headpos[0] = view.x ;
	    m_headpos[1] = view.y ;
	    m_headpos[2] = view.z ;
	    m_headpos[3] = view.h ;
	    m_headpos[4] = view.p ;
	    m_headpos[5] = view.r ;
	  }
	else
	  {
	    //Get the head position  or set it to 0
	    if (DGL::getApp()->getHeadTracking())
	      {
		m_head->read(m_headpos);
		//dtkCoord foo ;
		//foo.set(m_headpos);
		//foo.print() ;
	      }
	    else
	      {
		memset(m_headpos, 0, sizeof(float)*6);
	      }
	  }

/*
	OpenThreads::Barrier* bar=NULL;
	if (bar == NULL)
	{
		int numW =0;
		for (unsigned int i=0; i< DGL::getApp()->display()->getNumPipes(); i++)
			numW += DGL::getApp()->display()->getPipe(i)->getNumWindows();	
		bar = new OpenThreads::Barrier(numW);
	}
*/
	//If using discrete stereo, set left draw callback
	//This is outside of the if so that if mono is being used it will
	//force the same buffer/eye to be used for all rendering.
	if( DGL::getApp()->getDiscreteStereo() )
		DGL::setDiscreteStereoLeft();
	if (m_viewport->getScreen()->getWindow()->getStereo())
	{
		//Set draw to the left buffer
		glDrawBuffer(GL_BACK_LEFT);
		//Set the eye
		m_eye=LEFT_EYE;
		//Clear it - called by the DGL Display class
		clear(cam);
		//Draw into it
		draw(cam);
		//If using discrete stereo, set right draw callback
		if( DGL::getApp()->getDiscreteStereo() )
			DGL::setDiscreteStereoRight();
		//Set draw to the right buffer
		glDrawBuffer(GL_BACK_RIGHT);
		//Set the eye
		m_eye=RIGHT_EYE;
		//Clear it Called by the dgl display class
		clear(cam);
		//Draw into it
		draw(cam);
	//	glDrawBuffer(GL_BACK);
	}
	else
	{
		//Reset the head position to 0,0,0
		//This is due to no head tracking on non stereo systems
		//memset(m_headpos, 0, sizeof(float)*6);
		//Set to the back buffer - DGL Display handles this
		//if (DGL::getApp()->getMultiThreaded())
		//clear(cam);
		//Draw into it
		draw(cam);
	}
	//cam.getRenderSurface()->swapBuffers();
	return true;
}
void DGLScreenHandler::clear(Camera& cam)
{
	//Set the clear color for the screen
	float r,g,b,a;
	DGL::getApp()->getClearColor(r,g,b,a);
	glClearColor(r,g,b,a);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void DGLScreenHandler::draw(Camera& cam)
{

	dglVec3<float> tmp;
	dglVec3<float> delta;
	dglVec3<float> eyeTx;
	dglVec3<float> v;
	dglVec4<float> up;
	dglVec4<float> p;
	dglMatrix emat;

	m_rot_mat.makeEuler(m_viewport->getScreen()->getOffset().h,
			    m_viewport->getScreen()->getOffset().p,
			    m_viewport->getScreen()->getOffset().r);
	m_inv_rot_mat.makeEulerInv(m_viewport->getScreen()->getOffset().h,
				   m_viewport->getScreen()->getOffset().p,
				   m_viewport->getScreen()->getOffset().r);


	//DIVERSE3 test eye code
	/*
	dglVec3<float> headdir;
	headdir.set(m_headpos[0], m_headpos[1]-90, m_headpos[2]);
	headdir = m_rot_mat * dglVec3<float>(0.0f, 1.0f, 0.0f);
	headdir.cross(dglVec3<float>(0.0f,0.0f,1.0f));
	headdir.normalize();
	delta.x = headdir.x * m_viewport->getScreen()->getInterOccular() /2;
	delta.y = headdir.y * m_viewport->getScreen()->getInterOccular() /2;
	delta.z = headdir.z * m_viewport->getScreen()->getInterOccular() /2;
	*/
	
	m_eyevec.set(m_headpos[0], m_headpos[1], m_headpos[2]);

	// interoccular is in meters- use this to convert to dpf 
	float offset =  m_viewport->getScreen()->getInterOccular()/(2.f*DGL::getApp()->getMeters()) ;
	emat.makeEuler(m_headpos[3], m_headpos[4], m_headpos[5]);
	delta.x = offset ;
	delta.y=0.0f;
	delta.z=0.0f;
	if (m_eye == LEFT_EYE) 
	{
		m_eyevec -= (emat * delta) ;
	}
	else if (m_eye == RIGHT_EYE)
	{
		m_eyevec += (emat * delta) ;
	}

	//Point in the Y direction (DIVERSE COORD SPECIFIC)
	p = dglVec4<float>(0.0f, 1.0f, 0.0f, 0.0f);
	p = m_rot_mat * p;
	p.normalize();

	//Set up to be in the Z direction (DIVERSE COORD SPECIFIC)
	up = dglVec4<float>(0.0f, 0.0f, 1.0f, 0.0f);
	up = m_rot_mat *up;
	up.normalize();

	//Adjust to eye
	v.x = m_viewport->getScreen()->getOffset().x - m_eyevec.x;
	v.y = m_viewport->getScreen()->getOffset().y - m_eyevec.y;
	v.z = m_viewport->getScreen()->getOffset().z - m_eyevec.z;

	//Rotate eye to face the screen
	eyeTx = m_inv_rot_mat * m_eyevec;

	//Temp variables
	//Distance from eye to screen
	float D = (p.x * v.x) + (p.y * v.y) + (p.z *v.z);
	float s = m_viewport->getScreen()->getNear() / D;
	float top=0.0f, left=0.0f, right=0.0f, bottom=0.0f;


	dglVec4<float> c(m_viewport->getScreen()->getOffset().x, m_viewport->getScreen()->getOffset().y, m_viewport->getScreen()->getOffset().z, 0);
	dglVec4<float> test;
	test = m_inv_rot_mat * c;

	//The frustum...
	/*
	float screen_t = c.z + (m_viewport->getScreen()->getHeight() / 2.0f);
	float screen_b = c.z + (m_viewport->getScreen()->getHeight() / -2.0f);
	float screen_r = c.x + (m_viewport->getScreen()->getWidth() / 2.0f);
	float screen_l = c.x + (m_viewport->getScreen()->getWidth() / -2.0f);
	*/

	float screen_t = test.z + (m_viewport->getScreen()->getHeight() / 2.0f);
	float screen_b = test.z + (m_viewport->getScreen()->getHeight() / -2.0f);
	float screen_r = test.x + (m_viewport->getScreen()->getWidth() / 2.0f);
	float screen_l = test.x + (m_viewport->getScreen()->getWidth() / -2.0f);

	top = (screen_t - eyeTx.z) * s;
	bottom = (screen_b - eyeTx.z) * s;
	left = (screen_l - eyeTx.x) * s;
	right = (screen_r - eyeTx.x) * s;

	//All that math for 6 measly GL calls...
	
	int Vx, Vy ;
	unsigned int Vw, Vh;
	m_viewport->getGeometry(Vx, Vy, Vw, Vh);
	if (Vw == 0 || Vh == 0)
	{
		//Get information from OpenGL about its state
		int vp[4];
		glGetIntegerv(GL_VIEWPORT, (GLint*)vp);
		Vx =  vp[0];
		Vy =  vp[1];
		Vw =  vp[2];
		Vh =  vp[3];
	}
	else
	{
		glViewport(Vx,Vy,Vw,Vh);
	}
	//glViewport(0,0,cam.getRenderSurface()->getWindowWidth(), cam.getRenderSurface()->getWindowHeight());


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//For checking to see if the gluLookAt is correct or not
	//gluPerspective(m_viewport->getScreen()->getFov(), (float)Vw / (float)Vh, m_viewport->getScreen()->getNear(), m_viewport->getScreen()->getFar());
	//Pre viewport code, this is WRONG
	//gluPerspective(90, cam.getRenderSurface()->getWindowWidth() / cam.getRenderSurface()->getWindowHeight(), m_viewport->getScreen()->getNear(), m_viewport->getScreen()->getFar());
	//For some reason this doesn't work properly, will fix in the future
	glFrustum(left, right, bottom, top, m_viewport->getScreen()->getNear(), m_viewport->getScreen()->getFar());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_eyevec.x, m_eyevec.y, m_eyevec.z, m_eyevec.x + p.x, m_eyevec.y + p.y, m_eyevec.z + p.z, up.x, up.y, up.z);


	//Rotate manually?
	/*
	glRotatef(-90.f, 1.0f, 0.0f, 0.0f);

	glRotatef(-m_viewport->getScreen()->getOffset().r, 0.0f, 1.0f, 0.0f);
	glRotatef(-m_viewport->getScreen()->getOffset().p, 1.0f, 0.0f, 0.0f);
	glRotatef(-m_viewport->getScreen()->getOffset().h, 0.0f, 0.0f, 1.0f);
	glTranslatef(-m_viewport->getScreen()->getOffset().x, -m_viewport->getScreen()->getOffset().y, -m_viewport->getScreen()->getOffset().z);

	*/

	/*
	if (DGL::getApp()->getHeadTracking())
	{
		glTranslatef(-m_headpos[0], -m_headpos[1], -m_headpos[2]);
		glRotatef(-m_headpos[3], 0.0f, 0.0f, 1.0f);
		glRotatef(-m_headpos[4], 1.0f, 0.0f, 0.0f);
		glRotatef(-m_headpos[5], 0.0f, 1.0f, 0.0f);
	}
	*/

	//Implementation of viewports

	//Now we actually get to draw the Opengl!
	DGL::getApp()->draw();
}

DGLScreenHandler::DGLScreenHandler()
{
  m_manager = (dtkManager*) DGL::getApp();
  if (m_manager == NULL || m_manager->isInvalid())
    {
      printf("DGLScreenHandler::DGLScreenHandler: Bad manager :(\n");
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
	dtkMsg.add(DTKMSG_ERROR,"DGLScreenHandler::DGLScreenHandler: failed to get dtkInLocator(\"head\").\n");
      if (m_manager->add(m_head)) return;
    }
  
  memset(m_headpos, 0, sizeof(float)*6);
  m_head->write(m_headpos) ;

	//Default to non stereo
	m_eye = MONO;
}

DGLScreenHandler::~DGLScreenHandler()
{
	if(m_head)
		delete m_head;
}
