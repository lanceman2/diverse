#include <Inventor/SoDB.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <dgl.h>
#include <dcoin.h>
#include <iostream>
using namespace std;
/*
#if !defined( WIN32 ) || defined ( COIN_3DS_LOADER_SUPPORT )
#include <3dsLoader.h>
#endif
*/


//Static class member declarations
DCoin*   DCoin::m_self = NULL;
SoGroup* DCoin::m_base= NULL;
SoGroup* DCoin::m_ether = NULL;
SoGroup* DCoin::m_world = NULL;
bool     DCoin::m_flipval    = true;
SoTransform* DCoin::m_flip = NULL;

//=============================================================================
// Function Name: draw
/// @brief Draw the coin scene graph.
//=============================================================================
int DCoin::draw() 
{
  //Initalization for multiple contexts
  //static int contextcount=0;
  //static ContextVar<int> cid;

  //If the current context is not defined
  //Increment it for the next context
  //if (!cid.isDefined())
  //	  cid = contextcount++;

  //Get a viewport to display into
  //necessary for resizing the window
  if (m_flipval)
  	glRotatef(90,1,0,0);
  SbViewportRegion ViewPort;
  //Temporary viewport
  GLint viewport[4];
  //Get the viewport from opengl
  glGetIntegerv(GL_VIEWPORT, viewport);
  //Set the viewport
  ViewPort.setWindowSize(viewport[2], viewport[3]);
  ViewPort.setViewportPixels(viewport[0], viewport[1], viewport[2], viewport[3]);
  //Create the render action
  SoGLRenderAction myRenderAction(ViewPort);
  myRenderAction.setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
  //Open GL lighting
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  //Fix for specular lighting across different walls?
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1) ;
  //Set the context for the renderer
  //myRenderAction.setCacheContext(cid.get());
  //Render the world
  myRenderAction.apply(m_world);
  //myRenderAction.setNumPasses(2);
  //Render the ether
  glPushMatrix();
  glLoadIdentity();
  myRenderAction.apply(m_ether);
  glPopMatrix();
  //done
return 0;
}

//=============================================================================
// Function Name: flipWorld
/// @brief Flip the world -90 degrees in the x direction so that it is consistent
/// with DGL.
/// @param  Flip - set to true to flip the world, or false not to flip it.
//=============================================================================
void DCoin::flipWorld(const bool& Flip)
{
	m_self->m_flipval = Flip;
	/*
	dMatrix test90;
	test90.makeIdentity();
	test90.makeRotate(DEG2RAD(-90), dVec3(1,0,0));
	dQuat rot = test90.getQuat();
	//Set the rotation and then the translation of the navigation node
	m_flip->rotation.setValue(rot.x(), rot.y(), rot.z(), rot.w());
	//Setup the world
	m_world->addChild(m_flip);
	m_self->m_flipval = Flip;
	*/
}

//=============================================================================
// Function Name: postConfig
/// @brief Set up the initial viewport for the system.
/// @return Always returns 1.
//=============================================================================
int DCoin::postConfig()
{
	/*
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	m_scenemanager->setWindowSize(SbVec2s(viewport[2], viewport[3]));
	m_scenemanager->setSize(SbVec2s(viewport[2], viewport[3]));
	m_scenemanager->setViewportRegion(SbViewportRegion(viewport[2], viewport[3]));
	m_scenemanager->scheduleRedraw();
	*/
	return 1;
}



//=============================================================================
// Function Name: postProcess
/// @brief Set the viewport each time through in case of resizing.
//=============================================================================
int DCoin::postFrame()
{
  SoDB::getSensorManager()->processTimerQueue();
  SoDB::getSensorManager()->processDelayQueue(TRUE);
  return 0;
}


//=============================================================================
// Function Name: init
/// @brief Initalize the scene graph.
/// @param  FlipMyWorld - true to flip the world or false to leave it as is.
//=============================================================================
void DCoin::init(char* Name,const bool& FlipMyWorld)
{
	if (!m_self)
	{
		m_self = new DCoin(Name);
		flipWorld(FlipMyWorld);
		m_base->addChild(m_ether);
		m_base->addChild(m_world);
	}
}

//=============================================================================
// Function Name: getWorld
/// @brief Get the navigated node.
/// @return A pointer to the navigated world node.
//=============================================================================
SoGroup* DCoin::getWorld()
{
	return m_self->m_world;	
}

//=============================================================================
// Function Name: getRoot
/// @brief Get the root node of the scene graph.
/// @return A pointer to the navigated world node.
//=============================================================================
SoGroup* DCoin::getScene()
{
	return m_self->m_base;	
}

//=============================================================================
// Function Name: getEther
/// @brief Return the ether node.
/// @return  The non navigated ether node .
//=============================================================================
SoGroup* DCoin::getEther()
{
	return m_self->m_ether;
}

//=============================================================================
// Function Name: constructor 
/// @brief Constructor.
///
/// Create all of the nodes and set up all of the scene graph components
/// we need.
//=============================================================================
DCoin::DCoin(char* name): dglAugment(name)
{
    SoDB::init();
    SoNodeKit::init();
    //SoInteraction::init();

    //Start making all of the different nodes 
    m_base = new SoGroup;
    m_base->ref();
    m_ether = new SoGroup;
    m_world = new SoGroup;
    m_ether->ref();
    m_world->ref();
    //m_base->addChild(new SoDirectionalLight);

    //Now rotate the world 90 degrees because of axis differences
    m_flip = new SoTransform;
    m_flip->ref();

    /*
    dMatrix test90;
    test90.rotate(-90, dVec3(1,0,0));
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
	//Get the quaternion out of the matrix and store in x->w
	test90.quat(&x,&y,&z,&w );
	//Set the rotation and then the translation of the navigation node
	m_flip->rotation.setValue(x,y,z, w);
	//Setup the world
	if (m_flipval)
	{
	    m_world->addChild(m_flip);
	}
	*/
    //Add us to dgl so we actually get drawn
    DGL::addAugment(this);
}

//=============================================================================
// Function Name:  Destructor
/// @brief Destructor.
///
/// Destroy the scene graph.
//=============================================================================
DCoin::~DCoin()
{
	m_base->unref();
	m_ether->unref();
	m_world->unref();
}

//=============================================================================
// Function Name: loadFile
/// @brief Load a file from disk into coin.
/// @param filename - the name of the file to load.
/// @return A pointer to the model or null if it did not work.
//=============================================================================
SoSeparator* DCoin::loadFile(const string& filename)
{
	SoSeparator* myGraph; 
	SoInput input;
	input.addDirectoryLast( DGL_DATA_DIR );
	if (!input.openFile(filename.c_str()))
	{
		cout <<"Warning could not open file: " << filename << endl;
		return NULL;
	}
/*
#ifndef WIN32
	if (is3dsFile(&input))
	{
		if (!read3dsFile(&input, myGraph))
		{
			input.closeFile();
			cout <<"Warning, could not read 3ds file\n";
			return myGraph;
		}
		else
		{
			input.closeFile();
			return myGraph;
		}
	}
#endif 
*/
	myGraph = SoDB::readAll(&input);
	if (myGraph == NULL)
		cout << "Could not inventor file\n";
	input.closeFile();
	return myGraph;
}
