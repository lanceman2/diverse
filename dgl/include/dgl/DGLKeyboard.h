#ifndef __DGL_KEYBOARD_H__
#define __DGL_KEYBOARD_H__
#include <map>

/*!
 * @class DGLKeyboard DGLKeyboard.h dgl.h
 *
 * @brief Reads and writes keyboard data.  
 *
 * Unlike the dtkInXKeyboard clas it does not require X11.  It contains
 * callbacks which are called by Producer when a Keyboard event occurs, and
 * methods DGL programs can use to access this data.
 *
 * Keyboard data are also written to the dtk shared memory file
 * "dgl/keyboard" as two Producer::KeyCharacter items.  The first is the new
 * state of the key that caused the event, and the second is the
 * KeyCharacter which caused the event.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class DGLAPI DGLKeyboard
{
public:
  /*!
   * Specifies the action that caused the event
   */
  enum ACTION {PRESSED=1,  /*!< a key press */
	       RELEASED=0, /*!< a key release */
	       UNKNOWN=-1  /*!< unknown key state */
  } ;

  /*!
   * a std::map between a KeyCharacter (the key) and its state (the value)
   */
  typedef std::map<KeyCharacter, int> KEYMAP ;

  // usual constructor
  DGLKeyboard(void) ;

  /*!
   * @brief is the specified key a "special" key
   *
   * A "special" key is something that can't be represented as an ASCII
   * character, such as a function key or arrow.
   *
   * @param A the key to check for "specialness"
   *
   * @return true if the key is "special", otherwise false
   */
  static bool isSpecial(KeyCharacter A) ;

  /*!
   * @brief Return the ASCII equivalent of a Producer::KeyCharacter, or zero
   * if the Producer::KeyCharacter is "special"
   *
   * @param A the Producer::KeyCharacter to convert to ASCII
   *
   * @return the ASCII equivalent of a Producer::KeyCharacter, or zero
   * if the Producer::KeyCharacter is "special"
   */
  static char getChar(KeyCharacter A) ;

  /*!
   * @brief Return the Producer::KeyCharacter equivalent of an ASCII character
   *
   * @param c the ASCII character to cobvert to a Producer::KeyCharacter
   *
   * @return the Producer::KeyCharacter equivalent of an ASCII character
   */
  static KeyCharacter getKeyCharacter(char c)
    { return (KeyCharacter)c ; }

  /*!
   * @brief Return the state of the specified Producer::KeyCharacter
   *
   * @param A the Producer::KeyCharacter to query
   *
   * @return the state of the specified Producer::KeyCharacter
   */
  static ACTION getState(KeyCharacter A) ;

  /*!
   * @brief Performs a read of the keyboard queue
   *
   * @param a is set to the state of the key read from the queue.  If
   * nothing was on the queue, a is set to DGLKeyboard::UNKNOWN.
   *
   * @return the Producer::KeyCharacter read from the queue.  If
   * nothing was on the queue, zero is returned
   */
  KeyCharacter readKeyCharacter(ACTION *a) ;

  /*!
   * @brief Write keyboard data
   *
   * @param A the Producer::KeyCharacter to write
   *
   * @param a the Producer::KeyCharacter state
   *
   * @return zero on success, non-zero if an error occurred
   */
  int writeKeyCharacter(KeyCharacter A, ACTION a) ;

  /*! 
   * @brief Performs a read of the keyboard queue
   *
   * @param a is set to the state of the key read from the queue.  If
   * nothing was on the queue, a is set to DGLKeyboard::UNKNOWN.
   *
   * @return the ASCII character read from the queue. If nothing was on the
   * queue, or the key read was "special", zero is returned
   */
  char readChar(ACTION *a)
    { return getChar(readKeyCharacter(a)) ; }

  /*!
   * @brief Write keyboard data
   *
   * @param c the ASCII character to write
   *
   * @param a the Producer::KeyCharacter state
   *
   * @return zero on success, non-zero if an error occurred
   */
  int writeChar(char c, ACTION a)
    { return writeKeyCharacter(getKeyCharacter(c), a) ; }

  /*!
   * @brief Specify which window received the mouse input
   *
   * @param w pointer to a DGLWindow
   *
   * This method is called by the DProducerInputCallback object.
   */
  void setWindow(DGLWindow* w) { m_window = w ; } ;

  /*!
   * @brief Returns which window received the mouse input
   *
   * @return pointer to a DGLWindow
   */
  DGLWindow *getWindow() { return m_window ; } ;


 private:
  dtkSharedMem*      m_keyboard;
  // buff[0] = KeyCharacter to read/write
  // buff[1] = static_cast<KeyChar>(ACTION) ;
  KeyCharacter       buff[2*sizeof(KeyCharacter)];
  static KEYMAP      state ;
  // the cursor can only be in one window at a time
  static DGLWindow*  m_window;
} ;

#endif
