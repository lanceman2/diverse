/***************************************************************************
                          DVtkRenderWindow.h  -  Version 0.3.2
                             -------------------
    begin                : Fri Apr 26 2002
    copyright            : (C) 2002 by Michael Shore
    email                : mshore@vt.edu
 ***************************************************************************/

#ifndef VTKDGLRENDERWINDOW_H
#define VTKDGLRENDERWINDOW_H

#include <vtkOpenGLRenderWindow.h>

#include <dgl.h>

class DVtkRenderer;
class DVtkWandRenderer;


/*!
 * @class DVtkRenderWindow
 *
 * @brief The DIVERSE interface to VTK
 *
 * This class provides a neutered vtkRenderWindow class
 * for DVtkRenderer to interact with.
 *
 * Since DGL handles all our windowing needs, we don't want VTK's RenderWindows
 * to get in our way!
 *
 * @author Michael Shore
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DVTKAPI DVtkRenderWindow : public vtkOpenGLRenderWindow
{
 public:
  
  /*!
   * @brief Create a new DVtkRenderWindow
   */
  static DVtkRenderWindow* New();
  
  /*!
   * @brief Add the renderer into the renderer collection
   *
   * @param newRen renderer to be added
   */
  void AddRenderer(DVtkRenderer *newRen);

  /*!
   * @brief Add the renderer into the renderer collection
   *
   * @param newRen renderer to be added
   */
  void AddRenderer(DVtkWandRenderer *newRen);
  
  /*!
   * @brief Render the scene
   */
  void Render();
  
  /*!
   * @brief Currently a no-op
   */
  void MakeCurrent();
  
  //Definition of unused virtual methods
  
  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void SetDisplayId(void*){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void SetWindowId(void*){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void SetNextWindowId(void*){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void SetParentId(void*){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void* GetGenericDisplayId(){return NULL;}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void* GetGenericWindowId(){return NULL;}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void* GetGenericParentId(){return NULL;}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void* GetGenericContext(){return NULL;}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void* GetGenericDrawable(){return NULL;}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void SetWindowInfo(char*){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void SetNextWindowInfo(char*){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void SetParentInfo(char*){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void Start(){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void Frame(){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void HideCursor(){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void ShowCursor(){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void SetFullScreen(int){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  void WindowRemap(){}

  /*!
   * @brief Definition of unused virtual method. Not implemented.
   */
  int  GetEventPending(){return 0;}

  /*!
   * @brief Definition of unused virtual method. Not implemented
   */
  int* GetScreenSize(){return 0;}

  /*!
   * @brief Definition of unused virtual method. Not implemented
   */
  void  Finalize() {};
/*!
   * @brief Definition of unused virtual method. Not implemented
   */
  bool IsCurrent() { return true;}


  
#if VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION >= 2

virtual void CreateAWindow() {};
virtual void DestroyWindow() {};

#endif
  
 protected:
  DVtkRenderWindow();
  ~DVtkRenderWindow();
  
  DVtkRenderer *ren;
  DVtkWandRenderer *wandren;
  
 private:
  DVtkRenderWindow(const DVtkRenderWindow*); // not implemented
  void operator= (const DVtkRenderWindow*); // not implemented
  
};

#endif
