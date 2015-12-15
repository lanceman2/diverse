/***************************************************************************
                          DVtkRenderWindow.cpp  -  Version 0.3.2 
                             -------------------
    begin                : Fri Apr 26 2002
    copyright            : (C) 2002 by Michael Shore
    email                : mshore@sv.vt.edu
 ***************************************************************************/

#include <dvtk.h>

#include <vtkRendererCollection.h>

DVtkRenderWindow* DVtkRenderWindow::New()
{
   return new DVtkRenderWindow;
}


void DVtkRenderWindow::AddRenderer(DVtkRenderer *newRen)
{
   // Store the pointer to the renderer, and give the renderer
   // a pointer to this render window.
   ren = newRen;
   ren -> SetRenderWindow(this);
   
   // Add the renderer into the renderer collection.
   // This is needed. to allow some code, such as
   // importers, to function properly.
   this -> Renderers -> AddItem(newRen);
}

void DVtkRenderWindow::AddRenderer(DVtkWandRenderer *newRen)
{
   // Store the pointer to the renderer, and give the renderer
   // a pointer to this render window.
   wandren = newRen;
   wandren -> SetRenderWindow(this);
   
   // Add the renderer into the renderer collection.
   // This is needed. to allow some code, such as
   // importers, to function properly.
   this -> Renderers -> AddItem(newRen);
}



void DVtkRenderWindow::Render()
{
   if (ren != NULL) ren -> Render();
}

// Replaces the MakeCurrent method in VTK
// with one that does nothing.
void DVtkRenderWindow::MakeCurrent()
{
//   printf("DGL Render Window MakeCurrent!\n");
}

DVtkRenderWindow::DVtkRenderWindow()
{
}

DVtkRenderWindow::~DVtkRenderWindow()
{
	printf("Deleting DVtkRenderWindow may cause errors!\n");
}
