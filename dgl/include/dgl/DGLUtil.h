// ****************************************************************************
// ** dgl/lib_dgl/util.H                                                     **
// ** Class declarations for a lot of little things that don't warrant their **
// ** own file.                                                              **
// ****************************************************************************

#ifndef _DGLUTIL_H_
#define _DGLUTIL_H_
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif 
#include <dtk.h>

#include <vector>
#include <string>

#define DEG2RAD(x)  ((float) ((x)*M_PI/180.0f))
#define RAD2DEG(x)  ((float) ((x)*180.0f/M_PI))

class DGLWindow ;

namespace DGLUtil 
{
  /*!
   * @brief Convert a char array to a float
   *
   * @param s the char array containg the ASCII representation of a floating point number
   * 
   * @param f receives s converted to a floating point number
   *
   * @return zero on success, non-zero if an error occurred
   *
   * @author John Kelso, kelso@nist.gov
   * @author Andrew A. Ray, anray2@vt.edu
   */
  DGLAPI int string_to_float(const char *s, float *f) ;

  /*!
   * @brief Convert a char array to a int
   *
   * @param s the char array containg the ASCII representation of an integer
   * 
   * @param i receives s converted to an integer
   *
   * @return zero on success, non-zero if an error occurred
   *
   * @author John Kelso, kelso@nist.gov
   * @author Andrew A. Ray, anray2@vt.edu
   */
  DGLAPI int string_to_int(const char *s, int *i) ;

  /*!
   * @brief rotate 0,1,0 through a heading and pitch
   *
   * 0,1,0 is used, as that's xyz if hpr=0,0,0
   *
   * @param h heading
   *
   * @param p pitch
   *
   * @return position of rotated vector
   *
   * @author John Kelso, kelso@nist.gov
   * @author Andrew A. Ray, anray2@vt.edu
   */
  DGLAPI dtkVec3 eulerToXyz(float h, float p) ;

  /*!
   * @brief Parse an X11 DISPLAY string it into its components
   * 
   * DISPLAY has the format [hostname]:displayNumber[.screenNumber]
   *
   * @param display is an X11 DISPLAY string
   *
   * @param hostname is the piece of DISPLAY before the ":", or blank if not set.
   *
   * @param displayNumber is the piece of DISPLAY between the ":" and the
   * "." or the end of the string.
   *
   * @param screenNumber is the piece of DISPLAY after the ".", or 0 if not specified
   *
   * @return zero on success, or non-zero if DISPLAY is improperly formatted
   *
   * @author John Kelso, kelso@nist.gov
   * @author Andrew A. Ray, anray2@vt.edu
   */
  DGLAPI int parseDisplay(const char *display, char **hostname, unsigned int *displayNumber, unsigned int *screenNumber) ;
 
  /*!
   * @brief Build an X11 DISPLAY string from its components
   * 
   * DISPLAY has the format [hostname]:displayNumber[.screenNumber]
   *
   * @param hostname is the piece of DISPLAY before the ":", or blank if not set.
   *
   * @param displayNumber is the piece of DISPLAY between the ":" and the
   * "." or the end of the string.
   *
   * @param screenNumber is the piece of DISPLAY after the "."
   *
   * @return the X11 DISPLAY string
   *
   * @author John Kelso, kelso@nist.gov
   * @author Andrew A. Ray, anray2@vt.edu
   */
  DGLAPI char * buildDisplay(char *hostname, unsigned int displayNumber, unsigned int screenNumber) ;
 

  /*!
   * @brief Parse a char array into a std::vector of std::string words
   *
   * Parameters are separated by any number of whitespace characters
   *
   * Everything after and including the pound sign is discarded
   *
   * Tabs are NOT converted to whitespace- only spaces count as whitespace
   *
   * Single and double quotes can be used to put spaces in parameters.  The
   * backslash character can be used to escape the meaning of a single or
   * double quote or space
   *
   * @param line char array to be parsed
   *
   * @return a std::vector of std::string words, one string per whitespace
   * separated word in line
   *
   * @author John Kelso, kelso@nist.gov
   * @author Andrew A. Ray, anray2@vt.edu
   */
  DGLAPI std::vector<std::string> parseLine(char *line) ;
}    

/*!
 * @class dglVec2 DGLUtil.h dgl.h
 *
 * @brief Templated class to support 2-element data
 *
 * @author Chris Logie
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
template <class datatype>
class DGLAPI dglVec2
{
  public:
  /*!
   * Create a dglVec2 with unspecified values
   */
  dglVec2() 
    { x=data[0]; y=data[1]; w=data[0]; h=data[1];};
  
  /*!
   * Create a dglVec2 with initial values
   *
   * @param a first element
   *
   * @param b second element
   */
  dglVec2(datatype a, datatype b) :
    x(data[0]), y(data[1]),
    w(data[0]), h(data[1])
  {
    data[0] = a;
    data[1] = b;
  }

  /*!
   * Create a dglVec2 with initial values
   *
   * @param v intial value
   */
  dglVec2(const dglVec2 &v) :
    x(data[0]), y(data[1]),
    w(data[0]), h(data[1])
  {
    data[0] = v.data[0];
    data[1] = v.data[1];
  }

  /*!
   * @brief element-by-element addition operator
   */
  dglVec2 operator+(const dglVec2 &v) const {
    return dglVec2(x + v.x, y + v.y);
  }

  /*!
   * @brief element-by-element subtraction operator
   */
  dglVec2 operator-(const dglVec2 &v) const {
    return dglVec2(x - v.x, y - v.y);
  }

  /*!
   * @brief element-by-element multiplication operator
   */
  dglVec2 operator*(const datatype &s) const {
    return dglVec2(x * s, y * s);
  }

  /*!
   * @brief element-by-element division operator
   */
  dglVec2 operator/(const datatype &s) const {
    return dglVec2(x / s, y / s);
  }

  /*!
   * @brief element-by-element assignment operator
   */
  const dglVec2 &operator=(const dglVec2 &v)
  {
    x = v.x;
    y = v.y;
    return *this;
  }

  /*!
   * @brief element-by-element addition assignment operator
   */
  const dglVec2 &operator+=(const dglVec2 &v)
  {
    x += v.x;
    y += v.y;
    return *this;
  }

  /*!
   * @brief element-by-element subtraction assignment operator
   */
  const dglVec2 &operator-=(const dglVec2 &v)
  {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  /*!
   * @brief element-by-element multiplication assignment operator
   */
  const dglVec2 &operator*=(datatype s)
  {
    x *= s;
    y *= s;
    return *this;
  }

  /*!
   * @brief element-by-element division assignment operator
   */
  const dglVec2 &operator/=(datatype s)
  {
    x /= s;
    y /= s;
    return *this;
  }

  /*!
   * the first element
   */
  datatype &x ;

  /*!
   * the second element
   */
  datatype &y ;

  /*!
   * the first element
   */
  datatype &w ;

  /*!
   * the second element
   */
  datatype &h;

  /*!
   * access elements as an array
   */
  datatype data[2];
};

/*!
 * @class dglVec3 DGLUtil.h dgl.h
 *
 * @brief Templated class to support 3-element data
 *
 * @author Chris Logie
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
template <class datatype>
class DGLAPI dglVec3
{
 public:
  /*!
   * Create a dglVec3 with unspecified values
   */
  dglVec3() :
    x(data[0]), y(data[1]), z(data[2]),
    h(data[0]), p(data[1]), r(data[2])
    {}
  
  /*!
   * Create a dglVec3 with initial values
   *
   * @param a first element
   *
   * @param b second element
   *
   * @param c third element
   */
  dglVec3(datatype a, datatype b, datatype c) :
    x(data[0]), y(data[1]), z(data[2]),
    h(data[0]), p(data[1]), r(data[2])
  {
    data[0] = a;
    data[1] = b;
    data[2] = c;
  }

  /*!
   * Create a dglVec3 with initial values
   *
   * @param v intial value
   */
  dglVec3(const dglVec3 &v) :
    x(data[0]), y(data[1]), z(data[2]),
    h(data[0]), p(data[1]), r(data[2])
  {
    data[0] = v.data[0];
    data[1] = v.data[1];
    data[2] = v.data[2];
  }

  /*!
   * @brief element-by-element addition operator
   */
  dglVec3 operator+(const dglVec3 &v) const
  {
    return dglVec3(x + v.x, y + v.y, z + v.z);
  }

  /*!
   * @brief element-by-element subtraction operator
   */
  dglVec3 operator-(const dglVec3 &v) const
  {
    return dglVec3(x - v.x, y - v.y, z - v.z);
  }

  /*!
   * @brief element-by-element multiplication operator
   */
  dglVec3 operator*(const datatype &s) const
  {
    return dglVec3(x * s, y * s, z * s);
  }

  /*!
   * @brief element-by-element division operator
   */
  dglVec3 operator/(const datatype &s) const 
  {
    return dglVec3(x / s, y / s, z / s);
  }

  /*!
   * @brief element-by-element assignment operator
   */
  const dglVec3 &operator=(const dglVec3 &v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
  }

  /*!
   * @brief element-by-element addition assignment operator
   */
  const dglVec3 &operator+=(const dglVec3 &v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  /*!
   * @brief element-by-element subtraction assignment operator
   */
  const dglVec3 &operator-=(const dglVec3 &v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  /*!
   * @brief element-by-element multiplication assignment operator
   */
  const dglVec3 &operator*=(const datatype &s)
  {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  /*!
   * @brief element-by-element division assignment operator
   */
  const dglVec3 &operator/=(const datatype &s)
  {
    x /= s;
    y /= s;
    z /= s;
    return *this;
  }

  /*!
   * @brief Return the length of the vector
   *
   * @return the length of the vector
   */
  datatype length()
    {
      return sqrt((float) x*x + y*y + z*z);
    }
  
  /*!
   * @brief Normalize the vector
   *
   * @return false if the vector is so small that this would cause an
   * inaccurate result, otherwise true
   */
  bool normalize()
    {
      float denom = sqrt((float) x*x + y*y + z*z);
      
      if (denom < 0.00001f)
	return false;
      
      x /= denom;
      y /= denom;
      z /= denom;
      
      return true;
    }
  /*!
   * @brief Set the elements of the vector
   *
   * @param ix first element
   *
   * @param iy second element
   *
   * @param iz third element
   */
  void set(const datatype& ix, const datatype& iy, const datatype& iz)
    {
      x=ix;
      y=iy;
      z=iz;
    }
  
  /*!
   * @brief Cross multiply with another vector
   *
   * @param cross the vector to cross multiply with this vector
   */
  void cross(dglVec3<datatype> cross)
    {
      datatype compx =x;
      datatype compy =y;
      datatype compz =z;
      x=compy*cross.z - compz*cross.y;
      y=compz*cross.x - compx*cross.z;
      z=compx*cross.y - compy*cross.x;
    }
  
  /*!
   * @brief Dot multiply with another vector
   *
   * @param other the vector to dot multiply with this vector
   */
  datatype dot(dglVec3<datatype> other)
  {
	return x*other.x + y*other.y + z*other.z;	
  }

public:
  /*!
   * the first element
   */
  datatype &x ;

  /*!
   * the second element
   */
  datatype &y ;

  /*!
   * the third element
   */
  datatype &z ;

  /*!
   * the first element
   */
  datatype &h ;

  /*!
   * the second element
   */
  datatype &p ;

  /*!
   * the third element
   */
  datatype &r ;

  /*!
   * access elements as an array
   */
  datatype data[3];
};

/*!
 * @class dglVec4 DGLUtil.h dgl.h
 *
 * @brief Templated class to support 4-element data
 *
 * @author Chris Logie
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
template <class datatype>
class DGLAPI dglVec4
{
 public:

  /*!
   * Create a dglVec4 with unspecified values
   */
  dglVec4() :
    x(data[0]), y(data[1]), z(data[2]), w(data[3]),
    h(data[0]), p(data[1]), r(data[2])
    {}

  /*!
   * Create a dglVec4 with initial values
   *
   * @param a first element
   *
   * @param b second element
   *
   * @param c third element
   *
   * @param d fourth element
   */
  dglVec4(datatype a, datatype b, datatype c, datatype d) :
    x(data[0]), y(data[1]), z(data[2]), w(data[3]),
    h(data[0]), p(data[1]), r(data[2])
    {
      data[0] = a;
      data[1] = b;
      data[2] = c;
      data[3] = d;
    }

  /*!
   * Create a dglVec4 with initial values
   *
   * @param v intial value
   */
  dglVec4(const dglVec4 &v) :
    x(data[0]), y(data[1]), z(data[2]), w(data[3]),
    h(data[0]), p(data[1]), r(data[2])
    {
      data[0] = v.data[0];
      data[1] = v.data[1];
      data[2] = v.data[2];
      data[3] = v.data[3];
    }
  
  /*!
   * @brief element-by-element addition operator
   */
  dglVec4 operator+(const dglVec4 &v) const
  {
    return dglVec4(x + v.x, y + v.y, z + v.z, w + v.w);
  }

  /*!
   * @brief element-by-element subtraction operator
   */
  dglVec4 operator-(const dglVec4 &v) const
  {
    return dglVec4(x - v.x, y - v.y, z - v.z, w - v.w);
  }

  /*!
   * @brief element-by-element multiplication operator
   */
  dglVec4 operator*(const datatype &s) const
  {
    return dglVec4(x * s, y * s, z * s, w * s);
  }

  /*!
   * @brief element-by-element division operator
   */
  dglVec4 operator/(const datatype &s) const
  {
    return dglVec4(x / s, y / s, z / s, w / s);
  }

  /*!
   * @brief element-by-element assignment operator
   */
  const dglVec4 &operator=(const dglVec4 &v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
  }

  /*!
   * @brief element-by-element addition assignment operator
   */
  const dglVec4 &operator+=(const dglVec4 &v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
  }

  /*!
   * @brief element-by-element subtraction assignment operator
   */
  const dglVec4 &operator-=(const dglVec4 &v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
  }

  /*!
   * @brief element-by-element multiplication assignment operator
   */
  const dglVec4 &operator*=(const datatype &s)
  {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
  }

  /*!
   * @brief element-by-element division assignment operator
   */
  const dglVec4 &operator/=(const datatype &s)
  {
    x /= s;
    y /= s;
    z /= s;
    w /= s;
  }

  /*!
   * @brief Normalize the vector
   *
   * @return false if the vector is so small that this would cause an
   * inaccurate result, otherwise true
   */
  bool normalize()
    {
      float denom = sqrt((float) x*x + y*y + z*z);
      
      if (denom < 0.00001f)
	return false;
      
      x /= denom;
      y /= denom;
      z /= denom;
      w /= denom;
      
      return true;
    }
  
 public:
  /*!
   * the first element
   */
  datatype &x ;

  /*!
   * the second element
   */
  datatype &y ;

  /*!
   * the third element
   */
  datatype &z ;

  /*!
   * the fourth  element
   */
  datatype &w ;

  /*!
   * the first element
   */
  datatype &h ;

  /*!
   * the second element
   */
  datatype &p ;

  /*!
   * the third element
   */
  datatype &r ;

  /*!
   * access elements as an array
   */
  datatype data[4];
};

/*!
 * @class dglMatrix DGLUtil.h dgl.h
 *
 * @brief 4x4 matrix class
 *
 * @author Chris Logie
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class DGLAPI dglMatrix
{
public:

  /*!
   * create an identity matrix
   */
  dglMatrix();
  
  /*!
   * create an intialized matrix
   *
   * Rows or Columns?  It depends on how you look at it.  But, know this-
   * elements 12, 13 and 14 in the array store the x, y, z translation
   * values.  Access them with &matrix(0,3), &matrix(1,3) and &matrix(2,3)
   *
   * @param data_in an array of 16 floats.
   */
  dglMatrix(float *data_in);

  /*!
   * create an intialized matrix
   *
   * @param m initialization matrix
   */
  dglMatrix(const dglMatrix &m);

  /*!
   * @brief Set this matrix to the identity
   * 
   * @return zero on success, non-zero if an error occurred
   */
  int makeIdent();

  /*!
   * @brief Set this matrix to represent a translation
   *
   * @param x x component of the translation
   *
   * @param y y component of the translation
   *
   * @param z z component of the translation
   * 
   * @return zero on success, non-zero if an error occurred
   */
  int makeTrans(float x, float y, float z);

  /*!
   * @brief Set this matrix to represent an Euler rotation
   *
   * @param h heading component of the Euler rotation
   *
   * @param p pitch component of the Euler rotation
   *
   * @param r roll component of the Euler rotation
   * 
   * @return zero on success, non-zero if an error occurred
   */
  int makeEuler(float h, float p, float r);

  /*!
   * @brief Set this matrix to represent the inverse of an Euler rotation
   *
   * @param h heading component of the Euler rotation
   *
   * @param p pitch component of the Euler rotation
   *
   * @param r roll component of the Euler rotation
   * 
   * @return zero on success, non-zero if an error occurred
   */
  int makeEulerInv(float h, float p, float r);

  /*!
   * @brief Invert the matrix
   * 
   * @return zero on success, non-zero if an error occurred
   */
  int invert();

  /*!
   * @brief  element-by-element assignment operator
   */
  const dglMatrix &operator=(const dglMatrix &m);

  /*!
   * @brief matrix multiplication assignment operator
   */
  const dglMatrix &operator*=(const dglMatrix &m);

  /*!
   * @brief matrix multiplication operator
   */
  dglMatrix operator*(const dglMatrix &m) const;

  /*!
   * @brief multiply operator
   */
  dglVec4<float> operator*(const dglVec4<float> &v) const;

  /*!
   * @brief multiply operator
   */
  dglVec3<float> operator*(const dglVec3<float> &v) const;

  /*!
   * @brief return an element of the matrix
   *
   * @param r which row
   * 
   * @param c which column
   */
  float &operator()(unsigned int r, unsigned int c);


public:
  float data[16];
};
#endif
