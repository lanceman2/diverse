#ifndef _EXPORTS_H_
#define _EXPORTS_H_

#include <dgl/config.h>

#ifdef DGL_ARCH_WIN32
/* Prevent inclusion of winsock.h in windows.h */
/* Yes Windows is Kludgy */
#  ifndef _WINSOCKAPI_
#    define _WINSOCKAPI_
#  endif
#  include <windows.h>
#  undef _WINSOCKAPI_
#  ifdef DGL_EXPORTS
#    define DGLAPI __declspec(dllexport)
#  else
#    define DGLAPI __declspec(dllimport)
#  endif
#  ifdef DOSG_EXPORTS
#    define DOSGAPI __declspec(dllexport)
#  else
#    define DOSGAPI __declspec(dllimport)
#  endif
#  ifdef DCOIN_EXPORTS
#    define DCOINAPI __declspec(dllexport)
#  else
#    define DCOINAPI __declspec(dllimport)
#  endif
#  ifdef DVTK_EXPORTS
#    define DVTKAPI __declspec(dllexport)
#  else
#    define DVTKAPI __declspec(dllimport)
#  endif
#else
#  ifndef DGLAPI
#    define DGLAPI
#  endif
#  ifndef DOSGAPI
#    define DOSGAPI
#  endif
#  ifndef DCOINAPI
#    define DCOINAPI
#  endif
#  ifndef DVTKAPI
#    define DVTKAPI
#  endif
#endif

#endif

