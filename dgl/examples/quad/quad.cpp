//Example of how to get VTK content into DGL
/** port from Example Code on www.vtk.org **/
//Originally done by Mike Shore, updated by Andrew A. Ray

#include <dvtk.h>

#include <vtkGarbageCollector.h>
#include <vtkProperty.h>
#include <vtkQuadric.h>
#include <vtkSampleFunction.h>
#include <vtkContourFilter.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkImageData.h>

int main (int argc, char** argv)
{
	  // -- create the quadric function object --
	  
	  // create the quadric function definition
	  vtkQuadric *quadric = vtkQuadric::New();
	  quadric->SetCoefficients(.5,1,.2,0,.1,0,0,.2,0,0);

	  // sample the quadric function
	  vtkSampleFunction *sample = vtkSampleFunction::New();
	  sample->SetSampleDimensions(50,50,50);
	  sample->SetImplicitFunction(quadric);

	  // Create five surfaces F(x,y,z) = constant between range specified
	  vtkContourFilter *contours = vtkContourFilter::New();
	  contours->SetInput(sample->GetOutput());
	  contours->GenerateValues(5, 0.0, 1.2);

	  // map the contours to graphical primitives
	  vtkPolyDataMapper *contMapper = vtkPolyDataMapper::New();
	  contMapper->SetInput(contours->GetOutput());
	  contMapper->SetScalarRange(0.0, 1.2);

	  // create an actor for the contours
	  vtkActor *contActor = vtkActor::New();
	  contActor->SetMapper(contMapper);

	  // -- create a box around the function to indicate the sampling volume --

	  // create outline
	  vtkOutlineFilter *outline = vtkOutlineFilter::New();
	  outline->SetInput(sample->GetOutput());

	  // map it to graphics primitives
	  vtkPolyDataMapper *outlineMapper = vtkPolyDataMapper::New();
	  outlineMapper->SetInput(outline->GetOutput());

	  // create an actor for it
	  vtkActor *outlineActor = vtkActor::New();
	  outlineActor->SetMapper(outlineMapper);
	  outlineActor->GetProperty()->SetColor(0,0,0);

	  // -- render both of the objects --
	  
	  // a renderer and render window
	  dglInit();
	  DVtkRenderer *ren1 = DVtkRenderer::New();
	  DVtkRenderWindow *renWin = DVtkRenderWindow::New();
	  renWin->AddRenderer(ren1);

	  // don't need an interactor

	  // add the actors to the scene
	  ren1->AddActor(contActor);
	  ren1->AddActor(outlineActor);
	  ren1->SetBackground(1,1,1);
	  
	  ren1->RotateSceneX(90);

	  // render and run (lights and cameras are created automatically)
	dglStart();
	dglRun();
	  return 0;
}
