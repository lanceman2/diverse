/***************************************************************************
                          DVtkRenderer.h  -  Version 0.2.1
                             -------------------
    begin                : Fri Apr 26 2002
    copyright            : (C) 2002 by Michael Shore
    email                : mshore@sv.vt.edu
 ***************************************************************************/
#ifndef VTKDGLRENDERER_H
#define VTKDGLRENDERER_H

#include <vtkOpenGLRenderer.h>

#include <dgl.h>

/*!
 * @class DVtkRenderer
 *
 * @brief The DIVERSE interface to VTK
 *
 * @author Michael Shore
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DVTKAPI DVtkRenderer : public vtkOpenGLRenderer, public dglAugment
{
   public:

	static DVtkRenderer *New(char* name="VTKAUGMENT"); //create a new instance of DVtkRenderer

	void AddActor(vtkProp *p);

	void RotateSceneX(float deg);
	void DisableClear(){ clear = false; }
	void EnableClear(){ clear = true; }

	void Render(); //Render the scene
      /** Callback Methods **/
	int preFrame();
	int initGL();
	int draw();
	int postFrame();

   protected:
      DVtkRenderer(char* name);
      ~DVtkRenderer();   //destructor


		/** lock and unlock the mutex for thread safety **/
      void lock() {DGL::lock( "DVTK_LOCK" );}
      void unlock() {DGL::unlock( "DVTK_LOCK");}

	friend void MyUpdateGeometry(void *ren); // friend function to get around draw() const problem

   private:
      DVtkRenderer(const DVtkRenderer&);   // not implemented
      void operator= (const DVtkRenderer&);  // not implemented

		float degX,degY,degZ;

      bool clear;

};

#endif
