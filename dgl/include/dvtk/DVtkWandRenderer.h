/***************************************************************************
                          DVtkWandRenderer.h  -  Version 0.2.1
                             -------------------
    begin                : Fri Apr 26 2002
    copyright            : (C) 2002 by Michael Shore
    email                : mshore@sv.vt.edu
 ***************************************************************************/
#ifndef VTKDGLWANDRENDERER_H
#define VTKDGLWANDRENDERER_H

#include <vtkOpenGLRenderer.h>

#include <dgl.h>

/*!
 * @class DVtkWandRenderer
 *
 * @brief The DIVERSE interface to VTK
 *
 * @author Michael Shore
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DVTKAPI DVtkWandRenderer : public vtkOpenGLRenderer, public dglAugment
{
   public:

	static DVtkWandRenderer *New(char* name="VTKAUGMENT"); //create a new instance of DVtkWandRenderer

	void AddActor(vtkProp *p);

	void RotateSceneX(float deg);
	void DisableClear(){ clear = false; }
	void EnableClear(){ clear = true; }

	void On() { rendering = true; }
	void Off() { rendering = false; }

	void Render(); //Render the scene
      /** Callback Methods **/
	int preFrame();
	int initGL();
	int draw();

   protected:
      DVtkWandRenderer(char* name);
      ~DVtkWandRenderer();   //destructor


		/** lock and unlock the mutex for thread safety **/
      void lock() {DGL::lock( "DVTK_LOCK" );}
      void unlock() {DGL::unlock( "DVTK_LOCK" );}

	friend void MyUpdateGeometry(void *ren); // friend function to get around draw() const problem

   private:
      DVtkWandRenderer(const DVtkWandRenderer&);   // not implemented
      void operator= (const DVtkWandRenderer&);  // not implemented

		float degX,degY,degZ;

      bool clear;

      DGL* m_app;
      dtkCoord wand_coord;
      dtkInCave *cave;

      bool rendering;

};

#endif
