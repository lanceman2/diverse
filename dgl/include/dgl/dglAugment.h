#ifndef _dgl_augment_h_
#define _dgl_augment_h_


/*! 
 * @class  dglAugment dglAugment.h dgl.h
 * @brief DGL Augment class, based on DTK's dtkAugment class
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */

class DGLAPI dglAugment : public dtkAugment 
{
 public:
  
  /*!
   * Specifies which callbacks the augment uses
   */
  enum DGL_AUGMENT_TYPE{BASE,     /*!< a normal callback */
			OVERLAY,  /*!<an overlay callback, a draw callback
				    that's called last, so its graphics are
				    not occluded by normal callbacks */
			PRENAV    /*!<a prenav callback, a draw callback
				    that is called before the navigation is
				    applied */
  };
  
  /*!
   * @param Name the name of the augment- must be unique!
   *
   * @param type the type of augment- i.e., what callbacks will be invoked.
   */
  dglAugment(char* Name, DGL_AUGMENT_TYPE type=BASE);
  
  ~dglAugment(void);
  
  /*!
   * @brief preConfig callback, invoked once, after DGL::init and before DGL::config
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int preConfig(void);
  
  /*!
   * @brief postConfig callback, invoked once, after DGL::config and before DGL::frame
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int postConfig(void);
  
  /*!
   * @brief preFrame callback, invoked every frame, before DGL::frame
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int preFrame(void);
  
  /*!
   * @brief postFrame callback, invoked every frame, after DGL::frame
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int postFrame(void);
  
  /*!
   * @brief Draw the graphics
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int draw(void);
  
  /*!
   * @brief Specifies that the augment will have its callbacks called
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int activate(void);
  
  /*!
   * @brief Specifies that the augment will not have its callbacks called
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int deactivate(void);
  
  /*!
   * @brief return true if augment is active, false if augment is inactive
   *
   * @return true if augment is active, false if augment is inactive
   */
  virtual bool isActive(void);
  
  /*!
   * @brief Callback to set up OpenGL
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int setupGL(void);
  
  /*!
   * @brief Callback to init OpenGL
   *
   * @return 0 on success, non-zero if an error occurs
   */
  virtual int initGL(void);
  
 private:
  bool m_active;
};
#endif
