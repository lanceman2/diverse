/////////
//
// dosg-transform
//
// loads one or more files, applies a global transformation, and dumps
// them as a single file
//
// John Kelso, kelso@nist.gov 1/07
//
////////

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <osgUtil/SmoothingVisitor>

#include <dtk.h> 
#include <dgl.h>
#include <dosg/DOSGUtil.h>

#ifdef DGL_ARCH_WIN32
#  define SEPARATOR ";"
#else
#  define SEPARATOR ":"
#endif

// global switches and pointers
static int verbose = 0 ;
static int optimize = 0 ;
static int normals = 0 ;
static int center = 0 ;
static int examine = 0 ;
static int bound = 0 ;

// absolute xyz for --center or --examine
static osg::Vec3 *xyz = NULL ;

// relative translation specified
static osg::Vec3 *translate = NULL ;

// center of transformations specified
static osg::Vec3 *origin = NULL ;

// allocated and set in processArgs
static char *name = NULL ;

// number of files to load, and their names
static int objc = 0 ;
static char **objv = NULL ;

static char *outfile = NULL ;

static osg::Vec3 hpr = osg::Vec3(0.f, 0.f, 0.f) ;
static osg::Vec3 scale = osg::Vec3(1.f, 1.f, 1.f) ;


////////////////////////////////////////////////////////////////////////
void usage(char *name) {
    fprintf(stderr, 
	    "Usage: %s [ --verbose | \\\n"
	    "            --translate X Y Z | \\\n"
	    "            --origin [ X Y Z ] | \\\n"
	    "            --center [ X Y Z ] | \\\n"
	    "            --euler H P R | \\\n"
	    "            --scale  X [ Y Z ]  | \\\n"
	    "            --bound [ B ] | \\\n"
	    "            --optimize | \\\n"
	    "            --examine | \\\n"
	    "            --normals \\\n"
	    "            infile [ infile ... ] outfile\n"
	    " where:\n"
	    "    --verbose\n"
	    "                    print informational messages when running\n"
	    "    --translate X Y Z\n"
	    "                    translates the model group by (X Y Z)\n" 
	    "    --origin [ X Y Z ]\n"
	    "                    centers the transformatinos around coordinate (X Y Z).\n"
	    "                    If (X,Y,X) isn't given, (0,0,0) is used.\n"
	    "    --center [ X Y Z ]\n"
	    "                    centers the model group around coordinate (X Y Z)\n"
	    "                    or the origin if (X Y Z) not given\n"
	    "    --euler H P R\n"
	    "                    rotates the model group by Euler angles (H, P, R)\n"
	    "    --scale   X [ Y Z ]\n"
	    "                    scales by (X,Y,Z), or uniformly by X if Y and Z\n"
	    "                    are omitted\n"
	    "    --bound [ B ]\n"
	    "                    uniformly scales the model group such that it fits into\n"
	    "                    a sphere with radius B, or 1, if B is omitted\n"
	    "    --optimize\n"
	    "                    invoke osgUtil::Optimize on the scenegraph\n"
	    "    --examine\n"
	    "                    creates a model similar to what diversifly's --examine\n"
	    "                    option produces.  The center is at (0,2,0) and the scale\n"
	    "                    is such that the object's bounding sphere has a radius of 1\n"
	    "    --normals\n"
	    "                    calls osgUtil::SmoothingVisitor to generate normals for\n"
	    "                    the surfaces.  It might not always generate the normals\n"
	    "                    you want, but is usually better than no normals at all.\n\n"
	    " infile [ infile ... ] is a list of files to be loaded as a group\n\n"
	    " outfile is a file to store the transformed files\n\n"
	    " if outfile is \"-\", an OSG file is sent to standard output.\n\n"
	    " \"--\" options can be abbreviated with minimal unique letters, for example:\n"
	    "    the \"--verbose\" option can be given as just \"--v\"\n\n"
	    " Since options can conflict (like --scale and --bound), the rightmost value\n"
	    "    takes precedence.  If more than one operation is specified, they will be\n"
	    "    done in the following order:\n"
	    " If --origin or --translate are given, --center and --examine are ignored.\n"
	    " If --center or --examine are given, --origin and --translate are ignored.\n"
	    "         1.  Translation or Centering\n"
	    "         2.  Euler rotation\n"
	    "         3.  Scale or Bounding sphere\n"
	    ,name) ;
}

////////////////////////////////////////////////////////////////////////
// local function prototypes that are defined after main()
static int processArgs(int argc, char **argv) ;

////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) 
{
  
    DOSG::init() ;

    if (processArgs(argc, argv)) return 1 ;
  
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

    osg::Group *node = new osg::Group ;
    for (int i=0; i<objc; i++) 
    {
	osg::Node *fileNode = osgDB::readNodeFile(std::string(objv[i])) ;
	if (fileNode)
	{
	    node->addChild(fileNode) ;
	} 
	else
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to load file \"%s\", exiting\n",objv[i]) ;
	    return 1 ;
	}
    }
    if (normals)
    {
	if (verbose)
	    printf("generating normals\n") ;
	osgUtil::SmoothingVisitor sv ;
	node->accept(sv) ;
    }

    osg::BoundingBox bbox ;
    bbox = DOSGUtil::computeBoundingBox(*node) ;
    float radius =  ((bbox._max - bbox._min).length())/2.f ;
    osg::Vec3 centerXYZ = (bbox._min + bbox._max)/2.f ;

    if (verbose) 
    {
	printf("input file(s) center = (%f, %f, %f), radius = %f\n",
	       centerXYZ[0], centerXYZ[1], centerXYZ[2],
	       radius) ;
    }
  
    if (center) 
    {
	if (verbose) 
	    printf("center xyz = (%f, %f, %f)\n", (*xyz)[0], (*xyz)[1], (*xyz)[2]) ;
    } 
    if (examine) 
    {
	scale[0] = scale[1] = scale[2] = 1.f/radius ;
	(*xyz)[0] = 0.f ;
	(*xyz)[1] = 2.f ;
	(*xyz)[2] = 0.f ;
	if (verbose) 
	    printf("examine xyz = (%f, %f, %f), scale = %f\n", 
		   (*xyz)[0], (*xyz)[1], (*xyz)[2], scale[0]) ;
    } 
    if (bound) 
    {
	scale[0] = scale[1] = scale[2] = scale[0]/radius ;
	if (verbose) 
	    printf("bounded scale = %f\n", scale[0]) ;
    } 

    // this was lifted from osgconv's OrientationConverter class

    // Order of operations here is :
    // 1. Translate to -center or --translate or --center
    // 2. Rotate to new orientation
    // 3. Scale in new orientation coordinates
    // 4. Translation (center or examine)
  
    osg::MatrixTransform* transform = new osg::MatrixTransform;
    transform->setDataVariance(osg::Object::STATIC);
    transform->addChild(node);

    osg::Matrix T1 ;
    if (!(origin || translate))
    {
	T1.makeTranslate(-centerXYZ);
	if (verbose)
	    printf("translating by -centerXYZ, %f %f %f\n",-centerXYZ[0], -centerXYZ[1], -centerXYZ[2]) ;
    }
    else
    {
	osg::Vec3 t = osg::Vec3(0.f,0.f,0.f);
	if (translate)
	    t+=(*translate) ;
	if (origin)
	    t-=(*origin) ;
	T1.makeTranslate(t);
	if (verbose)
	    printf("translating by translate-origin, %f %f %f\n",t[0], t[1], t[2]) ;
    }
  
    if (verbose)
    {
	printf("applying hpr = (%f, %f, %f)\n", hpr[0], hpr[1], hpr[2]) ;
	printf("applying scale = (%f, %f, %f)\n", scale[0], scale[1], scale[2]) ;
    }
    osg::Matrix R = osg::Matrix::rotate(DOSGUtil::euler_to_quat(hpr[0], hpr[1], hpr[2])) ;
    osg::Matrix S = osg::Matrix::scale(scale);

    osg::Matrix T2 ;
    if (xyz)
    {
	T2.makeTranslate((*xyz));
	if (verbose)
	    printf("translating back by xyz, %f %f %f\n",(*xyz)[0], (*xyz)[1], (*xyz)[2]) ;
    }
    else if (!(origin || translate))
    {
	T2.makeTranslate(centerXYZ);
	if (verbose)
	    printf("translating back by center, %f %f %f\n",centerXYZ[0], centerXYZ[1], centerXYZ[2]) ;
    }
    else
    {
	if (origin)
	{
	    T2.makeTranslate((*origin));
	    if (verbose)
		printf("translating back by origin, %f %f %f\n",(*origin)[0], (*origin)[1], (*origin)[2]) ;
	}
    }

  
    osg::Matrix T = T1 * R * S * T2 ;
    transform->setMatrix( T );
#ifdef OLDPRODUCER
//Need code for OLDPRODUCER to check for identity
#else
    if (!T.isIdentity())
    {
	if (verbose)
	    printf("applying osgUtil::Optimizer::FlattenStaticTransformsVisitor\n") ;
	osgUtil::Optimizer::FlattenStaticTransformsVisitor fstv;
	transform->accept(fstv);
	fstv.removeTransforms(transform);
    }
#endif

    if (optimize) 
    {
	if (verbose)
	    printf("optimizening graph\n") ;
	osgUtil::Optimizer optimizer;
	optimizer.optimize(transform);
    }
  
    if (verbose) 
    {
	bbox = DOSGUtil::computeBoundingBox(*transform) ;
	radius = ((bbox._max - bbox._min).length())/2.f ;
	centerXYZ = (bbox._min + bbox._max)/2.f ;

	printf("output file center = (%f, %f, %f), radius = %f\n",
	       centerXYZ[0], centerXYZ[1], centerXYZ[2],
	       radius) ;
    }


    if (!strcmp(outfile, "-"))
    {
	osgDB::ReaderWriter *writer = osgDB::Registry::instance()->getReaderWriterForExtension("osg") ;
	writer->writeNode(*transform, std::cout );
    }
    else
    {
	if (osgDB::writeObjectFile(*transform, std::string(outfile)))  
	    // normal exit
	    return 0;
	else
	{
	    dtkMsg.add(DTKMSG_ERROR,
		       "Unable to write file \"%s\", exiting\n",outfile) ;
	    return 1 ;
	}
    }
    
}

////////////////////////////////////////////////////////////////////////
static int processArgs(int argc, char **argv) {

    objc = 0 ;

    int c ;
  
    name = strdup(argv[0]) ;
  
    if (argc<3) {
	usage(name) ;
	return 1 ;
    }

    outfile = strdup(argv[argc-1]) ;
    argc-- ;

    // process remaining arguments
    c = 1 ;
  
    while (c<argc) {
    
	if (!strncmp("--",argv[c],strlen(argv[c]))) {
	    usage(name) ;
	    return 1 ;
      
	} else if (!strncmp("--verbose",argv[c],strlen(argv[c]))) {
	    verbose = 1 ;
	    c++ ;
      
	} else if (!strncmp("--optimize",argv[c],strlen(argv[c]))) {
	    optimize = 1 ;
	    c++ ;
      
	} else if (!strncmp("--normals",argv[c],strlen(argv[c]))) {
	    normals = 1 ;
	    c++ ;
      
	} else if (!strncmp("--examine",argv[c],strlen(argv[c]))) {
	    examine = 1 ;
	    bound = 0 ;
	    center = 0 ;
	    if (!xyz)
		xyz = new osg::Vec3 ;
	    if (origin)
	    {
		delete origin ;
		origin = NULL ;
	    }
	    if (translate)
	    {
		delete translate ;
		translate = NULL ;
	    }
	    c++ ;
      
	} else if (!strncmp("--translate",argv[c],strlen(argv[c]))) {
	    examine = 0 ;
	    center = 0 ;
	    if (!translate)
		translate = new osg::Vec3 ;
	    if (xyz)
	    {
		delete xyz ;
		xyz = NULL ;
	    }
	    c++ ;
	    if (c+2<argc) {

		if (DGLUtil::string_to_float(argv[c],&((*translate)[0]))) {
		    dtkMsg.add(DTKMSG_ERROR,
			       "Missing or invalid X xyz parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&((*translate)[1]))) {
		    dtkMsg.add(DTKMSG_ERROR,
			       "Missing or invalid Y xyz parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&((*translate)[2]))) {
		    dtkMsg.add(DTKMSG_ERROR,
			       "Missing or invalid Z xyz parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

	    } else {
		dtkMsg.add(DTKMSG_ERROR,
			   "Not enough xyz parameters\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--origin",argv[c],strlen(argv[c]))) {
	    examine = 0 ;
	    center = 0 ;
	    if (!origin)
		origin = new osg::Vec3 ;
	    if (xyz)
	    {
		delete xyz ;
		xyz = NULL ;
	    }
	    c++ ;
	    // parameters are optional, but all three are required if specified
	    // test first one to see if it's a number
	    if (!DGLUtil::string_to_float(argv[c],&((*origin)[0]))) {
		c++ ;
		if (c+1<argc) {
	  
		    if (DGLUtil::string_to_float(argv[c],&((*origin)[1]))) {
			dtkMsg.add(DTKMSG_ERROR,
				   "Missing or invalid Y origin parameter\n") ;
			usage(name) ;
			return 1 ;
		    }
		    c++ ;
	  
		    if (DGLUtil::string_to_float(argv[c],&((*origin)[2]))) {
			dtkMsg.add(DTKMSG_ERROR, 
				   "Missing or invalid Z origin parameter\n") ;
			usage(name) ;
			return 1 ;
		    } 
		    c++ ;
	  
		} else {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Not enough origin parameters\n") ;
		    usage(name) ;
		    return 1 ;
		}
	    }
      
	} else if (!strncmp("--center",argv[c],strlen(argv[c]))) {
	    examine = 0 ;
	    center = 1 ;
	    if (!xyz)
		xyz = new osg::Vec3 ;
	    if (origin)
	    {
		delete origin ;
		origin = NULL ;
	    }
	    if (translate)
	    {
		delete translate ;
		translate = NULL ;
	    }
	    c++ ;
	    // parameters are optional, but all three are required if specified
	    // test first one to see if it's a number
	    if (!DGLUtil::string_to_float(argv[c],&((*xyz)[0]))) {
		c++ ;
		if (c+1<argc) {
	  
		    if (DGLUtil::string_to_float(argv[c],&((*xyz)[1]))) {
			dtkMsg.add(DTKMSG_ERROR,
				   "Missing or invalid Y center parameter\n") ;
			usage(name) ;
			return 1 ;
		    }
		    c++ ;
	  
		    if (DGLUtil::string_to_float(argv[c],&((*xyz)[2]))) {
			dtkMsg.add(DTKMSG_ERROR, 
				   "Missing or invalid Z center parameter\n") ;
			usage(name) ;
			return 1 ;
		    } 
		    c++ ;
	  
		} else {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Not enough center parameters\n") ;
		    usage(name) ;
		    return 1 ;
		}
	    }
      
	} else if (!strncmp("--euler",argv[c],strlen(argv[c]))) {
	    c++ ;
	    if (c+2<argc) {

		if (DGLUtil::string_to_float(argv[c],&hpr[0])) {
		    dtkMsg.add(DTKMSG_ERROR,
			       "Missing or invalid H hpr parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&hpr[1])) {
		    dtkMsg.add(DTKMSG_ERROR,
			       "Missing or invalid P hpr parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&hpr[2])) {
		    dtkMsg.add(DTKMSG_ERROR,
			       "Missing or invalid R hpr parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "Not enough hpr parameters\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--scale",argv[c],strlen(argv[c]))) {
	    examine = 0 ;
	    bound = 0 ;
	    c++ ;
	    // first parameter is required, other two are optional
	    if (c<argc) {
		if (DGLUtil::string_to_float(argv[c],&(scale[0]))) {
		    dtkMsg.add(DTKMSG_ERROR,
			       "Invalid scale = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    if (scale[0] == 0.0f) {
			dtkMsg.add(DTKMSG_ERROR,
				   "Invalid scale = %s\n",argv[c]) ;
			usage(name) ;
			return 1 ;
		    }
		}
		c++ ;

		// if more than one parameter, then all three required
		// if Y is of invalid format, assume it's not a Y
		if (c<argc && !DGLUtil::string_to_float(argv[c],&(scale[1]))) {
		    if (scale[1] == 0.0f) {
			dtkMsg.add(DTKMSG_ERROR,
				   "Invalid Y scale = %s\n",argv[c]) ;
			usage(name) ;
			return 1 ;
		    }
		    c++ ;
	  
		    if ((c<argc) &&
			(DGLUtil::string_to_float(argv[c],&(scale[2]))) || 
			scale[2] == 0.0f) {
			dtkMsg.add(DTKMSG_ERROR,
				   "Invalid Z scale = %s\n",argv[c]) ;
			usage(name) ;
			return 1 ;
		    }
		    c++ ;
		} else { // Y not specified or no more args
		    scale[2] = scale[1] = scale[0] ;
		}
	    
	    } else { //if (c<argc) {
		dtkMsg.add(DTKMSG_ERROR,
			   "No scale parameter\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--bound",argv[c],strlen(argv[c]))) {
	    examine = 0 ;
	    bound = 1 ;
	    c++ ;
	    // parameter is optional
	    if (!DGLUtil::string_to_float(argv[c],&(scale[0]))) {
		c++ ;
	    } else {
		scale[0] = 1.0f ;
	    }

	    if (scale[0] == 0.0f) {
		dtkMsg.add(DTKMSG_ERROR,
			   "%s: Invalid bound value\n",name) ;
		usage(name) ;
		return 1 ;
	    }
	    scale[2] = scale[1] = scale[0] ;


	} else if (!strncmp("--",argv[c],1)) {
	    dtkMsg.add(DTKMSG_ERROR,
		       "%s: Unknown option %s\n",name,argv[c]) ;
	    usage(name) ;
	    return 1 ;
	} else {
	    break ;
	}
    } //while
  
    objv = (char **) malloc((argc-c)*sizeof(char *)) ;

    // the rest are files to load
    while (c<argc) {
	objv[objc] = argv[c] ;
	objc++ ;
	c++ ;
    }
 
    if (objc == 0) {
	dtkMsg.add(DTKMSG_ERROR, 
		   "%s: Not enough files specified, need at least one input \n"
		   "and exactly one output\n",name,argv[c]) ;
	usage(name);
	return 1;
    }

    return 0 ;
}


