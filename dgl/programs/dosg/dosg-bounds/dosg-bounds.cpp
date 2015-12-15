// returns the bounding sphere of a set of loadable files
// John Kelso, kelso@nist.gov 4/06

#include <osgDB/ReadFile>
#include <dtk.h>
#include <dgl.h>
#include <dosg/DOSGUtil.h>

#ifdef DGL_ARCH_WIN32
#  define SEPARATOR ";"
#else
#  define SEPARATOR ":"
#endif

int main(int argc, char **argv) 
{
  
  DOSG::init() ;

  if (argc < 2)
    {
      fprintf(stderr, "usage: %s file [...]\n", argv[0]);
      return 1 ;
    }
  


  osg::ArgumentParser arguments(&argc,argv);

  {
    std::string path ;
    path = "." SEPARATOR DGL_DATA_DIR ;
    char *osgfilepath = getenv("OSG_FILE_PATH") ;
    if (osgfilepath)
      {
	path += SEPARATOR ;
	path += osgfilepath ;
      }
    osgDB::Registry::instance()->setDataFilePathList(path) ;
  }

  osg::Node *node ;
  node = osgDB::readNodeFiles(arguments) ;
  if (!node) 
    return 1 ;


  osg::BoundingBox bbox ;
  bbox = DOSGUtil::computeBoundingBox(*node) ;

  osg::BoundingSphere sphere(bbox) ;

  osg::Vec3 center = sphere.center() ;

  printf("bounding sphere radius = %f\n",sphere.radius()) ;

  printf("X: %f - %f, delta: %f center: %f\n",
	 bbox.xMin(), bbox.xMax(), bbox.xMax()-bbox.xMin(), 
	 (bbox.xMax()+bbox.xMin())/2.0);

  printf("Y: %f - %f, delta: %f center: %f\n",
	 bbox.yMin(), bbox.yMax(), bbox.yMax()-bbox.yMin(), 
	 (bbox.yMax()+bbox.yMin())/2.0);

  printf("Z: %f - %f, delta: %f center: %f\n",
	 bbox.zMin(), bbox.zMax(), bbox.zMax()-bbox.zMin(), 
	 (bbox.zMax()+bbox.zMin())/2.0);

  return 0;
}
