#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

// how fast to move around in jump mode
#define XYZ (.5f)
#define HPR (50.f)

/*!
 * @class simulatorKeyboardControl 
 *
 * @brief DSO which reads the keyboard and sends message to control the
 * simulator DSO of your choice
 *
 * -  'h' or 'H'		
 *   - toggle display of head/wand objects
 * -  'j' or 'J'		
 *   - toggle jump view
 * -  '4'/'6' or Keypad Left/Right	
 *   - rotate jump view left (+H)/right (-H)
 * -  '2'/'8' or Keypad Up/Down	
 *   - rotate jump view up (+P)/down (-P)
 * -  Keypad -/+		
 *   - move jump view in (-Y)/out (+Y)
 * -  '5' or Keypad Begin	
 *   - reset jump view 
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class simulatorKeyboardControl : public dtkAugment
{
public:
  simulatorKeyboardControl(dtkManager *);
  
  virtual int postFrame(void) ;
  virtual void sendJumpTo(void) ;
  virtual void jump_left(int s) ;
  virtual void jump_up(int s) ;
  virtual void jump_in(int s) ;
  virtual void jump_reset(void) ;

private:
  
  // dtkTime is a gettimeofday() wrapper
  dtkTime t;

  DGLKeyboard   *kb ;
  DGLMessage    *msg ;
  DGLPostOffice *msgSender ;
  // jump position
  float x, y, z, h, p, r ;
  float delta_t ;

  // for using H key for both head and wand visibility
  int headWand ;

  // object, jump status
  int head ;
  int wand ;
  int jump ;

};


simulatorKeyboardControl::simulatorKeyboardControl(dtkManager *m) :
  dtkAugment("simulatorKeyboardControl")
{
  setDescription("reads the keyboard and sets dtk shared memory segments to "
		 "control the simulator") ;
  
  kb = new DGLKeyboard ;

  //Class for sending the message 
  msgSender = new DGLPostOffice("dgl/DGLSimulator") ;
  msg = new DGLMessage ;

  head = 0 ;
  wand = 0 ;
  jump = 0 ;

  headWand = 0 ;

  validate() ;
}

void simulatorKeyboardControl::sendJumpTo(void) 
{
  msg->clear() ;
  msg->setSubject("jumpTo");
  msg->addFloat(x) ;
  msg->addFloat(y) ;
  msg->addFloat(z) ;
  msg->addFloat(h) ;
  msg->addFloat(p) ;
  msg->addFloat(r) ;
  msgSender->sendMessage(msg);
}

void simulatorKeyboardControl::jump_left(int s)
{
  float sine = sinf(DEG2RAD(HPR*delta_t));
  float cosine = cosf(DEG2RAD(HPR*delta_t));
  h += s*HPR*delta_t;
  float savex = (x*cosine - s*y*sine);
  y = (s*x*sine + y*cosine);
  x = savex;
}

void simulatorKeyboardControl::jump_up(int s)
{
  p -= s*HPR*delta_t;
  float d = sqrtf(x*x + y*y + z*z) ;
  x =   d*sinf(DEG2RAD(h)) * cosf(DEG2RAD(p));
  y = - d*cosf(DEG2RAD(h)) * cosf(DEG2RAD(p));
  z = - d*sinf(DEG2RAD(p));
}

void simulatorKeyboardControl::jump_in(int s)
{
  float d = sqrtf(x*x + y*y + z*z) ;
  if((d > 0.01 || s < 0) && (d < 10000.0f || s> 0))
    {
      x -= s*x*XYZ*delta_t;
      y -= s*y*XYZ*delta_t;
      z -= s*z*XYZ*delta_t;
    }
}

void simulatorKeyboardControl::jump_reset(void)
{
  /// initial value of jump outside view
  x = 1.f;
  y = 0.f ;
  z = 0.f ;
  h = 90.f ;
  p = 0.f ;
  r = 0.f ;
}


int simulatorKeyboardControl::postFrame(void)
{

  static bool first = true ;
  if (first)
    {
      t.reset(0.0,1);
      first = false ;
      jump_reset() ;
      sendJumpTo() ;
    }
  
  delta_t = (float) t.delta();

  DGLKeyboard::ACTION a ;
  KeyCharacter A = kb->readKeyCharacter(&a) ;

  // do rate based keys, just poll to see if pressed
  if (DGLKeyboard::getState(KeyChar_KP_Left)==DGLKeyboard::PRESSED ||
      DGLKeyboard::getState(KeyChar_4)==DGLKeyboard::PRESSED)
    {
      jump_left(1) ;
      sendJumpTo() ;
    }
  else if (DGLKeyboard::getState(KeyChar_KP_Right)==DGLKeyboard::PRESSED ||
      DGLKeyboard::getState(KeyChar_6)==DGLKeyboard::PRESSED)
    {
      jump_left(-1) ;
      sendJumpTo() ;
    }
  else if (DGLKeyboard::getState(KeyChar_KP_Up)==DGLKeyboard::PRESSED ||
      DGLKeyboard::getState(KeyChar_8)==DGLKeyboard::PRESSED)
    {
      jump_up(1) ;
      sendJumpTo() ;
    }
  else if (DGLKeyboard::getState(KeyChar_KP_Down)==DGLKeyboard::PRESSED ||
      DGLKeyboard::getState(KeyChar_2)==DGLKeyboard::PRESSED)
    {
      jump_up(-1) ;
      sendJumpTo() ;
    }
  else if (DGLKeyboard::getState(KeyChar_KP_Add)==DGLKeyboard::PRESSED)
    {
      jump_in(-1) ;
      sendJumpTo() ;
    }
  else if (DGLKeyboard::getState(KeyChar_KP_Subtract)==DGLKeyboard::PRESSED)
    {
      jump_in(1) ;
      sendJumpTo() ;
    }
  
  // do actions caused by a button release
  else if (A && a==DGLKeyboard::RELEASED)
    {
      if (A == KeyChar_h || A == KeyChar_H)
	{
	  headWand = (headWand+1)%4 ;
	  if (headWand == 0)
	    {
	      msg->clear() ;
	      msg->setSubject("head");
	      msg->addInt(0) ;
	      msgSender->sendMessage(msg);

	      msg->clear() ;
	      msg->setSubject("wand");
	      msg->addInt(0) ;
	      msgSender->sendMessage(msg);
	    }
	  else if (headWand == 1)
	    {
	      msg->clear() ;
	      msg->setSubject("head");
	      msg->addInt(1) ;
	      msgSender->sendMessage(msg);
	    }
	  else if (headWand == 2)
	    {
	      msg->clear() ;
	      msg->setSubject("head");
	      msg->addInt(0) ;
	      msgSender->sendMessage(msg);

	      msg->clear() ;
	      msg->setSubject("wand");
	      msg->addInt(1) ;
	      msgSender->sendMessage(msg);
	    }
	  else if (headWand == 3)
	    {
	      msg->clear() ;
	      msg->setSubject("head");
	      msg->addInt(1) ;
	      msgSender->sendMessage(msg);
	    }
	}
     else if (A == KeyChar_j || A == KeyChar_J)
	{
	  jump = ~jump ;
	  msg->clear() ;
	  msg->setSubject("jump");
	  msg->addInt(jump) ;
	  msgSender->sendMessage(msg);
	}
      else if (A == KeyChar_KP_Begin || A == KeyChar_5) //KP 5
	{
	  jump_reset() ;
	  sendJumpTo() ;
	}
    }
  
  return CONTINUE;
}



/************ DTK C++ dtkAugment loader/unloader functions ***************
 *
 * All DTK dtkAugment DSO files are required to declare these two
 * functions.  These function are called by the loading program to get
 * your C++ objects loaded.
 *
 *************************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
  return new simulatorKeyboardControl(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}


