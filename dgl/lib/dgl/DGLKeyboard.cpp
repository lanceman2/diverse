// simple class to help make the keyboard easier to handle
// John Kelso, kelso@nist.gov, 4/06


#ifdef OLDPRODUCER
#include <Producer/KeyboardMouse>
#else
#include <Producer/KeyboardMouse.h>
#endif

#include <dtk.h>
#include <dgl.h>

DGLWindow*  DGLKeyboard::m_window = NULL ;

DGLKeyboard::KEYMAP DGLKeyboard::state ;

bool DGLKeyboard::isSpecial(KeyCharacter A)
{
  if (A<0x007F && A>0x001F)
    return false ;
  else
    return true ;
}

char DGLKeyboard::getChar(KeyCharacter A)
{
  if (isSpecial(A))
    return 0 ;
  else
    return char(A) ;
}

DGLKeyboard::ACTION DGLKeyboard::getState(KeyCharacter A)
{
  KEYMAP::iterator pos = state.find(A) ;
  if (pos == state.end()) 
    return UNKNOWN ;
  else
    return static_cast<ACTION>(state[A]) ;
}

DGLKeyboard::DGLKeyboard(void)
{
#ifdef DGL_ARCH_WIN32
  m_keyboard = new dtkSharedMem(2*sizeof(KeyCharacter), "dgl\\keyboard"); 
#else
  m_keyboard = new dtkSharedMem(2*sizeof(KeyCharacter), "dgl/keyboard"); 
#endif
  m_keyboard->queue() ;
  m_keyboard->flush() ;
  m_window = NULL ;
  return ;
}

KeyCharacter DGLKeyboard::readKeyCharacter(ACTION *a)
{
  if (m_keyboard->qread(buff)) 
    {
      *a = static_cast<ACTION>(buff[1]) ;
      state[buff[0]] = buff[1] ;
      return buff[0] ;
    }
  else
    {
      *a = UNKNOWN ;
      return KeyCharacter(0) ;
    }
}

int DGLKeyboard::writeKeyCharacter(KeyCharacter A, ACTION a)
{
  if (a==PRESSED || a==RELEASED)
    {
      buff[0] = A ;
      buff[1] =  static_cast<KeyCharacter>(a) ;
      m_keyboard->write(buff) ;
      return 0 ;
    }
  else
    return 1 ;
}
