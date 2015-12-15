#ifndef _DGLSIMULATOR_H_
#define _DGLSIMULATOR_H_

class DGLPostOffice ;
class DGLMessage ;
class dtkManager ;
 
/*!
 * @class DGLSimulator DGLSimulator.h dgl.h
 *
 * @brief Base class for immersive simulators
 *
 * This class contains implemented and stubbed virtual methods to aid
 * in creating an immersive simulator.
 *
 * The implemented methods read messages from the DGLPostOffice
 * "dgl/DGLSimulator", parse them and set booleans which a subclass can
 * check to determine what action to perform.
 *
 * Mesages have the subject of "head", "wand", "jump" and "jumpto"
 *
 * An example base class implementation is the dosgSimulator DSO.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class DGLAPI DGLSimulator : public dtkAugment
{
 public:

  /*!
   * @param m pointer to the dtkManager object
   */
  DGLSimulator(dtkManager *m) ;

  /*!
   * @brief Read messages from the DGLPostOffice and call the
   * appropriate methods based on the message's subject
   */
  virtual int postFrame(void) ;

  // these are implemented in this class, parse the messgages, and invoke the
  // methods in the next section
  /*!
   * @brief calls DGLSimulator::head
   *
   * Calls DGLSimulator::head(true) if the message's body is non-zero,
   * otherwise calls DGLSimulator::head(false)
   */
  virtual void headMsg(void) ;

  /*!
   * @brief calls DGLSimulator::wand
   *
   * Calls DGLSimulator::wand(true) if the message's body is non-zero,
   * otherwise calls DGLSimulator::wand(false)
   */
  virtual void wandMsg(void) ;

  /*!
   * @brief calls DGLSimulator::jump
   *
   * Calls DGLSimulator::jump(true) if the message's body is non-zero,
   * otherwise calls DGLSimulator::jump(false)
   */
  virtual void jumpMsg(void) ;

  /*!
   * @brief calls DGLSimulator::jumpTo
   *
   * Calls DGLSimulator::jumpTo(float x, float y, float z, float h, float p,
   * float r), passing it a position and orientation based on six floats
   * passed in its message
   */
  virtual void jumpToMsg(void) ;

  // these should be implemented by the sub-class if you want something
  // usful to happen
  /*!
   * @brief Stubbed method- all it does is print its parameter
   */
  virtual void head(bool) ;

  /*!
   * @brief Stubbed method- all it does is print its parameter
   */
  virtual void wand(bool) ;

  /*!
   * @brief Stubbed method- all it does is print its parameters
   */
  virtual void jump(bool) ;
  
  /*!
   * @brief Stubbed method- all it does is print its parameter
   */
  virtual void jumpTo(dtkCoord) ;
  
 protected:
  DGLMessage *msg ;
 private:
  DGLPostOffice *msgSender ;
  
} ;

#endif
