/***************************************************************************
                          DVtkWandRenderer.cpp  - Version 0.3.2
                             -------------------
    begin                : Fri Apr 26 2002
    copyright            : (C) 2002 by Michael Shore
    email                : mshore@vt.edu
 ***************************************************************************/

/***************************************************************************
 *
 * Portions of this code modified from the Visualization Toolkit.
 * The VTK code is covered under the following copyright:
 *
 * 	Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
 *		All rights reserved.
 *		See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *		This software is distributed WITHOUT ANY WARRANTY; without even 
 *   	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 *		PURPOSE.  See the above copyright notice for more information.
 *
 ***************************************************************************/

#include <dvtk.h>
#include <GL/gl.h>

#include <vtkProp.h>
#include <vtkMapper.h>


//Method to create a new renderer
DVtkWandRenderer *DVtkWandRenderer::New(char* name)
{
	return new DVtkWandRenderer(name);
}

//Add actor to the renderer
void DVtkWandRenderer::AddActor(vtkProp *p)
{
	static bool global_rendering = false;

	if (p->IsA("vtkActor") && !global_rendering)
	{
		((vtkActor*)p)->GetMapper()->GlobalImmediateModeRenderingOn();
		global_rendering = true;
	}

	vtkRenderer::AddActor(p);
}


//Method to rotate scene along X axis
void DVtkWandRenderer::RotateSceneX(float deg)
{
	degX = deg;
}

//The render method calls DGL methods, which render the scene via callbacks
void DVtkWandRenderer::Render()
{
	DGL::getApp()->frame();
}


//Constructor
DVtkWandRenderer::DVtkWandRenderer(char* name)
:dglAugment(name, dglAugment::PRENAV)
{
	degX = 0;
	clear = false;

	rendering = true;

	m_app = DGL::getApp();
   
   //Thread Safety
   DGL::createLock( "DVTK_LOCK" );

}


//Destructor
DVtkWandRenderer::~DVtkWandRenderer()
{
	//Thread Saftey
   DGL::deleteLock( "DVTK_LOCK" );
}


//Callback for OpenGL Initialization
int DVtkWandRenderer::initGL()
{
	
   // A bunch of GL calls to initialize lighting, materials, etc.
   // Most of this is stolen from earlier versions of vtk4CAVE, I think.

   glEnable(GL_NORMALIZE);

	
return 0;
}


int DVtkWandRenderer::preFrame()
{	
	
	/***********************************************
	 * This method contains code modified from     *
	 * part of the Render() method of vtkRenderer. *
	 * *********************************************/

	int      i;
   	vtkProp  *aProp;
	
	//create PropArray for storing props to be rendered
   	if ( this->Props->GetNumberOfItems() > 0 )
   	{
		this->PropArray = new vtkProp 
			*[this->Props->GetNumberOfItems()];
   	}
   	else
   	{
		this->PropArray = NULL;
   	}

   	//fill prop array
   	this->PropArrayCount = 0;
   	for ( i = 0, this->Props->InitTraversal(); 
		(aProp = this->Props->GetNextProp());i++ )
   	{
		if ( aProp->GetVisibility() )
		{
			this->PropArray[this->PropArrayCount++] = aProp;
		}
	}


	static bool once = false;

    	if (!once) 
	{
		once = true;
		cave = (dtkInCave *) m_app->get("caveInputDevices", 
			DTKINCAVE_TYPE);    
    	}

    	if (!cave) 
    	{
		dtkMsg.add(DTKMSG_WARN,
	       "Could not find the proper dtkInCave object.\n");
		return DTK_CALLBACK_ERROR;
    	}
    	if (cave->wand) 
	{
		float *f = cave->wand->read();
		wand_coord = dtkCoord(f[0], f[1], f[2], f[3], f[4], f[5]);

				
		return DTK_CONTINUE;
    	}

    	return DTK_CALLBACK_ERROR;
}


//Callback to display graphics
int DVtkWandRenderer::draw() 
{

   /***********************************************
	 * This method contains code modified from     *
	 * part of the Render() method of vtkRenderer. *
	 * *********************************************/
    if (rendering)
    {
	if(clear)
	{
	    glClearColor(this->Background[0],this->Background[1],
		this->Background[2],1.0);
	    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}
  	
	//if there are no props, give error message
	if ( this->PropArrayCount == 0 )
	{
	    vtkDebugMacro( << "There are no visible props!" );
	}
   
	//otherwise, render the props
	else
	{
	    //render props
	    glPushMatrix();

	    double scale = 0.03;

	    glTranslatef(wand_coord.x, wand_coord.y, wand_coord.z);
		
	    glRotatef(wand_coord.h, 0.f, 0.f, 1.f);
	    glRotatef(wand_coord.p, 1.f, 0.f, 0.f);
	    glRotatef(wand_coord.r, 0.f, 1.f, 0.f);


	    glPushMatrix();

	    glTranslatef(0,0.2,0);
		
	    glScaled(scale,scale,scale);

	    if (degX != 0)
		glRotatef(degX,1,0,0);

	    //glRotatef(180,0,0,1);

	    glEnable(GL_LIGHTING);
	    UpdateGeometry(); //Render actors
	    glDisable(GL_LIGHTING);

	    glPopMatrix();
		
	    glColor3f(1.f, 1.f, 0.f);
	    float len = 0.1f;
	    float light_spot_cutoff = 15.f;
	    float rad = len * tan(light_spot_cutoff * M_PI / 180.f);
	    glBegin(GL_LINES);
	    glVertex3f(0.f, 0.f, 0.f);
	    glVertex3f(rad, 0.1f, rad);
	    glVertex3f(0.f, 0.f, 0.f);
	    glVertex3f(-rad, 0.1f, rad);
	    glVertex3f(0.f, 0.f, 0.f);
	    glVertex3f(-rad, 0.1f, -rad);
	    glVertex3f(0.f, 0.f, 0.f);
	    glVertex3f(rad, 0.1f, -rad);
	    glEnd();
      
	    glBegin(GL_LINE_LOOP);
	    glVertex3f(rad, .1f, rad);
	    glVertex3f(rad, .1f, -rad);
	    glVertex3f(-rad, .1f, -rad);
	    glVertex3f(-rad, .1f, rad);
	    glEnd();

	    glPopMatrix();
	}
    }
    
    return 0;
}
