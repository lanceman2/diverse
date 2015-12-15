/***************************************************************************
                          DVtkRenderer.cpp  - Version 0.3.2
                             -------------------
    begin                : Fri Apr 26 2002
    copyright            : (C) 2002 by Michael Shore
    email                : mshore@vt.edu
 ***************************************************************************/

/***************************************************************************
 * Fixes -
 * 2007-02-02 :: Major memory leak was fixed . The memory leak
 *               caused major problems with large datasets.
 *               preFrame() was allocating PropArray but
 *               PropArray was being never deleted.
 *               Patch by: Philip Schuchardt - ohc@vt.edu
 *               Update - 2007-02-20 - Found bug with about fix, corrected
 *               Problem was a double delete bug in draw, moved delete to
 *               postFrame
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


void MyUpdateGeometry(void *ren);

//Method to create a new renderer
DVtkRenderer *DVtkRenderer::New(char* name)
{
	return new DVtkRenderer(name);
}

//Add actor to the renderer
void DVtkRenderer::AddActor(vtkProp *p)
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
void DVtkRenderer::RotateSceneX(float deg)
{
	degX = deg;
}

//The render method calls DGL methods, which render the scene via callbacks
void DVtkRenderer::Render()
{
	DGL::getApp()->frame();
}


//Constructor
DVtkRenderer::DVtkRenderer(char* name)
:dglAugment(name)
{
	degX = 0;
	clear = true;
   
   //Thread Safety
   DGL::createLock( "DVTK_LOCK" );

}


//Destructor
DVtkRenderer::~DVtkRenderer()
{
	//Thread Saftey
   DGL::deleteLock( "DVTK_LOCK" );
}


//Callback for OpenGL Initialization
int DVtkRenderer::initGL()
{
	
   // A bunch of GL calls to initialize lighting, materials, etc.
   // Most of this is stolen from earlier versions of vtk4CAVE, I think.
   glClearColor(this->Background[0],this->Background[1],this->Background[2],1.0);
   glClearDepth(1.);
   GLfloat mat_specular[]={1.0,1.0,1.0,1.0};
   GLfloat lt_specular[]={1.0,1.0,1.0,1.0};
   GLfloat lt_diffuse[]={0.8,0.8,0.8,1.0};
	GLfloat lt_ambient[]={0.5,0.5,0.5,1.0};
   GLfloat lt_position0[]={0,0,0,1};
   GLfloat lt_position1[]={1,-1,1,1};
   GLfloat mat_shininess[]={50.0};
   glDisable(GL_CULL_FACE);
   glClearColor(this->Background[0],this->Background[1],this->Background[2],1.0);
   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   glShadeModel(GL_SMOOTH);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,mat_shininess);
   glLightfv(GL_LIGHT0,GL_SPECULAR,lt_specular);
   glLightfv(GL_LIGHT0,GL_DIFFUSE,lt_diffuse);
	glLightfv(GL_LIGHT0,GL_AMBIENT,lt_ambient);
   glLightfv(GL_LIGHT0,GL_POSITION,lt_position0);
   glLightfv(GL_LIGHT1,GL_SPECULAR,lt_specular);
   glLightfv(GL_LIGHT1,GL_DIFFUSE,lt_diffuse);
   glLightfv(GL_LIGHT1,GL_POSITION,lt_position1);
   glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,1.0);
	glLightModelf(GL_LIGHT_MODEL_AMBIENT,1.0);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_DEPTH_TEST);
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   glLineWidth(1.0);

   //enable alpha / blending
   glEnable(GL_ALPHA_TEST);
   glAlphaFunc(GL_LESS,0);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	/*
	glPushMatrix();
	glRotatef(90,1,0,0);
	ClearLights();
	UpdateLightGeometry();
	UpdateLights();
	glPopMatrix();
	*/
	
	
	
return 0;
}


int DVtkRenderer::preFrame()
{
	/*
	glPushMatrix();
	//glRotatef(90,1,0,0);
	ClearLights();
	UpdateLightGeometry();
	UpdateLights();
	glPopMatrix();
	*/
	
	
	/***********************************************
	 * This method contains code modified from     *
	 * part of the Render() method of vtkRenderer. *
	 * *********************************************/

	int      i;
   vtkProp  *aProp;
   
   //set background color
   glClearColor(this->Background[0],this->Background[1],this->Background[2],1.0);

   //create PropArray for storing props to be rendered
   if ( this->Props->GetNumberOfItems() > 0 )
   {
       this->PropArray = new vtkProp *[this->Props->GetNumberOfItems()];
   }
   else
   {
       this->PropArray = NULL;
   }

   //fill prop array
   this->PropArrayCount = 0;
   for ( i = 0, this->Props->InitTraversal(); (aProp = this->Props->GetNextProp());i++ )
   {
       if ( aProp->GetVisibility() )
       {
           this->PropArray[this->PropArrayCount++] = aProp;
       }
   }


   return DTK_CONTINUE;
}


//Callback to display graphics
int DVtkRenderer::draw() 
{

   /***********************************************
	 * This method contains code modified from     *
	 * part of the Render() method of vtkRenderer. *
	 * *********************************************/

   if(clear)
   {
   	glClearColor(this->Background[0],this->Background[1],this->Background[2],1.0);
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

		if (degX != 0)
			glRotatef(degX,1,0,0);

		//MyUpdateGeometry((void *)this); //friend function to get around const problem
		glEnable(GL_LIGHTING);
		UpdateGeometry(); //Render actors
		glDisable(GL_LIGHTING);

		glPopMatrix();
   }

   return 0;
}
int DVtkRenderer::postFrame()
{
   // Clean up the space we allocated before. If the PropArray exists,
   // they all should exist
   if ( this->PropArray)
   {
       delete [] this->PropArray;
       this->PropArray                = NULL;
   }
   return 0;
}

//Friend function to get around problem of trying to call UpdateGeometry in a const method.
void MyUpdateGeometry(void *ren)
{
	DVtkRenderer *renderer = (DVtkRenderer*) ren;
      
	//UpdateGeometry does not appear to be thread safe.
	//Lock the renderer with a mutex while it renders 
	//to allow multi-threaded displays to render.
   renderer->lock();
	
		renderer->UpdateGeometry(); //Render actors
		
   renderer->unlock(); //After renderering, unlock so next thread can render.
}
