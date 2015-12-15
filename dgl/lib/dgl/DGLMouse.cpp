// simple class to help make the mouse easier to handle
// John Kelso, kelso@nist.gov, 5/06


#ifdef OLDPRODUCER
#include <Producer/KeyboardMouse>
#else
#include <Producer/KeyboardMouse.h>
#endif

#include <dtk.h>
#include <dgl.h>

DGLWindow*  DGLMouse::m_window = NULL ;

// write new mouse data
int DGLMouse::setMouse(float x, float y) {
  if (x>1.f)
    x=1.f ;
  if (x<-1.f)
    x=-1.f ;
  if (y>1.f)
    y=1.f ;
  if (y<-1.f)
    y=-1.f ;
  m_position[0] = x ;
  m_position[1] = y ;
  m_mouse->write(m_position) ;
  m_pointer->write(m_position) ;
  return 0 ;
}

// read mouse, queued, like dtkSharedMem::qRead(), returns 0 if no new
// data, -1 on error, else number of items on queue
int  DGLMouse::readMouse(float *x, float *y) {
  float buff[2] ;
  int c = m_mouse->qread(buff) ;
  if (c>0) 
    {
      *x=buff[0] ;
      *y=buff[1] ;
    }
  return c ;
}

// read current mouse position, like dtkSharedMem::read(), returns
// non-zero on error
int  DGLMouse::getMouse(float *x, float *y) {
  float buff[2] ;
  m_mouse->read(buff) ;
  *x=buff[0] ;
  *y=buff[1] ;
  return 0 ;
}

DGLMouse::DGLMouse(void) {
  m_manager = (dtkManager*) DGL::getApp();
  if (m_manager == NULL || m_manager->isInvalid())
    {
      printf("DGLMouse::DGLMouse: Bad manager :(\n");
      return;
    }
  
  if (!(m_pointer = (dtkInValuator*)m_manager->check("pointer")))
    {
      m_pointer = new dtkInValuator(m_manager->record(),2,"pointer");
      if (!m_pointer || m_pointer->isInvalid())
	dtkMsg.add(DTKMSG_ERROR,"DGLMouse::DGLMouse: failed to get dtkInValuator.\n");
      m_pointer->setDescription("DGL mouse pointer");
      if (m_manager->add(m_pointer)) return ;
    }

#ifdef DGL_ARCH_WIN32
  m_mouse    = new dtkSharedMem(sizeof(float)*2, "dgl\\mouse");
#else
  m_mouse    = new dtkSharedMem(sizeof(float)*2, "dgl/mouse");
#endif
  m_mouse->queue(0) ;
  m_mouse->flush() ;
  setMouse(0.f, 0.f) ;
  m_window = NULL ;
  return ;
}
