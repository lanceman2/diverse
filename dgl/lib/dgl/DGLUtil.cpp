#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dtk.h>

#include <vector>
#include <string>

#include <GL/gl.h>
#include <dgl.h>

namespace DGLUtil {
  int string_to_float(const char *s, float *f) 
  {
    
    if (!s || !f)
      return 1 ;
    
    char *endptr ;
    *f = (float) strtod(s, &endptr) ;
    if (s == endptr) {
      //printf("error- strtod conversion error\n") ;
      return 1 ;
    } else if (endptr[0] != 0) {
      for (char *p=endptr; *p ; p++) {
	if (!isspace(*p)) {
	  //printf("error- trailing trash\n") ;
	  return 1 ;
	}
      }
    }
    //printf("*f=%f\n",*f) ;
    return 0 ;
  }
  
  int string_to_int(const char *s, int *i) 
  {
    
    if (!s || !i)
      return 1 ;
    
    char *endptr ;
    *i = (int) strtol(s, &endptr, 10) ;
    if (s == endptr) {
      //printf("error- strtod conversion error\n") ;
      return 1 ;
    } else if (endptr[0] != 0) {
      for (char *p=endptr; *p ; p++) {
	if (!isspace(*p)) {
	  //printf("error- trailing trash\n") ;
	  return 1 ;
	}
      }
    }
    //printf("*i=%d\n",*i) ;
    return 0 ;
  }
  
  // move 0,1,0 through an angle of h & p (r isn't needed)
  // and return the xyz position
  // 0,1,0 is used, as that's xyz if hpr=0,0,0
  dtkVec3 eulerToXyz(float h, float p) 
  {
    
    float ch = cosf(DEG2RAD(h)) ;
    float sh = sinf(DEG2RAD(h)) ;
    float sp = sinf(DEG2RAD(p)) ;
    float cp = cosf(DEG2RAD(p)) ;
    
    dtkVec3 xyz ;
    xyz.x =  (-sh*cp) ;
    xyz.y = (ch*cp) ;
    xyz.z = (sp) ;
    return xyz ;
  }
  
  int parseDisplay(const char *display, char **hostname, unsigned int *displayNumber, unsigned int *screenNumber)
  {
    
    if (!display || display[0] == '\0' || !hostname || !displayNumber || !screenNumber)
      return 1 ;
    
    std::string displaystring ;
    const char *s ;
    int i ;
    *displayNumber = 0 ;
    *screenNumber = 0 ;
    *hostname = '\0' ;
    int rf ;
    
    displaystring = display;
    rf = displaystring.rfind(".") ;
    if (rf>0)
      {
	s = displaystring.substr(rf+1).c_str() ;
	if (DGLUtil::string_to_int(s, &i))
	    return 1 ;
	if (i<0)
	  return 1 ;
	*screenNumber = i ;
	displaystring = displaystring.substr(0,rf) ;
      }
    
    rf = displaystring.rfind(":") ;
    if (rf>=0)
      {
	s = displaystring.substr(rf+1).c_str() ;
	if (DGLUtil::string_to_int(s, &i))
	    return 1 ;
	if (i<0)
	  return 1 ;
	*displayNumber = i ;
	if (rf>0)
	  {
	    *hostname = strdup((displaystring.substr(0,rf)).c_str()) ;
	  }
      }
    else
      {
	return 1 ;
      }
    
    return 0 ;
  }
  
  char *buildDisplay(char *hostname, unsigned int displayNumber, unsigned int screenNumber)
  {
    std::stringstream foo ;
    std::string line ;
    foo << hostname << ":" << displayNumber << "." << screenNumber ;
    foo >> line ;
    return strdup(line.c_str()) ;
  }
  
  static void parseLine_addChar(char c, std::vector<std::string> &args, bool &inSpace)
  {
    // was in a whitespace, now have a char, so create a new paramater
    if (inSpace)
      {
	string s ;
	s+=c ;
	args.push_back(s) ;
	inSpace = false ;
      }
    else
      args.back()+=c ;
  }

  // parse a char *line into parameters- you can use single and double quotes to put spaces in parameters
  std::vector<std::string> parseLine(char *line)
  {
    bool inSpace = true;
    bool inDoubleQuote = false ;
    bool inSingleQuote = false ;
    bool prevEscape = false ;

    std::vector<std::string> args ;
    args.resize(0) ;

    for (unsigned int i=0; i<strlen(line); i++) {
      char c = line[i] ;
      if (c == '\n')
	continue;
      if  (c == '\\') {
	prevEscape = true ;
      } else {
	if (prevEscape) {
	  // if previous character was an escape, just add to arg
	  parseLine_addChar(c, args, inSpace) ;
	} else if (c == '"') {
	  if (inSingleQuote) {
	    parseLine_addChar(c, args, inSpace) ;
	  } else 
	    if (inDoubleQuote) {
	      inDoubleQuote = false ;
	    } else {
	      inDoubleQuote = true ;
	    }
	} else if  (c == '\'') {
	  if (inDoubleQuote)
	    parseLine_addChar(c, args, inSpace) ;
	  else 
	    if (inSingleQuote) {
	      inSingleQuote = false ;
	    } else {
	      inSingleQuote = true ;
	    }
	  
	} else if  (c == ' ') {
	  if (prevEscape || inSingleQuote || inDoubleQuote) {
	    parseLine_addChar(c, args, inSpace) ;
	  } else if (!inSpace) {
	    inSpace = true ;
	  }
	  
	} else if (c == '#') {
	  return args ;
	  
	} else {
	  parseLine_addChar(c, args, inSpace) ;
	}
	prevEscape = false ;
      }
    }
    return args ;
  }



}

///////////////////////////////////////////////////////////////////////////////
// dglMatrix Methods                                                         //
///////////////////////////////////////////////////////////////////////////////

dglMatrix::dglMatrix(void)
{
  data[0] = 1.0f;
  data[1] = 0.0f;
  data[2] = 0.0f;
  data[3] = 0.0f;
  data[4] = 0.0f;
  data[5] = 1.0f;
  data[6] = 0.0f;
  data[7] = 0.0f;
  data[8] = 0.0f;
  data[9] = 0.0f;
  data[10] = 1.0f;
  data[11] = 0.0f;
  data[12] = 0.0f;
  data[13] = 0.0f;
  data[14] = 0.0f;
  data[15] = 1.0f;
}

dglMatrix::dglMatrix(const dglMatrix &m)
{
  memcpy((void *) data, (void *) m.data, 16*sizeof(float));
}

dglMatrix::dglMatrix(float *m)
{
  memcpy((void *) data, (void *) m, 16 * sizeof(float));
}

int dglMatrix::makeIdent(void)
{
  data[0] = 1.0f;
  data[1] = 0.0f;
  data[2] = 0.0f;
  data[3] = 0.0f;
  data[4] = 0.0f;
  data[5] = 1.0f;
  data[6] = 0.0f;
  data[7] = 0.0f;
  data[8] = 0.0f;
  data[9] = 0.0f;
  data[10] = 1.0f;
  data[11] = 0.0f;
  data[12] = 0.0f;
  data[13] = 0.0f;
  data[14] = 0.0f;
  data[15] = 1.0f;

  return 0;
}

int dglMatrix::makeEuler(float h, float p, float r)
{
  float A = sin(DEG2RAD(h));
  float B = cos(DEG2RAD(h));
  float C = sin(DEG2RAD(p));
  float D = cos(DEG2RAD(p));
  float E = sin(DEG2RAD(r));
  float F = cos(DEG2RAD(r));

  data[0]  = (B*F)-(A*C*E);
  data[1]  = (A*F)+(B*C*E);
  data[2]  = -(D*E);
  data[3]  = 0.0f;
  data[4]  = -(A*D);
  data[5]  = (B*D);
  data[6]  = C;
  data[7]  = 0.0f;
  data[8]  = (B*E)+(A*C*F);
  data[9]  = (A*E)-(B*C*F);
  data[10] = (D*F);
  data[11] = 0.0f;
  data[12] = 0.0f;
  data[13] = 0.0f;
  data[14] = 0.0f;
  data[15] = 1.0f;

  return 0;
}

int dglMatrix::makeEulerInv(float h, float p, float r)
{
  float A = sin(-DEG2RAD(h));
  float B = cos(-DEG2RAD(h));
  float C = sin(-DEG2RAD(p));
  float D = cos(-DEG2RAD(p));
  float E = sin(-DEG2RAD(r));
  float F = cos(-DEG2RAD(r));

  data[0] = (B*F)+(A*C*E);
  data[1] = (A*D);
  data[2] = -(B*E)+(A*C*F);
  data[3] = 0.0f;
  data[4] = -(A*F)+(B*C*E);
  data[5] = (B*D);
  data[6] = (A*E)+(B*C*F);
  data[7] = 0.0f;
  data[8] = (D*E);
  data[9] = -C;
  data[10] = (D*F);
  data[11] = 0.0f;
  data[12] = 0.0f;
  data[13] = 0.0f;
  data[14] = 0.0f;
  data[15] = 1.0f;

  return 0;
}

int dglMatrix::makeTrans(float x, float y, float z)
{
  data[0] = 1.0f;
  data[1] = 0.0f;
  data[2] = 0.0f;
  data[3] = 0.0f;
  data[4] = 0.0f;
  data[5] = 1.0f;
  data[6] = 0.0f;
  data[7] = 0.0f;
  data[8] = 0.0f;
  data[9] = 0.0f;
  data[10] = 1.0f;
  data[11] = 0.0f;
  data[12] = x;
  data[13] = y;
  data[14] = z;
  data[15] = 1.0f;

  return 0;
}

int dglMatrix::invert(void)
{
  int i, j, k;
  dglMatrix m(*this), inv;
  bool isInvertable=true;
  float tmp;

  inv.makeIdent();

  for (i=0; i<4; i++) {
    if (m(i, i) < 0.000001) {
      for (j=0; j<4; j++) {
	if (m(j, i) > 0.000001) {
	  for (k=0; k < 4; k++) {
	    tmp = m(j, k);
	    m(j, k) = m(i, k);
	    m(i, k) = tmp;

	    tmp = inv(j, k);
	    inv(j, k) = inv(i, k);
	    inv(i, k) = tmp;
	  }
	}
      }
    }
  }
      
  for (i=0; i<4; i++) {
    for (j=i+1; j<4; j++) {
      if (fabs(m(j, i)) > 0.000001) {
	float c = m(i, i) / m(j, i);
	for (k=0; k<4; k++) {
	  m(j, k)   = (  m(j, k) * c) -   m(i, k);
	  inv(j, k) = (inv(j, k) * c) - inv(i, k);
	}
      }
    }
  }

  for (i=3; i>=0; i--) {
    for (j=i-1; j>=0; j--) {
      if (fabs(m(j, i)) > 0.000001) {
	float c = m(i, i) / m(j, i);
	for (k=0; k<4; k++) {
	  m(j, k)   = (  m(j, k) * c) -   m(i, k);
	  inv(j, k) = (inv(j, k) * c) - inv(i, k);
	}
      }
    }
  }

  for (i=0; i<4; i++) {
    if (fabs(m(i, i)) < 0.000001)
      isInvertable = false;
    else {
      float c = 1 / m(i, i);
      for (j=0; j<4; j++) {
	m(i, j)   =   m(i, j) * c;
	inv(i, j) = inv(i, j) * c;
      }
    }
  }

  for (i=0; i<16; i++)
    if (fabs(inv.data[i]) < 0.000001)
      inv.data[i] = 0.0f;

  if (isInvertable) {
    memcpy(data, inv.data, 16*sizeof(float));
  } else {
    printf("ERROR0: Matrix could not be inverted...\n");
    return 1;
  }

  
  
  return 0;
}

const dglMatrix &dglMatrix::operator=(const dglMatrix &m)
{
  memcpy ((void *) data, (void *) m.data, 16*sizeof(float));
  return (*this);
}

const dglMatrix &dglMatrix::operator*=(const dglMatrix &mul)
{
  const float *d1 = data;
  const float *d2 = mul.data;
  float a[16];

  a[0] = (d1[0]*d2[0]) + (d1[4]*d2[1]) + (d1[8] *d2[2]) + (d1[12]*d2[3]);
  a[1] = (d1[1]*d2[0]) + (d1[5]*d2[1]) + (d1[9] *d2[2]) + (d1[13]*d2[3]);
  a[2] = (d1[2]*d2[0]) + (d1[6]*d2[1]) + (d1[10]*d2[2]) + (d1[14]*d2[3]);
  a[3] = (d1[3]*d2[0]) + (d1[7]*d2[1]) + (d1[11]*d2[2]) + (d1[12]*d2[3]);

  a[4] = (d1[0]*d2[4]) + (d1[4]*d2[5]) + (d1[8] *d2[6]) + (d1[12]*d2[7]);
  a[5] = (d1[1]*d2[4]) + (d1[5]*d2[5]) + (d1[9] *d2[6]) + (d1[13]*d2[7]);
  a[6] = (d1[2]*d2[4]) + (d1[6]*d2[5]) + (d1[10]*d2[6]) + (d1[14]*d2[7]);
  a[7] = (d1[3]*d2[4]) + (d1[7]*d2[5]) + (d1[11]*d2[6]) + (d1[15]*d2[7]);

  a[8]  = (d1[0]*d2[8]) + (d1[4]*d2[9]) + (d1[8] *d2[10]) + (d1[12]*d2[11]);
  a[9]  = (d1[1]*d2[8]) + (d1[5]*d2[9]) + (d1[9] *d2[10]) + (d1[13]*d2[11]);
  a[10] = (d1[2]*d2[8]) + (d1[6]*d2[9]) + (d1[10]*d2[10]) + (d1[14]*d2[11]);
  a[11] = (d1[3]*d2[8]) + (d1[7]*d2[9]) + (d1[11]*d2[10]) + (d1[15]*d2[11]);

  a[12] = (d1[0]*d2[12]) + (d1[4]*d2[13]) + (d1[8] *d2[14]) + (d1[12]*d2[15]);
  a[13] = (d1[1]*d2[12]) + (d1[5]*d2[13]) + (d1[9] *d2[14]) + (d1[13]*d2[15]);
  a[14] = (d1[2]*d2[12]) + (d1[6]*d2[13]) + (d1[10]*d2[14]) + (d1[14]*d2[15]);
  a[15] = (d1[3]*d2[12]) + (d1[7]*d2[13]) + (d1[11]*d2[14]) + (d1[15]*d2[15]);

  memcpy ((void *) data, (void *) a, 16*sizeof(float));
  return (*this);
}

dglVec4<float> dglMatrix::operator*(const dglVec4<float> &v) const
{
  float a = (data[0]*v.x) + (data[4]*v.y) + (data[8] *v.z) + (data[12]*v.w);
  float b = (data[1]*v.x) + (data[5]*v.y) + (data[9] *v.z) + (data[13]*v.w);
  float c = (data[2]*v.x) + (data[6]*v.y) + (data[10]*v.z) + (data[14]*v.w);
  float d = (data[3]*v.x) + (data[7]*v.y) + (data[11]*v.z) + (data[15]*v.w);
  
  return dglVec4<float>(a, b, c, d);
}

dglVec3<float> dglMatrix::operator*(const dglVec3<float> &v) const
{
  float a = (data[0]*v.x) + (data[4]*v.y) + (data[8] *v.z) + data[12];
  float b = (data[1]*v.x) + (data[5]*v.y) + (data[9] *v.z) + data[13];
  float c = (data[2]*v.x) + (data[6]*v.y) + (data[10]*v.z) + data[14];
  float d = (data[3]*v.x) + (data[7]*v.y) + (data[11]*v.z) + data[15];

  a /= d;
  b /= d;
  c /= d;

  return dglVec3<float>(a, b, c);
}

dglMatrix dglMatrix::operator*(const dglMatrix &mul) const
{
  float a[16];
  const float *d1 = data;
  const float *d2 = mul.data;

  a[0] = (d1[0]*d2[0]) + (d1[4]*d2[1]) + (d1[8] *d2[2]) + (d1[12]*d2[3]);
  a[1] = (d1[1]*d2[0]) + (d1[5]*d2[1]) + (d1[9] *d2[2]) + (d1[13]*d2[3]);
  a[2] = (d1[2]*d2[0]) + (d1[6]*d2[1]) + (d1[10]*d2[2]) + (d1[14]*d2[3]);
  a[3] = (d1[3]*d2[0]) + (d1[7]*d2[1]) + (d1[11]*d2[2]) + (d1[12]*d2[3]);

  a[4] = (d1[0]*d2[4]) + (d1[4]*d2[5]) + (d1[8] *d2[6]) + (d1[12]*d2[7]);
  a[5] = (d1[1]*d2[4]) + (d1[5]*d2[5]) + (d1[9] *d2[6]) + (d1[13]*d2[7]);
  a[6] = (d1[2]*d2[4]) + (d1[6]*d2[5]) + (d1[10]*d2[6]) + (d1[14]*d2[7]);
  a[7] = (d1[3]*d2[4]) + (d1[7]*d2[5]) + (d1[11]*d2[6]) + (d1[15]*d2[7]);

  a[8]  = (d1[0]*d2[8]) + (d1[4]*d2[9]) + (d1[8] *d2[10]) + (d1[12]*d2[11]);
  a[9]  = (d1[1]*d2[8]) + (d1[5]*d2[9]) + (d1[9] *d2[10]) + (d1[13]*d2[11]);
  a[10] = (d1[2]*d2[8]) + (d1[6]*d2[9]) + (d1[10]*d2[10]) + (d1[14]*d2[11]);
  a[11] = (d1[3]*d2[8]) + (d1[7]*d2[9]) + (d1[11]*d2[10]) + (d1[15]*d2[11]);

  a[12] = (d1[0]*d2[12]) + (d1[4]*d2[13]) + (d1[8] *d2[14]) + (d1[12]*d2[15]);
  a[13] = (d1[1]*d2[12]) + (d1[5]*d2[13]) + (d1[9] *d2[14]) + (d1[13]*d2[15]);
  a[14] = (d1[2]*d2[12]) + (d1[6]*d2[13]) + (d1[10]*d2[14]) + (d1[14]*d2[15]);
  a[15] = (d1[3]*d2[12]) + (d1[7]*d2[13]) + (d1[11]*d2[14]) + (d1[15]*d2[15]);

  return dglMatrix((float *) a);
}

float &dglMatrix::operator()(unsigned int r, unsigned int c)
{
  return data[4*c + r];
}

