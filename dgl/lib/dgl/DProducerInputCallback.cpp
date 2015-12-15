#include <dtk.h>
#include <dgl.h>

#ifdef OLDPRODUCER
#include <Producer/RenderSurface>
#include <Producer/Camera>
#include <Producer/KeyboardMouse>
#else
#include <Producer/RenderSurface.h>
#include <Producer/Camera.h>
#include <Producer/KeyboardMouse.h>
#endif

using namespace Producer;

void DMKCallback::setRS(RenderSurface* rs)
{
	m_rs = rs;
	m_win = DGL::display()->getWFromR(m_rs) ;
	//printf("DMKCallback::setRS: window = %p\n",m_win) ;
}

void DMKCallback::mouseMotion(float A, float B)
{
  m_mouse->setMouse(A,B) ;
  m_mouse->setWindow(m_win) ;
  return;
}

void DMKCallback::passiveMouseMotion(float A, float B)
{
  m_mouse->setMouse(A,B) ;
  m_mouse->setWindow(m_win) ;
  //printf("DMKCallback::passiveMouseMotion %f %f, rs=%p\n",A,B,m_rs) ;
  //printf("  window = %p\n",m_win) ;
  return;
}

void DMKCallback::buttonPress(float A, float B, unsigned int Button)
{
  m_mousebutton->writeMouseButton(Button, A, B, DGLMouseButton::PRESSED) ;
  m_mouse->setWindow(m_win) ;
}
void DMKCallback::buttonRelease(float A, float B, unsigned int Button)
{
  m_mousebutton->writeMouseButton(Button, A, B, DGLMouseButton::RELEASED) ;
  m_mouse->setWindow(m_win) ;
}
void DMKCallback::keyPress(KeyCharacter A)
{
  m_kb->writeKeyCharacter(A, DGLKeyboard::PRESSED) ;
  m_mouse->setWindow(m_win) ;
 }
void DMKCallback::keyRelease(KeyCharacter A)
{
  m_kb->writeKeyCharacter(A, DGLKeyboard::RELEASED) ;
  m_mouse->setWindow(m_win) ;
}
void DMKCallback::specialKeyRelease(KeyCharacter A)
{
  m_kb->writeKeyCharacter(A, DGLKeyboard::RELEASED) ;
  m_mouse->setWindow(m_win) ;
}
void DMKCallback::specialKeyPress(KeyCharacter A)
{
  m_kb->writeKeyCharacter(A, DGLKeyboard::PRESSED) ;
  m_mouse->setWindow(m_win) ;
  if (A == KeyChar_Escape && DGL::getShutdownOnEscape())
    shutdown();
}

void DMKCallback::shutdown()
{
	DGL::getApp()->state  &= ~(DTK_ISRUNNING);
}

//Code to setup the manager / buttons /pointers / other dtk required components
void DMKCallback::setup()
{
	m_kb = new DGLKeyboard ;
	m_mouse = new DGLMouse ;
	m_mousebutton = new DGLMouseButton ;
}
