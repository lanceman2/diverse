//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/////////////// global data //////////////////////////////////
//////////////////////////////////////////////////////////////
/* All this data is declared in processArgs.C
 */

#define UNIMPLEMENTED
//#define UNIMPLEMENTED_NEARFAR

extern DGL *app;
extern DGLDisplay *display;

extern int verbose;

// allocated and set in processArgs
extern float *bound;
extern dtkVec3 *center;
extern char *name;
extern float scale;
extern float *response;
extern float lod;
extern float *interoccular ;
#ifndef UNIMPLEMENTED_NEARFAR
#  ifdef DGL_ARCH_WIN32
#    undef near
#    undef far
#    define near near
#    define far far
#  endif
extern float *near ;
extern float *far ;
#endif
#ifndef UNIMPLEMENTED
extern float *convergence ;
#endif
extern float ambient, diffuse, specular ;

// default origin
extern dtkVec3 *xyz;
extern dtkVec3 *hpr;

// number of files to load under world, and their names
extern std::vector<std::string> world ;

// number of files to load under head, and their names
extern std::vector<std::string> head ;

// number of files to load under wand, and their names
extern std::vector<std::string> wand ;

// number of files to load under ether, and their names
extern std::vector<std::string> ether ;

// number of files to load under nav, and their names
extern std::vector<std::string> nav ;

// number of files to load under scene, and their names
extern std::vector<std::string> scene ;

// number of files to load without children
extern std::vector<std::string> nochild ;

// true if --nolights specified
extern int nolights;

// true if NULL has been indicated as one of the files to load
extern int null;

// true if loading axis
extern int axis;

// true if displaying splash screen
extern int splash;

// true if splash screen times out
extern int timeout;

// true if backface culling- default is to show backfaces
extern int backfacecull;

// true if setting node names to file names- default is to not name nodes
extern int setname;

// true if "--examine" was specified
extern int examine;
extern float examineBound ;
extern dtkVec3 examineCenter ;

// pivot, if any
extern dtkCoord *pivot;

// number of scenelights to load, and their positions
extern std::vector<osg::Vec4> sceneLight ;

// number of etherlights to load, and their positions
extern std::vector<osg::Vec4> etherLight ;

// number of navlights to load, and their positions
extern std::vector<osg::Vec4> navLight ;

// number of worldlights to load, and their positions
extern std::vector<osg::Vec4> worldLight ;

// total number of lights
extern int numLights ;

// whether to dump the scenegraph on exit, and what filename to use
extern bool dumpOnExit ;
extern std::string dumpOnExitFilename ;

