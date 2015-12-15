//=============================================================================
// This is a useful example of how to write a simple DOSG application
// It illustrates how to load models into the world
//=============================================================================
#include <dosg.h>
#include<osgDB/Registry>
#include <dgl.h>
using namespace osg;

int main( int argc, char** argv )
{

  if (argc < 2)
    {
      fprintf(stderr, "usage: %s file [...]\n", argv[0]);
      return 1 ;
    }
  
  // set the search path for model files
  {
    std::string path ;
    path = ".:"DGL_DATA_DIR ;
    char *osgfilepath = getenv("OSG_FILE_PATH") ;
    if (osgfilepath)
      {
	path += ":" ;
	path += osgfilepath ;
      }
    osgDB::Registry::instance()->setDataFilePathList(path) ;
  }

  // load the files
  osg::ArgumentParser arguments(&argc,argv);
  osg::Node *node ;
  node = osgDB::readNodeFiles(arguments) ;
  if (!node) 
    return 1 ;

  //Create DGL & DOSG
  DOSG::init();

  // Start DGL
  dglStart();

  // Stick nodes into the scene
  DOSG::getWorld()->addChild( node );

  //Runtime loop
  dglRun();
  return 0;

}


