#include <stdio.h>
#include <string.h>
#include <dosg.h>
#include "global.h"

// from usage.C
extern void usage(char *name);


//////////////////////////////////////////////////////////////
/////////////// global data //////////////////////////////////
//////////////////////////////////////////////////////////////
/* declared extern in global.h
 */


DGL *app = NULL;
DGLDisplay *display = NULL ;

int verbose = 0 ;

// allocated and set in processArgs
float *bound = NULL ;
dtkVec3 *center = NULL ;
char *name = NULL ;
float scale = 1.0f ;
float *response = NULL ;
float lod = 1.0f ;
float *interoccular = NULL ;
#ifndef UNIMPLEMENTED_NEARFAR
float *near = NULL ;
float *far = NULL ;
#endif
#ifndef UNIMPLEMENTED
float *convergence = NULL ;
#endif
float ambient = 0.f ;
float diffuse = 0.8f ;
float specular = 1.0f ;

// default origin
dtkVec3 *xyz = NULL ;
dtkVec3 *hpr = NULL ;

// files to be loaded under world node
std::vector<std::string> world ;

// files to be loaded under head node
std::vector<std::string> head ;

// files to be loaded under wand node
std::vector<std::string> wand ;

// files to be loaded under ether node
std::vector<std::string> ether ;

// files to be loaded under nav node
std::vector<std::string> nav ;

// files to be loaded under scene node
std::vector<std::string> scene ;

// files to be loaded under nochild node
std::vector<std::string> nochild ;

// true if NULL has been indicated as one of the files to load
int null = 0 ;

// true if --nolights specified
int nolights = 0 ;

// true if loading axis
int axis = 0 ;

// true if displaying splash screen
int splash = 0 ;
// true if splash screen times out
int timeout = 1 ;

// true if backface culling- default is to show backfaces
int backfacecull = 0 ;

// true if setting node names to file names- default is to not name nodes
int setname = 0 ;

// true if "--examine" was specified
int examine = 0 ;
float examineBound = 1.f ;
dtkVec3 examineCenter = dtkVec3(0.f, 2.f, 0.f) ;

// pivot, if any
dtkCoord *pivot = NULL ;

// lights to be loaded under scene node
std::vector<osg::Vec4> sceneLight ;

// lights to be loaded under ether node
std::vector<osg::Vec4> etherLight ;

// lights to be loaded under nav node
std::vector<osg::Vec4> navLight ;

// lights to be loaded under world node
std::vector<osg::Vec4> worldLight ;

// total number of lights
int numLights = 0 ;

// whether to dump the scenegraph on exit, and what filename to use
bool dumpOnExit = false ;
std::string dumpOnExitFilename = "saved_model.osg" ;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


int processArgs(int argc, char **argv) {

    int c ;
  
    name = strdup(argv[0]) ;
  
    if (argc<2) {
	usage(name) ;
	return 1 ;
    }
  
    // process arguments
    c = 1 ;
  
    while (c<argc) {
	if (!strncmp("--",argv[c],strlen(argv[c]))) {
	    usage(name) ;
	    return 1 ;
      
	} else if (!strncmp("--verbose",argv[c],strlen(argv[c]))) {
	    verbose = 1 ;
	    c++ ;
      
	} else if (!strncmp("--head",argv[c],strlen(argv[c]))) {
	    c++ ;
	    // the rest are files to load
	    while ((c<argc) && (strncmp(argv[c],"--",2))) {
		if (!strcmp(argv[c],"NULL")) {
		    null = 1 ;
		    break ;
		}
		head.push_back(argv[c]) ;
		c++ ;
	    }

	} else if (!strncmp("--wand",argv[c],strlen(argv[c]))) {
	    c++ ;
	    // the rest are files to load
	    while ((c<argc) && (strncmp(argv[c],"--",2))) {
		if (!strcmp(argv[c],"NULL")) {
		    null = 1 ;
		    break ;
		}
		wand.push_back(argv[c]) ;
		c++ ;
	    }

	} else if (!strncmp("--ether",argv[c],strlen(argv[c]))) {
	    c++ ;
	    // the rest are files to load
	    while ((c<argc) && (strncmp(argv[c],"--",2))) {
		if (!strcmp(argv[c],"NULL")) {
		    null = 1 ;
		    break ;
		}
		ether.push_back(argv[c]) ;
		c++ ;
	    }

	} else if (!strncmp("--nochild",argv[c],strlen(argv[c]))) {
	    c++ ;
	    // the rest are files to load
	    while ((c<argc) && (strncmp(argv[c],"--",2))) {
		if (!strcmp(argv[c],"NULL")) {
		    null = 1 ;
		    break ;
		}
		nochild.push_back(argv[c]) ;
		c++ ;
	    }

	} else if (!strncmp("--nav",argv[c],strlen(argv[c]))) {
	    c++ ;
	    // the rest are files to load
	    while ((c<argc) && (strncmp(argv[c],"--",2))) {
		if (!strcmp(argv[c],"NULL")) {
		    null = 1 ;
		    break ;
		}
		nav.push_back(argv[c]) ;
		c++ ;
	    }

	} else if (!strncmp("--scene",argv[c],strlen(argv[c]))) {
	    c++ ;
	    // the rest are files to load
	    while ((c<argc) && (strncmp(argv[c],"--",2))) {
		if (!strcmp(argv[c],"NULL")) {
		    null = 1 ;
		    break ;
		}
		scene.push_back(argv[c]) ;
		c++ ;
	    }

	} else if (!strncmp("--world",argv[c],strlen(argv[c]))) {
	    c++ ;
	    // the rest are files to load
	    while ((c<argc) && (strncmp(argv[c],"--",2))) {
		if (!strcmp(argv[c],"NULL")) {
		    null = 1 ;
		    break ;
		}
		world.push_back(argv[c]) ;
		c++ ;
	    }

	} else if (!strncmp("--nolights",argv[c],strlen(argv[c]))) {
	    c++ ;
	    nolights = 1 ;
      
	} else if (!strncmp("--scale",argv[c],strlen(argv[c]))) {
	    if (bound) {
		delete bound ;
		bound = NULL ;
	    }
	    c++ ;
	    if (c<argc) {
		if (DGLUtil::string_to_float(argv[c],&scale)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid scale = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else if (scale == 0.0f) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid scale = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "No scale parameter\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--response",argv[c],strlen(argv[c]))) {
	    if (c<argc) {
		c++ ;
		response = new float ;
		if ((c<argc) && (DGLUtil::string_to_float(argv[c],response))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid response = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else if (*response == 0.0f) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid response = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "No response parameter\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--length",argv[c],strlen(argv[c]))) {
	    if (bound) {
		delete bound ;
		bound = NULL ;
	    }
	    c++ ;
	    if (c<argc) {
		float length ;
		if (DGLUtil::string_to_float(argv[c],&length)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid length = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else if (length == 0.f) {
		    dtkMsg.add(DTKMSG_WARN, 
			       "Length == 0.0f, ignored\n") ;
		} else { 
		    scale = length/app->getMeters() ;
		    c++;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "No length parameter\n") ;
		usage(name) ;
		return 1 ;
	    }

#ifndef UNIMPLEMENTED_NEARFAR
	} else if (!strncmp("--near",argv[c],strlen(argv[c]))) {
	    c++ ;
	    float f ;
	    if (c<argc) {
		if (DGLUtil::string_to_float(argv[c],&f)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid near = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    near = new float ;
		    *near = f ;
		    DOSG::setAutoNearFar(false) ;
		    c++ ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "No near parameter\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--far",argv[c],strlen(argv[c]))) {
	    c++ ;
	    if (c<argc) {
		float f ;
		if (DGLUtil::string_to_float(argv[c],&f)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid far = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    far = new float ;
		    *far = f ;
		    DOSG::setAutoNearFar(false) ;
		    c++ ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "No far parameter\n") ;
		usage(name) ;
		return 1 ;
	    }
	} else if (!strncmp("--autonearfar",argv[c],strlen(argv[c]))) {
	    c++ ;
	    DOSG::setAutoNearFar(true) ;
#endif
#ifndef UNIMPLEMENTED      
	} else if (!strncmp("--convergence",argv[c],strlen(argv[c]))) {
	    c++ ;
	    if (c<argc) {
		float f ;
		if (DGLUtil::string_to_float(argv[c],&f)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid convergence = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    convergence = new float ;
		    *convergence = f ;
		    //display->convergence(convergence) ;
		    c++ ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "No convergence parameter\n") ;
		usage(name) ;
		return 1 ;
	    }
#endif      
	} else if (!strncmp("--interoccular",argv[c],strlen(argv[c]))) {
	    c++ ;
	    if (c<argc) {
		float f ;
		if (DGLUtil::string_to_float(argv[c],&f)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid interoccular = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    interoccular = new float ;
		    *interoccular = f ;
		    //display->getPipe(0)->getWindow(0)->getScreen(0)->setInterOccular(interoccular) ;
		    c++ ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "No interoccular parameter\n") ;
		usage(name) ;
		return 1 ;
	    }
      

#ifndef UNIMPLEMENTED
	} else if (!strncmp("--lod",argv[c],strlen(argv[c]))) {
	    c++ ;
	    if (c<argc) {
		if (DGLUtil::string_to_float(argv[c],&lod)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid lod = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "No lod parameter\n") ;
		usage(name) ;
		return 1 ;
	    }
#endif
      
	} else if (!strncmp("--origin",argv[c],strlen(argv[c]))) {
	    if (!xyz)
		xyz = new dtkVec3 ;
	    if (!hpr) 
		hpr = new dtkVec3 ;
	    if (center) {
		delete center ;
		center = NULL ;
	    }
	    c++ ;
	    if (c+5<argc) {

		if (DGLUtil::string_to_float(argv[c],&(xyz->x))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "Invalid X value of origin = %s\n",argv[c]) ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&(xyz->y))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Y origin parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&(xyz->z))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Z origin parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&(hpr->h))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No H origin parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&(hpr->p))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No P origin parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&(hpr->r))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No R origin parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "Not enough origin parameters\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--euler",argv[c],strlen(argv[c]))) {
	    if (!hpr)
		hpr = new dtkVec3 ;
	    c++ ;
	    if (c+2<argc) {

		if (DGLUtil::string_to_float(argv[c],&(hpr->h))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No H hpr parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&(hpr->p))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No P hpr parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&(hpr->r))) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No R hpr parameter\n") ;
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
      
	} else if (!strncmp("--scenelight",argv[c],strlen(argv[c]))) {
	    c++ ;
	    float x,y,z,w;
	    if (c+2<argc) {
	
		if (DGLUtil::string_to_float(argv[c],&x)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No X scenelight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&y)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Y scenelight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&z)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Z scenelight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}
		// w parameter is optional
		if (!DGLUtil::string_to_float(argv[c],&w)) {
		    c++ ;
		} else {
		    w = 0.f ;
		}

		sceneLight.push_back(osg::Vec4(x,y,z, w)) ;
		if (verbose) {
		    printf("adding light to scene() at (%f, %f, %f, %f)\n",x,y,z,w) ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "Not enough scenelight parameters\n") ;
		usage(name) ;
		return 1 ;
	    }

	} else if (!strncmp("--pivot",argv[c],strlen(argv[c]))) {
	    c++ ;
	    float x,y,z,h,p,r;
	    h = p = r = 0.0f ;
	    if (c+2<argc) {
	
		if (DGLUtil::string_to_float(argv[c],&x)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No X pivot parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&y)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Y pivot parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&z)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Z pivot parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		    pivot = new dtkCoord ;
		    pivot->set(x,y,z,h,p,r) ;
		}

		// see if HPR specified
		if (!DGLUtil::string_to_float(argv[c],&h)) {
		    c++ ;
		    // H gotten, so we're committed to P and R
		    if (c+1<argc) { 
			if (DGLUtil::string_to_float(argv[c],&p)) {
			    dtkMsg.add(DTKMSG_ERROR, 
				       "No P pivot parameter\n") ;
			    usage(name) ;
			    return 1 ;
			} else {
			    c++ ;
			}
			if (DGLUtil::string_to_float(argv[c],&r)) {
			    dtkMsg.add(DTKMSG_ERROR, 
				       "No R pivot parameter\n") ;
			    usage(name) ;
			    return 1 ;
			} else {
			    c++ ;
			    pivot->set(x,y,z,h,p,r) ;
			}
		    } else {
			dtkMsg.add(DTKMSG_ERROR, 
				   "Not enough HPR pivot parameters\n") ;
			usage(name) ;
			return 1 ;
		    }


		}

	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "Not enough pivot parameters\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--etherlight",argv[c],strlen(argv[c]))) {
	    c++ ;
	    float x,y,z,w;
	    if (c+2<argc) {
	
		if (DGLUtil::string_to_float(argv[c],&x)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No X etherlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&y)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Y etherlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&z)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Z etherlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}
		// w parameter is optional
		if (!DGLUtil::string_to_float(argv[c],&w)) {
		    c++ ;
		} else {
		    w = 0.f ;
		}

		etherLight.push_back(osg::Vec4(x,y,z, w)) ;
		if (verbose) {
		    printf("adding light to ether() at (%f, %f, %f, %f)\n",x,y,z,w) ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "Not enough etherlight parameters\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--navlight",argv[c],strlen(argv[c]))) {
	    c++ ;
	    float x,y,z,w;
	    if (c+2<argc) {
	
		if (DGLUtil::string_to_float(argv[c],&x)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No X navlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&y)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Y navlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&z)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Z navlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}
		// w parameter is optional
		if (!DGLUtil::string_to_float(argv[c],&w)) {
		    c++ ;
		} else {
		    w = 0.f ;
		}

		navLight.push_back(osg::Vec4(x,y,z, w)) ;
		if (verbose) {
		    printf("adding light to nav() at (%f, %f, %f, %f)\n",x,y,z,w) ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "Not enough navlight parameters\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--worldlight",argv[c],strlen(argv[c]))) {
	    c++ ;
	    float x,y,z,w;
	    if (c+2<argc) {
	
		if (DGLUtil::string_to_float(argv[c],&x)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No X worldlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&y)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Y worldlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}

		if (DGLUtil::string_to_float(argv[c],&z)) {
		    dtkMsg.add(DTKMSG_ERROR, 
			       "No Z worldlight parameter\n") ;
		    usage(name) ;
		    return 1 ;
		} else {
		    c++ ;
		}
		// w parameter is optional
		if (!DGLUtil::string_to_float(argv[c],&w)) {
		    c++ ;
		} else {
		    w = 0.f ;
		}

		worldLight.push_back(osg::Vec4(x,y,z, w)) ;
		if (verbose) {
		    printf("adding light to world() at (%f, %f, %f, %f)\n",x,y,z,w) ;
		}
	    } else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "Not enough worldlight parameters\n") ;
		usage(name) ;
		return 1 ;
	    }
      
	} else if (!strncmp("--center",argv[c],strlen(argv[c]))) {
	    if (xyz)
	    {
		delete xyz ;
		xyz = NULL ;
	    }
	    if (hpr)
	    {
		delete hpr ;
		hpr = NULL ;
	    }
	    c++ ;
	    if (!center) 
		center = new dtkVec3 ;
	    *center = dtkVec3(0.f, 0.f, 0.f) ;
	    // parameters are optional, but all three are required if specified
	    // test first one to see if it's a number
	    if (c<argc) {
		float foo ;
		if (!DGLUtil::string_to_float(argv[c],&foo)) {
		    (*center).x = foo ;
		    c++ ;
		    if (c+1<argc) {
	    
			if (DGLUtil::string_to_float(argv[c],&foo)) {
			    dtkMsg.add(DTKMSG_ERROR, 
				       "No Y center parameter\n") ;
			    usage(name) ;
			    return 1 ;
			} else {
			    (*center).y = foo ;
			    c++ ;
			}
	    
			if (DGLUtil::string_to_float(argv[c],&foo)) {
			    dtkMsg.add(DTKMSG_ERROR, 
				       "No Z center parameter\n") ;
			    usage(name) ;
			    return 1 ;
			} else {
			    (*center).z = foo ;
			    c++ ;
			}
	    
		    } else {
			dtkMsg.add(DTKMSG_ERROR, 
				   "Not enough center parameters\n") ;
			usage(name) ;
			return 1 ;
		    }
		}
	    } 
      
      
	} else if (!strncmp("--bound",argv[c],strlen(argv[c]))) {
	    if (!bound) 
		bound = new float ;

	    c++ ;
	    // parameter is optional
	    if ((c<argc) && (!DGLUtil::string_to_float(argv[c],bound))) {
		c++ ;
	    } else {
		*bound = 1.0f ;
	    }
      
	    if (*bound <= 0.0f) {
		dtkMsg.add(DTKMSG_ERROR, 
			   "%s: Invalid bound value\n",name) ;
		usage(name) ;
		return 1 ;
	    }

	} else if (!strncmp("--ambient",argv[c],strlen(argv[c]))) {
	    c++ ;
	    if ((!DGLUtil::string_to_float(argv[c],&ambient)) && 
		(ambient >= 0.0f) && (ambient <= 1.0f)) {
		c++ ;
	    }
	    else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "%s: Invalid ambient value\n",name) ;
		usage(name) ;
		return 1 ;
	    }

	} else if (!strncmp("--diffuse",argv[c],strlen(argv[c]))) {
	    c++ ;
	    if ((!DGLUtil::string_to_float(argv[c],&diffuse)) && 
		(diffuse >= 0.0f) && (diffuse <= 1.0f)) {
		c++ ;
	    }
	    else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "%s: Invalid diffuse value\n",name) ;
		usage(name) ;
		return 1 ;
	    }

	} else if (!strncmp("--specular",argv[c],strlen(argv[c]))) {
	    c++ ;
	    if ((!DGLUtil::string_to_float(argv[c],&specular)) && 
		(specular >= 0.0f) && (specular <= 1.0f)) {
		c++ ;
	    }
	    else {
		dtkMsg.add(DTKMSG_ERROR, 
			   "%s: Invalid specular value\n",name) ;
		usage(name) ;
		return 1 ;
	    }

	} else if (!strncmp("--dumponexit",argv[c],strlen(argv[c]))) {
	    dumpOnExit = true ;
	    c++ ;
	    // parameter is optional
	    if (c<argc) {
		dumpOnExitFilename = argv[c] ;
		c++ ;
	    }
      
	} else if (!strncmp("--examine",argv[c],strlen(argv[c]))) {
	    if (xyz)
	    {
		delete xyz ;
		xyz = NULL ;
	    }
	    if (hpr)
	    {
		delete hpr ;
		hpr = NULL ;
	    }

	    if (!bound) 
		bound = new float ;
	    *bound = examineBound ;

	    if (!center) 
		center = new dtkVec3 ;
	    *center = examineCenter ;

	    examine = 1 ;

	    c++ ;

#ifndef UNIMPLEMENTED
	} else if (!strncmp("--axis",argv[c],strlen(argv[c]))) {
	    c++ ;
	    axis = 1 ;
#endif

#ifndef UNIMPLEMENTED
	} else if (!strncmp("--splash",argv[c],strlen(argv[c]))) {
	    c++ ;
	    splash = 1 ;
	    timeout = 1 ;
#endif
#ifndef UNIMPLEMENTED
	} else if (!strncmp("--splashnotimeout",argv[c],strlen(argv[c]))) {
	    c++ ;
	    splash = 1 ;
	    timeout = 0 ;
#endif

	} else if (!strncmp("--backfacecull",argv[c],strlen(argv[c]))) {
	    c++ ;
	    backfacecull = 1 ;

	} else if (!strncmp("--setname",argv[c],strlen(argv[c]))) {
	    c++ ;
	    setname = 1 ;

	} else if (!strncmp("--",argv[c],1)) {
	    dtkMsg.add(DTKMSG_ERROR, 
		       "%s: Unknown option %s\n",name,argv[c]) ;
	    usage(name) ;
	    return 1 ;
	} else {
	    // implied --world file
	    // the rest are files to load
	    while ((c<argc) && (strncmp(argv[c],"--",2))) {
		if (!strcmp(argv[c],"NULL")) {
		    null = 1 ;
		    c++ ;
		    break ;
		}
		world.push_back(argv[c]) ;
		c++ ;
	    }
	}
    } //while
    
    return 0 ;
}

