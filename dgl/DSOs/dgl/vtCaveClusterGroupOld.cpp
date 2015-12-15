#include <dtk.h>
#include <dgl.h>
#include <dtk/dtkDSO_loader.h>

/*************** DTK C++ DSO loader/unloader functions ***********/
/*
 * All DTK DSO files are required to declare these two functions.
 * These function are called by the loading program to get your
 * C++ objects loaded.
 *
 *****************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{

  //Configure everything to the VT Cave
  //Change the server machine name if you have a different server
  //Change the client machines to be your list of clients for your site
  //Change the specific display DSOS for your site also
#ifdef DGL_ARCH_WIN32
  SetEnvironmentVariable("DADS_SERVER", "console");
  SetEnvironmentVariable("DADS_CLIENTS", "dads1:dads2:dads3:dads4");
#elif defined IRIX
  putenv("DADS_SERVER=console");
  putenv("DADS_CLIENTS=dads1:dads2:dads3:dads4");
#else
  setenv("DADS_SERVER", "console", 1);
  setenv("DADS_CLIENTS", "dads1:dads2:dads3:dads4", 1);
#endif

  char* TEMP = getenv("DADS_DSO_FILES");
  if (TEMP == NULL)
  {
#ifdef DGL_ARCH_WIN32
  SetEnvironmentVariable("DADS_DSO_FILES", "vtCaveClusterClientGroup");
#elif defined IRIX
  putenv("DADS_DSO_FILES=vtCaveClusterClientGroup");
#else

  setenv("DADS_DSO_FILES", "vtCaveClusterClientGroup", 1);
#endif
  }
  else
  {
  string Concat = string( "vtCaveClusterClientGroup" ) +
      string( ":" ) + string(TEMP);
#ifdef DGL_ARCH_WIN32
  SetEnvironmentVariable("DADS_DSO_FILES", Concat.c_str());
#elif defined IRIX
  char* temp = new char[23+ Concat.size()];
    strcpy(temp, "DADS_DSO_FILES=");
    strcat(temp, Concat.c_str());
  putenv(temp);
#else
  setenv("DADS_DSO_FILES", Concat.c_str(),1);
#endif
  }
#ifdef DGL_ARCH_WIN32
  SetEnvironmentVariable("DADS_DSO_FILES_dads1", "vtCaveDisplayFront");
  SetEnvironmentVariable("DADS_DSO_FILES_dads2", "vtCaveDisplayFloor");
  SetEnvironmentVariable("DADS_DSO_FILES_dads3", "vtCaveDisplayRight");
  SetEnvironmentVariable("DADS_DSO_FILES_dads4", "vtCaveDisplayLeft");
#elif defined IRIX
  putenv("DADS_DSO_FILES_dads1=vtCaveDisplayFront");
  putenv("DADS_DSO_FILES_dads2=vtCaveDisplayFloor");
  putenv("DADS_DSO_FILES_dads3=vtCaveDisplayRight");
  putenv("DADS_DSO_FILES_dads4=vtCaveDisplayLeft");
#else
  setenv("DADS_DSO_FILES_dads1", "vtCaveDisplayFront",1);
  setenv("DADS_DSO_FILES_dads2", "vtCaveDisplayFloor",1);
  setenv("DADS_DSO_FILES_dads3", "vtCaveDisplayRight",1);
  setenv("DADS_DSO_FILES_dads4", "vtCaveDisplayLeft",1);
#endif
  DGL *app = static_cast<DGL*>(manager);
#ifdef DGL_ARCH_WIN32
  if(app->load("desktopCaveEmulateGroup;starter;navWrite"))
#else
  if(app->load("desktopCaveEmulateGroup:starter:navWrite"))
#endif
    return DTKDSO_LOAD_ERROR;
  else
    return DTKDSO_LOAD_UNLOAD;
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}


