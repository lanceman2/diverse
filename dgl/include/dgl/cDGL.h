#ifndef _CDGL_H_
#define _CDGL_H_

/// @author Andrew A. Ray, anray2@vt.edu

/// \brief pointer to a display callback function.
typedef void DGLAPI (*dglDisplayCB)(void);
/// \brief pointer to a preConfig callback function.
typedef void DGLAPI (*dglPreConfigCB)(void);
/// \brief pointer to a postConfig callback function.
typedef void DGLAPI (*dglPostConfigCB)(void);
/// \brief pointer to a preProcess callback function.
typedef void DGLAPI (*dglPreFrameCB)(void);
/// \brief pointer to a postProcess callback function.
typedef void DGLAPI (*dglPostFrameCB)(void);
/// \brief pointer to a preNavigation callback
typedef void DGLAPI (*dglPreNavCB)(void);
/// \brief pointer to an overlay callback
typedef void DGLAPI (*dglOverlayCB)(void);
/// \brief pointer to a setup 
typedef void DGLAPI (*dglSetupGLCB)(void);
/// \brief pointer to an init callback
typedef void DGLAPI (*dglInitGLCB)(void);
/// \brief pointer to a display callback function.
typedef void DGLAPI (*dglDisplayLeftCB)(void);
/// \brief pointer to a display callback function.
typedef void DGLAPI (*dglDisplayRightCB)(void);
extern "C"
{
  /// \fn void DGLAPI dglInit();
  /// \brief invokes DGL::init()
  void DGLAPI dglInit();

  void DGLAPI dglRun();
  void DGLAPI dglStart();
  bool DGLAPI dglIsRunning();
  void DGLAPI dglFrame();
  void DGLAPI dglPreConfigCallback( dglPreConfigCB cb);
  void DGLAPI dglPostConfigCallback( dglPostConfigCB cb);
  void DGLAPI dglDisplayCallback( dglDisplayCB cb);
  void DGLAPI dglPreFrameCallback( dglPreFrameCB cb);
  void DGLAPI dglPostFrameCallback( dglPostFrameCB cb);
  void DGLAPI dglOverlayCallback(dglOverlayCB cb);
  void DGLAPI dglPreNavCallback(dglPreNavCB cb);
  void DGLAPI *dglGetData();
  void DGLAPI dglSetData(void* Data);
  void DGLAPI dglSetupGLCallback(dglSetupGLCB cb);
  void DGLAPI dglInitGLCallback(dglInitGLCB cb);
  void DGLAPI dglSetDiscreteStereo(bool discreteStereo);
  void DGLAPI dglDisplayLeftCallback(dglDisplayCB cb);
  void DGLAPI dglDisplayRightCallback(dglDisplayCB cb);
}
#endif
