////////////////////////////////////////////////////////////////////////
// 
// The DIVERSE graphics interface to OpenGL, version 2.4.4
//
// released: Fri Jul 28 18:00:00 EDT 2006
//
// The DIVERSE HOMPAGE is at: http://diverse.sourceforge.net/
// 
// Copyright (C) 2000 - 2006  Andrew A. Ray, John Kelso, Ron Kriz
// 
// This software, the DIVERSE graphics interface to OpenGL (DGL),
// is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License (LGPL) as
// published by the Free Software Foundation; either version 2.1 of
// the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this software, in the top level source directory in
// a file named "COPYING"; if not, see it at:
// http://www.gnu.org/copyleft/lesser.html or write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
// 
////////////////////////////////////////////////////////////////////////
#include <dtk.h>
#include <dgl.h>
#include <dtk/dtkDSO_loader.h>

// default DSO to set a simple display graphics state

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
  DGL *app = static_cast<DGL *>(manager);

  if (!app)
	  printf("HUH\n");
  if (app->isInvalid())
  	printf("App is invalid, not good\n");

  app->setMeters( 1.2192 );

  DGLDisplay* display = app->display();
  if (display == NULL)
  {
	printf("Error could not load display\n");
  }
  //One pipe for a display
  
  DGLPipe* pipe = display->getPipe(display->addDisplayScreenPipe(0,1));

  //Set up the window
  DGLWindow* window = new DGLWindow("DGL Screen Display");
  window->setPipe(pipe);
  window->setStereo(true);
  window->setFullScreen(true);
  window->setBorder(false);
  window->setCursor(true);
  window->setResizeable(false);
  window->setDepth(24);
  window->setWindowDimensions(0,0,1280,1024);
  pipe->addWindow(window);

  //Set up the screen within the window
  DGLScreen* screen = new DGLScreen;
  screen->setWindow(window);
  window->addScreen(screen);
  screen->setNear(.001f);
  screen->setFar(1000.f);
  dtkCoord offset;
  offset.zero();
  offset.y=1.0f;
  screen->setWidth(2.0f);
  screen->setHeight(1.5f);
  screen->setOffset(offset);
  screen->setInterOccular(-.03);

  //Set up the viewport in the screen to view
  DGLViewport* viewport = new DGLViewport;
  screen->addViewport(viewport);
  viewport->setScreen(screen);
  viewport->setViewportType(DGLViewport::SCREEN);
  viewport->setGeometry(0,0,1280,1024);

  return DTKDSO_LOAD_UNLOAD;
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}
