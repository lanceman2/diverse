#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

#define XYZ (.75f)
#define HPR (90.f)
#define DELTA_HEAD_TO_WAND_X   (0.f)
#define DELTA_HEAD_TO_WAND_Y   (.2f)
#define DELTA_HEAD_TO_WAND_Z   (-.2f)

/*!
 * @class caveKeyboardInput
 *
 * @brief DSO which reads the keyboard and sets dtkIn* devices for head,
 * wand, joystick and buttons.  Uses the same key mappings as the dpf
 * caveSimInput DSO.
 *
 * -  Left/Right
 *   - move head & wand left (-X)/right (+X)
 * -  Up/Down		
 *   - move head & wand forward (+Y)/backwards (-Y)
 * -  Shift + Up/Down       
 *   - move head & wand up (+Z)/down (-Z)
 * -  Alt + Left/Right	
 *   - rotate head & wand left (+H)/right (-H)
 * -  Alt + Up/Down         
 *   - rotate head up (+P)/down (-P)
 * -  '<' or ','            
 *   - roll wand left (-R)
 * -  '>' or '.'            
 *   - roll wand right (+R)
 * -  'r' or 'R'		
 *   - reset head an wand
 * -  Keypad Home		
 *   - reset wand
 * -  Control + Mouse	
 *   - move wand left/right (X) front/back(Y)
 * -  Shift + Mouse		
 *   - move wand left/right (X) up/down(Z)
 * -  Alt + Mouse		
 *   - rotate wand left/right (H) up/down(P)
 * -  Space + Mouse		
 *   - set joystick (X and Y)
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class caveKeyboardInput : public dtkAugment
{
public:
  caveKeyboardInput(dtkManager *);
  
  int postFrame(void);
  
private:
  
  DGLKeyboard *kb ;
  DGLMouse *mouse ;
  DGLMouseButton *mouseButton ;

  dtkManager    *manager;

  // dtkTime is a gettimeofday() wrapper
  dtkTime t;

  dtkInLocator  *head;      // simulated head tracker out
  dtkInLocator  *wand;      // simulated wand tracker out
  dtkInValuator *joystick; // simulated joystick out
  // buttons already written to dtkInButtons("buttons") in DGLMouseButtons ;
  float head_xyzhpr[6], wand_xyzhpr[6], joystick_xy[2] ;

  // mouse values when modifier key pressed
  float start_x, start_y ;
  float caveSizeInDpf ;

  void clamp(float &f)
    {
      if (f<-1.f) f = -1.f ;
      else if (f>1.f) f = 1.f ;
    }
    
  void resetHead(void) 
  {
    memset(head_xyzhpr, 0, sizeof(float)*6) ;
    head->write(head_xyzhpr) ;
  }

  void resetWand(void) 
  {
    memset(wand_xyzhpr, 0, sizeof(float)*6) ;
    wand_xyzhpr[0] = DELTA_HEAD_TO_WAND_X ;
    wand_xyzhpr[1] = DELTA_HEAD_TO_WAND_Y ;
    wand_xyzhpr[2] = DELTA_HEAD_TO_WAND_Z ;
    wand->write(wand_xyzhpr) ;
  }

  void modifyHead(float x, float y, float z, float h, float p, float r)  
  {
    head->read(head_xyzhpr) ;
    head_xyzhpr[0] += x ;
    clamp(head_xyzhpr[0]) ;
    head_xyzhpr[1] += y ;
    clamp(head_xyzhpr[1]) ;
    head_xyzhpr[2] += z ;
    clamp(head_xyzhpr[2]) ;
    head_xyzhpr[3] += h ;
    head_xyzhpr[4] += p ;
    head_xyzhpr[5] += r ;
    head->write(head_xyzhpr) ;
  }

  void modifyWand(float x, float y, float z, float h, float p, float r)  
  {
    wand->read(wand_xyzhpr) ;
    wand_xyzhpr[0] += x ;
    clamp(wand_xyzhpr[0]) ;
    wand_xyzhpr[1] += y ;
    clamp(wand_xyzhpr[1]) ;
    wand_xyzhpr[2] += z ;
    clamp(wand_xyzhpr[2]) ;
    wand_xyzhpr[3] += h ;
    wand_xyzhpr[4] += p ;
    wand_xyzhpr[5] += r ;
    wand->write(wand_xyzhpr) ;
  }

  void set_wand_to_head(void)
  {
    head->read(head_xyzhpr) ;
    wand_xyzhpr[0] = - caveSizeInDpf*DELTA_HEAD_TO_WAND_Y*
      sinf(DEG2RAD(head_xyzhpr[3])) +
      head_xyzhpr[0];
    
    wand_xyzhpr[1] = caveSizeInDpf*DELTA_HEAD_TO_WAND_Y*
      cosf(DEG2RAD(head_xyzhpr[3])) +
      head_xyzhpr[1];
    
    wand_xyzhpr[2] = head_xyzhpr[2] + caveSizeInDpf*DELTA_HEAD_TO_WAND_Z;
    wand_xyzhpr[3] = head_xyzhpr[3];
    wand_xyzhpr[4] = 0.0f;
    wand_xyzhpr[5] = 0.0f;
    wand->write(wand_xyzhpr) ;
  }

};


caveKeyboardInput::caveKeyboardInput(dtkManager *m) :
  dtkAugment("caveKeyboardInput")
{
  setDescription("reads the keyboard and sets dtk shared memory segments to "
		 "control the simulator") ;
  
  kb = new DGLKeyboard ;

  mouse = new DGLMouse ;
  mouseButton = new DGLMouseButton ;

  manager = (dtkManager*) DGL::getApp();
  if (manager == NULL || manager->isInvalid())
    {
      printf("caveKeyboardInput::caveKeyboardInput: Bad manager :(\n");
      return;
    }
  
    #ifdef __APPLE__
     #ifdef check
      #undef check
     #endif // check
    #endif // __APPLE__
  if (!(head = (dtkInLocator*)manager->check("head")))
    {
      head = new dtkInLocator(manager->record(), "head");
      if (!head || head->isInvalid())
	dtkMsg.add(DTKMSG_ERROR,"caveKeyboardInput::caveKeyboardInput: failed to get dtkInLocator(\"head\").\n");
      if (manager->add(head)) return;
    }
  
  if (!(wand = (dtkInLocator*)manager->check("wand")))
    {
      wand = new dtkInLocator(manager->record(), "wand");
      if (!wand || wand->isInvalid())
	dtkMsg.add(DTKMSG_ERROR,"caveKeyboardInput::caveKeyboardInput: failed to get dtkInLocator(\"wand\").\n");
      if (manager->add(wand)) return;
    }
  
  if (!(joystick = (dtkInValuator*)manager->check("joystick", DTKINVALUATOR_TYPE)))
    {
      joystick = new dtkInValuator(manager->record(), 2, "joystick");
      if (!joystick || joystick->isInvalid())
	dtkMsg.add(DTKMSG_ERROR,"caveKeyboardInput::caveKeyboardInput: failed to get dtkInValuator(\"joystick\").\n");
      if (manager->add(joystick)) return;
    }

  
  caveSizeInDpf = 1.f;

  validate() ;
}

int caveKeyboardInput::postFrame(void)
{

  static bool first = true ;
  if (first)
    {
      t.reset(0.0,1);
      resetHead() ;
      resetWand() ;
      // force a write to zero in first frame
      joystick_xy[0] = -1.f ;
      joystick_xy[1] = -1.f ;

      first = false ;
    }

  float delta_t = (float) t.delta();

  DGLKeyboard::ACTION sl =  DGLKeyboard::getState(KeyChar_Shift_L) ; 
  DGLKeyboard::ACTION sr =  DGLKeyboard::getState(KeyChar_Shift_R) ; 
  bool shift = (sl==DGLKeyboard::PRESSED || sr==DGLKeyboard::PRESSED) ;

  DGLKeyboard::ACTION al =  DGLKeyboard::getState(KeyChar_Alt_L) ; 
  DGLKeyboard::ACTION ar =  DGLKeyboard::getState(KeyChar_Alt_R) ; 
  bool alt = (al==DGLKeyboard::PRESSED || ar==DGLKeyboard::PRESSED) ;

  DGLKeyboard::ACTION cl =  DGLKeyboard::getState(KeyChar_Control_L) ; 
  DGLKeyboard::ACTION cr =  DGLKeyboard::getState(KeyChar_Control_R) ; 
  bool control = (cl==DGLKeyboard::PRESSED || cr==DGLKeyboard::PRESSED) ;

  bool space = (DGLKeyboard::getState(KeyChar_space)==DGLKeyboard::PRESSED) ;

  // get keyboard actions
  DGLKeyboard::ACTION a ;
  KeyCharacter A = kb->readKeyCharacter(&a) ;

  // poll mouse position
  float x, y ;
  mouse->getMouse(&x, &y) ;

  if (a==DGLKeyboard::PRESSED &&
      (A==KeyChar_Shift_L   || A==KeyChar_Shift_R || 
       A==KeyChar_Alt_L     || A==KeyChar_Alt_R ||
       A==KeyChar_Control_L || A==KeyChar_Control_R))
    {
      //printf("start mouse at %f %f\n",x,y) ;
      start_x = x ;
      start_y = y ;
    }
  
  // do rate based keys, just poll to see if pressed
  if (DGLKeyboard::getState(KeyChar_Left)==DGLKeyboard::PRESSED)
    {
      if (alt) 
	{
	  modifyHead(0, 0, 0, HPR*delta_t, 0, 0) ; 
	  set_wand_to_head() ;
	}
      else
	{
	  modifyHead(-XYZ*delta_t, 0, 0, 0, 0, 0) ;
	  set_wand_to_head() ;
	}
    }
  else if (DGLKeyboard::getState(KeyChar_Up)==DGLKeyboard::PRESSED)
    {
      if (shift)
	{
	  modifyHead(0, 0, XYZ*delta_t, 0, 0, 0) ;
	  set_wand_to_head() ;
	}
      else if (alt)
	{
	  modifyHead(0, 0, 0, 0, HPR*delta_t, 0) ;
	}
      else
	{
	  modifyHead(0, XYZ*delta_t, 0, 0, 0, 0) ;
	  set_wand_to_head() ;
	}
    }
  else if (DGLKeyboard::getState(KeyChar_Right)==DGLKeyboard::PRESSED)
    {
      if (alt)
	{
	  modifyHead(0, 0, 0, -HPR*delta_t, 0, 0) ;
	  set_wand_to_head() ;
	}
      else
	{
	  modifyHead(XYZ*delta_t, 0, 0, 0, 0, 0) ;
	  set_wand_to_head() ;
	}
    }
  else if (DGLKeyboard::getState(KeyChar_Down)==DGLKeyboard::PRESSED)
    {
      if (shift)
	{
	  modifyHead(0, 0, -XYZ*delta_t, 0, 0, 0) ;
	  set_wand_to_head() ;
	}
      else if (alt)
	{
	  modifyHead(0, 0, 0, 0, -HPR*delta_t, 0) ;
	}
      else
	{
	  modifyHead(0, -XYZ*delta_t, 0, 0, 0, 0) ;
	  set_wand_to_head() ;
	}
    }
  else if (DGLKeyboard::getState(KeyChar_less)==DGLKeyboard::PRESSED || 
	   DGLKeyboard::getState(KeyChar_comma)==DGLKeyboard::PRESSED)
  {
    modifyWand(0, 0, 0, 0, 0, -HPR*delta_t) ;
  }
  else if (DGLKeyboard::getState(KeyChar_greater)==DGLKeyboard::PRESSED || 
	   DGLKeyboard::getState(KeyChar_period)==DGLKeyboard::PRESSED)
  {
      modifyWand(0, 0, 0, 0, 0, HPR*delta_t) ;
  }

  // do actions caused by a button release
  else if (A && a==DGLKeyboard::RELEASED)
    {
      if (A == KeyChar_r || A == KeyChar_R) 
	{
	  resetHead() ;
	  resetWand() ;
	}
      else if (A == KeyChar_Home)
	{
	  resetWand() ;
	}
    }
  
  else if (control)
    {
	modifyWand(x*x*x*delta_t*XYZ, y*y*y*delta_t*XYZ, 0, 0, 0, 0) ;
    }
  else if (shift)
    {
	modifyWand(x*x*x*delta_t*XYZ, 0, y*y*y*delta_t*XYZ, 0, 0, 0) ;
    }
  else if (alt)
    {
	modifyWand(0, 0, 0, -x*x*x*delta_t*HPR, y*y*y*delta_t*HPR, 0) ;
    }
  else if (space)
    {
      joystick_xy[0] = x ;
      joystick_xy[1] = y ;
      joystick->write(joystick_xy) ;
    }
  else if (!space && (joystick_xy[0] != 0.f ||joystick_xy[1] != 0.f))
    {
      joystick_xy[0] = 0.f ;
      joystick_xy[1] = 0.f ;
      joystick->write(joystick_xy) ;
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
  return new caveKeyboardInput(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}


