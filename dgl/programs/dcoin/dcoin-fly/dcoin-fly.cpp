#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotor.h>
#include <Inventor/nodes/SoShuttle.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoTransformSeparator.h>
#include <Inventor/nodes/SoMatrixTransform.h>


#include <Inventor/VRMLnodes/SoVRMLAnchor.h>
#include <Inventor/VRMLnodes/SoVRMLAppearance.h>
#include <Inventor/VRMLnodes/SoVRMLAudioClip.h>
#include <Inventor/VRMLnodes/SoVRMLBackground.h>
#include <Inventor/VRMLnodes/SoVRMLBillboard.h>
#include <Inventor/VRMLnodes/SoVRMLBox.h>
#include <Inventor/VRMLnodes/SoVRMLCollision.h>
#include <Inventor/VRMLnodes/SoVRMLColor.h>
#include <Inventor/VRMLnodes/SoVRMLColorInterpolator.h>
#include <Inventor/VRMLnodes/SoVRMLCone.h>
#include <Inventor/VRMLnodes/SoVRMLCoordinate.h>
#include <Inventor/VRMLnodes/SoVRMLCoordinateInterpolator.h>
#include <Inventor/VRMLnodes/SoVRMLCylinder.h>
#include <Inventor/VRMLnodes/SoVRMLCylinderSensor.h>
#include <Inventor/VRMLnodes/SoVRMLDirectionalLight.h>
#include <Inventor/VRMLnodes/SoVRMLDragSensor.h>
#include <Inventor/VRMLnodes/SoVRMLElevationGrid.h>
#include <Inventor/VRMLnodes/SoVRMLExtrusion.h>
#include <Inventor/VRMLnodes/SoVRMLFog.h>
#include <Inventor/VRMLnodes/SoVRMLFontStyle.h>
#include <Inventor/VRMLnodes/SoVRMLGeometry.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/VRMLnodes/SoVRMLImageTexture.h>
#include <Inventor/VRMLnodes/SoVRMLIndexedFaceSet.h>
#include <Inventor/VRMLnodes/SoVRMLIndexedLine.h>
#include <Inventor/VRMLnodes/SoVRMLIndexedLineSet.h>
#include <Inventor/VRMLnodes/SoVRMLIndexedShape.h>
#include <Inventor/VRMLnodes/SoVRMLInline.h>
#include <Inventor/VRMLnodes/SoVRMLInterpolator.h>
#include <Inventor/VRMLnodes/SoVRMLLight.h>
#include <Inventor/VRMLnodes/SoVRMLLOD.h>
#include <Inventor/VRMLnodes/SoVRMLMaterial.h>
#include <Inventor/VRMLnodes/SoVRMLMovieTexture.h>
#include <Inventor/VRMLnodes/SoVRMLNavigationInfo.h>
#include <Inventor/VRMLnodes/SoVRMLNormal.h>
#include <Inventor/VRMLnodes/SoVRMLNormalInterpolator.h>
#include <Inventor/VRMLnodes/SoVRMLOrientationInterpolator.h>
#include <Inventor/VRMLnodes/SoVRMLParent.h>
#include <Inventor/VRMLnodes/SoVRMLPixelTexture.h>
#include <Inventor/VRMLnodes/SoVRMLPlaneSensor.h>
#include <Inventor/VRMLnodes/SoVRMLPointLight.h>
#include <Inventor/VRMLnodes/SoVRMLPointSet.h>
#include <Inventor/VRMLnodes/SoVRMLPositionInterpolator.h>
#include <Inventor/VRMLnodes/SoVRMLProximitySensor.h>
#include <Inventor/VRMLnodes/SoVRMLScalarInterpolator.h>
#include <Inventor/VRMLnodes/SoVRMLScript.h>
#include <Inventor/VRMLnodes/SoVRMLSensor.h>
#include <Inventor/VRMLnodes/SoVRMLShape.h>
#include <Inventor/VRMLnodes/SoVRMLSound.h>
#include <Inventor/VRMLnodes/SoVRMLSphere.h>
#include <Inventor/VRMLnodes/SoVRMLSphereSensor.h>
#include <Inventor/VRMLnodes/SoVRMLSpotLight.h>
#include <Inventor/VRMLnodes/SoVRMLSwitch.h>
#include <Inventor/VRMLnodes/SoVRMLText.h>
#include <Inventor/VRMLnodes/SoVRMLTexture.h>
#include <Inventor/VRMLnodes/SoVRMLTextureCoordinate.h>
#include <Inventor/VRMLnodes/SoVRMLTextureTransform.h>
#include <Inventor/VRMLnodes/SoVRMLTimeSensor.h>
#include <Inventor/VRMLnodes/SoVRMLTouchSensor.h>
#include <Inventor/VRMLnodes/SoVRMLTransform.h>
#include <Inventor/VRMLnodes/SoVRMLVertexLine.h>
#include <Inventor/VRMLnodes/SoVRMLVertexPoint.h>
#include <Inventor/VRMLnodes/SoVRMLVertexShape.h>
#include <Inventor/VRMLnodes/SoVRMLViewpoint.h>
#include <Inventor/VRMLnodes/SoVRMLVisibilitySensor.h>
#include <Inventor/VRMLnodes/SoVRMLWorldInfo.h>

//#include <Inventor/VRMLnodes/SoWWWAnchor.h>
//#include <Inventor/VRMLnodes/SoWWWInline.h>

#include <dgl.h>
#include <dcoin.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#ifdef DGL_ARCH_WIN32
#  include<float.h>
#endif
using namespace std;

// ----------------------------------------------------------------------
//

int usage( int retval = 0 )
{
	printf( "Usage dcoin-fly [options] file\n" );
	printf( "Options:\n" );
	printf( "    -h|--help	Show this usage information.\n" );
	printf( "    -n|--near val    Set the near clipping plane distance.\n" );
	printf( "    -f|--far val     Set the far clipping plane distance.\n" );
	printf( "    file             The name of the file to be loaded.\n\n" );

	exit( retval );
}

float near_clip = FLT_MAX;
float far_clip = FLT_MAX;

std::string processArgs( int argc, char** argv )
{
	vector<string> args;
	for( int i=1;i<argc;i++ )
		args.push_back( argv[i] );

	std::string file;
	float val = 0.0f;

	unsigned int args_size = args.size();
	for( unsigned int a=0;a<args_size;a++ )
	{
		if( args[a] == "-h" || args[a] == "--help" )
			usage( 0 );
		else if( args[a] == "-n" || args[a] == "--near" )
		{
			if( a < args_size - 2 )
			{
				a++;
				if( convertStringToNumber( val, args[a].c_str() ) )
				{
					dtkMsg.add( DTKMSG_ERROR, "The parameter %s was passed an invalid floating point number %s\n", args[a-1].c_str(), args[a].c_str() );
					usage( -1 );
				}
				near_clip = val;
			}
			else
				usage( -1 );
		}
		else if( args[a] == "-f" || args[a] == "--far" )
		{
			if( a < args_size - 2 )
			{
				a++;
				if( convertStringToNumber( val, args[a].c_str() ) )
				{
					dtkMsg.add( DTKMSG_ERROR, "The parameter %s was passed an invalid floating point number %s\n", args[a-1].c_str(), args[a].c_str() );
					usage( -1 );
				}
				far_clip = val;
			}
		}
		else if ( args[a][0] == '-' )
		{
			dtkMsg.add( DTKMSG_ERROR, "Invalid option %s\n", args[a].c_str() );
			usage( -1 );
		}
		else if ( a == args_size - 1 )
		{
			file = args[a];
		}
	}
	return file;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		usage( -1 );
	}

	std::string file = processArgs( argc, argv );

	//DCoin::getSceneManager()->setBackgroundColor(SbColor(0.2f, 0.2f,  0.2f));
	dglInit();
	DCoin::init();
	SoSeparator* Temp = DCoin::loadFile( file.c_str() );

	if (Temp == NULL)
	{
		dtkMsg.add( DTKMSG_ERROR, "Could not load file %s\n", file.c_str() );
		return -1;
	}

	DGL::getApp()->preConfig();
	DGL::getApp()->config();
	DGL::getApp()->postConfig();

	if( near_clip != FLT_MAX )
	{
		printf("Near\n");
		DGL::display()->getPipe(0)->getWindow(0)->getScreen(0)->setNear(near_clip);
	}
	if( far_clip != FLT_MAX )
	{
		printf("Far\n");
		DGL::display()->getPipe(0)->getWindow(0)->getScreen(0)->setFar(far_clip);
	}

	DCoin::getWorld()->addChild(Temp);
	while (dglIsRunning())
	{
		dglFrame();
		//glRotatef(90, 1.0f, 0.0f, 0.0f);
	}
}
