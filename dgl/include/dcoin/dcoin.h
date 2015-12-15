#ifndef _D_COIN_H_
#define _D_COIN_H_

//Forward declarations of class names 
class SoSceneManager;
class SoGroup;
class SoTransform;

/*!
 * @class DCoin dcoin.h 
 *
 * @brief The DIVERSE interface to Coin
 *
 * This class basically consists of a bare bones
 * scenegraph setup with a navigated node and a non navigated node.
 * You can access the navigated node by using getWorld and the non navigated
 * version by using getEther.
 *
 * CAUTION:  before you use any functions in this class make sure you call
 * Dcoin::init() because this is a static class.  What this means is you need
 * to call all the functions by doing Dcoin::functionName.
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DCOINAPI DCoin : public dglAugment
{
 public:
  //For virtual functions
  virtual ~DCoin();

  /*!
   * @brief Initalize the class
   *
   * @param Name name passed to dglAugment
   * 
   * @param Flip Pass in true if you want the world flipped 90 degrees so
   * that it is consistent with DGL
   */
  static void init(char* Name="DCOIN", const bool& Flip=true);

  /*!
   * @brief Return the navigated world node
   *
   * @return the navigated world node
   */
  static SoGroup* getWorld();

  /*!
   * @brief Return the base node
   *
   * @return the base node
   */
  static SoGroup* getScene();

  /*!
   * @brief Return the non-navigated ether node
   *
   * @return the non-navigated ether node
   */
  static SoGroup* getEther();

  /*!
   * @brief Load a model file
   *
   * @return the node containg the loaded file
   */
  static SoSeparator* loadFile(const string& filename);

  /*!
   * @brief draw callback
   *
   * Do not call this unless you know exactly what you are doing and why.
   */
  virtual int draw() ;

  /*!
   * @brief  postConfig callback
   *
   * Do not call this unless you know exactly what you are doing and why.
   *
   * @return always returns 1
  */
  virtual int postConfig();

  /*!
   * @brief  postFrame callback
   *
   * Do not call this unless you know exactly what you are doing and why.
   *
   * @return always returns 0
   */
  virtual int postFrame();

  /*!
   * @brief Flip the world -90 degrees in the x direction so that it is consistent with DGL
   *
   * @param Flip - set to true to flip the world, or false not to flip it
   */
  static void flipWorld(const bool& Flip);
  
 private:
  //Private constructor so we guarantee a static class
  DCoin(char* Name);
  //Flip our axis properly
  static SoTransform* m_flip;
  //Pointer to self for a static class
  static DCoin* m_self;
  //The non navigated node
  static SoGroup* m_ether;
  //The navigated node
  static SoGroup* m_world;
  //The very root of the scene graph
  static SoGroup* m_base;
  //Flip the world or not?
  static bool     m_flipval;
};
#endif
