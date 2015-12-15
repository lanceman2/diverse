// simple class to help make the mouse buttons easier to handle
// John Kelso, kelso@nist.gov, 5/06


#ifdef OLDPRODUCER
#include <Producer/KeyboardMouse>
#else
#include <Producer/KeyboardMouse.h>
#endif

#include <dtk.h>
#include <dgl.h>

DGLWindow*  DGLMouseButton::m_window = NULL ;

DGLMouseButton::MOUSEBUTTONMAP DGLMouseButton::state ;


DGLMouseButton::ACTION DGLMouseButton::getState(unsigned int b) 
{
  MOUSEBUTTONMAP::iterator pos = state.find(b) ;
  if (pos == state.end()) 
    return UNKNOWN ;
  else
    return static_cast<ACTION>(state[b]) ;
}

DGLMouseButton::DGLMouseButton(void) 
{
  m_mouse = new DGLMouse ;

  m_manager = (dtkManager*) DGL::getApp();
  if (m_manager == NULL || m_manager->isInvalid())
    {
      printf("DGLMouseButton::DGLMouseButton: Bad manager :(\n");
      return;
    }
  if (!(m_button = (dtkInButton*)m_manager->check("buttons")))
    {
      m_button = new dtkInButton(m_manager->record(), 4, "buttons");
      if (!m_button || m_button->isInvalid())
	dtkMsg.add(DTKMSG_ERROR,"DGLMouseButton::DGLMouseButton: failed to get dtkInButton.\n");
      m_button->setDescription("DGL mouse buttons with 4 buttons");
      if (m_manager->add(m_button)) return;
    }
  
#ifdef DGL_ARCH_WIN32
  m_mousebutton  = new dtkSharedMem(sizeof(RECORD), "dgl\\mousebutton");
#else
  m_mousebutton  = new dtkSharedMem(sizeof(RECORD), "dgl/mousebutton");
#endif
  m_mousebutton->queue() ;
  m_mousebutton->flush() ;
  m_window = NULL ;

}

// returns 0 if no mouse button changed, else number of mouse buttons changed,
// setting action
unsigned int DGLMouseButton::readMouseButton(ACTION *a) 
{
  RECORD r ;
  if (m_mousebutton->qread(&r))
    {
      *a = r.a ;
      return r.b ;
    }
  else
    {
      return 0 ;
    }
}

// returns 0 if no mouse button changed, else number of mouse buttons changed,
// setting position and action
unsigned int DGLMouseButton::readMouseButton(float *x, float *y, ACTION *a) 
{
  RECORD r ;
  if (m_mousebutton->qread(&r))
    {
      *x = r.x ;
      *y = r.y ;
      *a = r.a ;
      return r.b ;
    }
  else
    {
      return 0 ;
    }
}

// writes mouse button data
int DGLMouseButton::writeMouseButton(unsigned int b, float x, float y, ACTION a) 
{
  if (x>1.f)
    x=1.f ;
  if (x<-1.f)
    x=-1.f ;
  if (y>1.f)
    y=1.f ;
  if (y<-1.f)
    y=-1.f ;

  RECORD r ;
  r.b = b ;
  r.x = x ;
  r.y = y ;
  r.a = a ;
  m_mousebutton->write(&r) ;
  
  //Dtk voodoo
  if (a == PRESSED)
    {
      if (b == 1)
	m_button->write(0,1);
      else if (b == 2)
	m_button->write(1,1);
      else if (b == 3)
	m_button->write(2,1);
      else if (b == 4)
	m_button->write(3,1);
    }
  else if (a==RELEASED) 
    {
      if (b == 1)
	m_button->write(0,0);
      else if (b == 2)
	m_button->write(1,0);
      else if (b == 3)
	m_button->write(2,0);
      else if (b == 4)
	m_button->write(3,0);
    }

  return 0 ;

}

