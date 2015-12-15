// fltk program to control DTK navigation through shared memory, using the
// shmNavControl DSO

#define SHM_NAV_NAME "dgl/shmNavControl"
#define SHM_NAV_SIZE (500)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <string>

#include <dtk.h>
#include <dgl.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Roller.H>
#include <FL/Fl_Value_Output.H>

using namespace std;

////////////////////////////////////////////////////////////////////////
// global variables
char *prog ;
dtkSharedMem *shmNav = NULL ;
Fl_Value_Output* textDisplay ;
Fl_Light_Button* scaleButton ;
Fl_Light_Button* responseButton ;

// current, initial, min and max scale and response values
float scale ;
float response ;

float initResponse = 1 ;
float initScale = 1 ;

float minScale = .001f ;
float maxScale = 100.f ;

float minResponse = .001f ;
float maxResponse = 100.f ;

// value of the roller for response and scale- always goes between 0 and 1
Fl_Roller* roller ;
float rollerResponse ;
float rollerScale ;

// scale factor for window size
float wscale = 1.f ;
  
// for raising roller input to power for scale/response values
float power = 4.f ;


////////////////////////////////////////////////////////////////////////
static void usage(char *name) {
  fprintf(stderr,"Usage: %s [FLTK options] \\\n"
	  "  [--navs [:]nav[:nav...][:]] \\\n"
	  "  [--scale min initial max] \\\n"
	  "  [--respone min initial max] \\\n"
	  "  [--wscale wscale] \\\n"
	  "  [--pow power]\n\n"
	  "  [:]nav[:nav...][:] is a list of one or more colon-separated navigations\n"
	  "  scale sets the min, initial and max navigational scale values\n"
	  "    The default scale values are %f %f %f\n"
	  "  response sets the min, initial and max navigational response values\n"
	  "    The default response values are %f %f %f\n"
	  "  pow sets the value of the power function used to map roller values to scale\n"
	  "    and response values- the formula used is pow(roller,power)*(max-min)+min\n"
	  "    the default power is %f\n"
	  "  wscale sets the scale of the application's window\n"
	  "    The default wscale is %f\n\n"
	  "  All options can be abbreviated by their first three characters,\n"
	  "  but if more letters are given, they must match exactly\n",
	  name, minScale, initScale, maxScale, minResponse, 
	  initResponse, maxResponse, power, wscale) ;
}

////////////////////////////////////////////////////////////////////////
static void resetCallback(Fl_Widget *w) {
  //~printf("resetCallback called\n") ;

  rollerScale = pow((initScale-minScale)/(maxScale-minScale), 1.f/power) ;
  rollerResponse = pow((initResponse-minResponse)/(maxResponse-minResponse), 1.f/power) ;
  scale = initScale ;
  response = initResponse ;

  if (responseButton->value()) {
    roller->value(rollerResponse) ;
    textDisplay->value(initResponse) ;
  } else {
    roller->value(rollerScale) ;
    textDisplay->value(initScale) ;
  }

  //~printf("rollerScale = %f, rollerResponse = %f\n",rollerScale,rollerResponse) ;

  //~printf("initResponse = %f\n",initResponse) ;
  //~float foo = pow(rollerResponse,power)*(maxResponse-minResponse) + minResponse ;
  //~printf("pow(rollerResponse,power)*(maxResponse-minResponse) + minResponse = %f\n",foo) ;


  // if w is NULL, don't update shared memory- not called by a button press, but just to init data
  if (w!=NULL)
    {
      char f[127] ;
      
      sprintf(f, "set scale %f", initScale) ;
      if (shmNav->write(f)) {
	fprintf(stderr,"%s: error writing to shared memory\n",prog) ;
	exit(1) ;
      }
      
      sprintf(f, "set response %f", initResponse) ;
      if (shmNav->write(f)) {
	fprintf(stderr,"%s: error writing to shared memory\n",prog) ;
	exit(1) ;
      }
      
      if (shmNav->write("set location 0 0 0 0 0 0")) {
	fprintf(stderr,"%s: error writing to shared memory\n",prog) ;
	exit(1) ;
      }
    }
}

////////////////////////////////////////////////////////////////////////
static void navCallback(Fl_Widget *w) {
  //~printf("navCallback called, value = %d, label = %s\n",
  //~((Fl_Light_Button *)w)->value(), ((Fl_Light_Button *)w)->label()) ;
  string n = "navigation \"" ;
  n += ((Fl_Light_Button *)w)->label() ;
  n += "\"" ;
  if (shmNav->write(n.c_str())) {
    fprintf(stderr,"%s: error writing to shared memory\n",prog) ;
    exit(1) ;
  }
}

////////////////////////////////////////////////////////////////////////
static void rollerScaleCallback(Fl_Widget *w) {
  rollerScale = (float)roller->value() ;
  //~printf("rollerScaleCallback called, value = %f   ",rollerScale) ;
  scale = pow(rollerScale,power)*(maxScale-minScale) + minScale ;

  //~printf("setting scale to %f\n",scale) ;
  char f[127] ;
  sprintf(f, "set scale %f", scale) ;
  if (shmNav->write(f)) {
    fprintf(stderr,"%s: error writing to shared memory\n",prog) ;
    exit(1) ;
  }
  textDisplay->value(scale) ;
}

////////////////////////////////////////////////////////////////////////
static void rollerResponseCallback(Fl_Widget *w) {
  rollerResponse = (float)roller->value() ;
  //~printf("rollerResponseCallback called, value = %f   ",rollerResponse) ;
  response = pow(rollerResponse,power)*(maxResponse-minResponse) + minResponse ;

  //~printf("setting response to %f\n",response) ;
  char f[127] ;
  sprintf(f, "set response %f", response) ;
  if (shmNav->write(f)) {
    fprintf(stderr,"%s: error writing to shared memory\n",prog) ;
    exit(1) ;
  }
  textDisplay->value(response) ;
}

////////////////////////////////////////////////////////////////////////
static void scaleCallback(Fl_Widget *w) {
  //~printf("scaleCallback called\n") ;
  textDisplay->value(scale) ;
  roller->value(rollerScale) ;
  roller->callback(rollerScaleCallback) ;
}

////////////////////////////////////////////////////////////////////////
static void responseCallback(Fl_Widget *w) {
  //~printf("responseCallback called\n") ;
  textDisplay->value(response) ;
  roller->value(rollerResponse) ;
  roller->callback(rollerResponseCallback) ;
}

////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
  
  int argsUsed ;

  Fl::args(argc, argv, argsUsed) ;
		 
  prog = argv[0] ;

  string navList ;

  while (argsUsed<argc) {
    int l = strlen(argv[argsUsed]) ;
    if (l<5)
      l=5 ;
    
    if ((argsUsed<argc) && !strncasecmp(argv[argsUsed],"--wscale",l)) {
      argsUsed++ ;
      if (argsUsed>=argc) {
	usage(prog) ;
	fprintf(stderr,"\nMissing wscale parameter\n") ;
	return 1 ;
      } else if (!(DGLUtil::string_to_float(argv[argsUsed], &wscale)) || wscale<=0.f) {
	usage(prog) ;
	fprintf(stderr,"\nInvalid wscale, \"%s\"\n",argv[argsUsed]) ;
	return 1 ;
      } else {
	argsUsed++ ;
      }
      
    } else if ((argsUsed<argc) && !strncasecmp(argv[argsUsed],"--scale",l)) {
      argsUsed++ ;
      if (argsUsed+2>=argc) {
	usage(prog) ;
	fprintf(stderr,"\nMissing scale parameter(s)\n") ;
	return 1 ;
      } else {
	if (!(DGLUtil::string_to_float(argv[argsUsed], &minScale)) || minScale<=0.f) {
	  usage(prog) ;
	  fprintf(stderr,"\nInvalid minimum scale, \"%s\"\n",argv[argsUsed]) ;
	  return 1 ;
	}
	argsUsed++ ;
	if (!(DGLUtil::string_to_float(argv[argsUsed], &initScale)) || initScale<=0.f) {
	  usage(prog) ;
	  fprintf(stderr,"\nInvalid initial scale, \"%s\"\n",argv[argsUsed]) ;
	  return 1 ;
	}
	argsUsed++ ;
	if (!(DGLUtil::string_to_float(argv[argsUsed], &maxScale)) || maxScale<=0.f) {
	  usage(prog) ;
	  fprintf(stderr,"\nInvalid maximum scale, \"%s\"\n",argv[argsUsed]) ;
	  return 1 ;
	}
	argsUsed++ ;
      }
      
    } else if ((argsUsed<argc) && !strncasecmp(argv[argsUsed],"--response",l)) {
      argsUsed++ ;
      if (argsUsed+2>=argc) {
	usage(prog) ;
	fprintf(stderr,"\nMissing response parameter(s)\n") ;
	return 1 ;
      } else {
	if (!(DGLUtil::string_to_float(argv[argsUsed], &minResponse)) || minResponse<=0.f) {
	  usage(prog) ;
	  fprintf(stderr,"\nInvalid minimum response, \"%s\"\n",argv[argsUsed]) ;
	  return 1 ;
	}
	argsUsed++ ;
	if (!(DGLUtil::string_to_float(argv[argsUsed], &initResponse)) || initResponse<=0.f) {
	  usage(prog) ;
	  fprintf(stderr,"\nInvalid initial response, \"%s\"\n",argv[argsUsed]) ;
	  return 1 ;
	}
	argsUsed++ ;
	if (!(DGLUtil::string_to_float(argv[argsUsed], &maxResponse)) || maxResponse<=0.f) {
	  usage(prog) ;
	  fprintf(stderr,"\nInvalid maximum response, \"%s\"\n",argv[argsUsed]) ;
	  return 1 ;
	}
	argsUsed++ ;
      }
      
    } else if ((argsUsed<argc) && !strncasecmp(argv[argsUsed],"--navs",l)) {
      argsUsed++ ;
      if (argsUsed>=argc) {
	usage(prog) ;
	fprintf(stderr,"\nMissing navs parameter\n") ;
	return 1 ;
      } else {
	navList = argv[argsUsed] ;
	argsUsed++ ;
      }
      
    } else {
      usage(prog) ;
      fprintf(stderr,"\nInvalid option, \"%s\"\n",argv[argsUsed]) ;
      return 1 ;
    }
  }
  

  //////////////////////////////////////////////////////////////////////
  // shared memory games

  // try to connect to shared memory created by shmNavControl
  // see if shared memory already exists
  dtkSharedMem *old_shm = new dtkSharedMem(SHM_NAV_NAME, 0) ;

  // it does, but is of wrong size
  if (old_shm && old_shm->isValid() && (old_shm->getSize()!=SHM_NAV_SIZE)) {
    fprintf(stderr, "%s: shared memory \"%s\" already exists, but wrong size.\n", 
	    prog, SHM_NAV_NAME) ;
    return 1 ;
  }
  
  // it doesn't, so create a new one
  if ((!old_shm) || 
      (old_shm && old_shm->isInvalid())) { // have to create a new segment
    shmNav = new dtkSharedMem(SHM_NAV_SIZE, SHM_NAV_NAME) ;
    if(!shmNav || shmNav->isInvalid()) {
      fprintf(stderr, "%s: couldn't open shared memory segment %s\n",
	      prog, SHM_NAV_NAME) ; 
      return 1 ;
    }
  } else { // reuse old segment
    shmNav = old_shm ;
  }
  
  char **navNames = NULL ;
  int navs = 0 ;

  if (!navList.empty()) {
    // turn colon-separated list into array
    // strip out leading colons
    while (navList[0] == ':') {
      navList = navList.substr(1) ;
    }
    // strip out trailing colons
    while (navList[navList.length()-1] == ':') {
      navList = navList.substr(0,navList.length()-1) ;
    }
    // make matching easier by putting back a colon at the beginning of the string
    navList = ':' + navList ;
    // work backwards through the string, so the last item is the default
    while (1) {
      string::size_type i = navList.rfind(':') ;
      if (i==string::npos)
	break ;
      navs++ ;
      navNames = (char**) realloc(navNames, navs*sizeof(char*)) ;
      string n = navList.substr(i+1) ;
      navNames[navs-1] = strdup(n.c_str()) ;
      navList = navList.substr(0,i) ;
    }
  }

  // set height of window based on number of navigations
  int h =  10+(30*navs) ;
  if (navs<2)
    h = 10+(30*2) ;
  h=(int)(h*wscale) ;

  // size of text in text box, and label size
  uchar textsize = (uchar)(14*wscale) ;

  Fl_Window *window = 
    new Fl_Window((int)(590*wscale), h);
  {

    Fl_Light_Button **navButtons = NULL ;

    Fl_Group* o = new Fl_Group(0, 0, (int)(180*wscale), h);
    {
      for (int i=0; i<navs; i++) {
	Fl_Light_Button* o ;
	navButtons = (Fl_Light_Button**) realloc(navButtons, (i+1)*sizeof(Fl_Light_Button*)) ;
	o = navButtons[i] = 
	  new Fl_Light_Button((int)(5*wscale), (int)((5+(30*i))*wscale), 
			      (int)(165*wscale), (int)(25*wscale), navNames[i]);
	o->type(FL_RADIO_BUTTON);
	o->callback(navCallback) ;
	o->labelsize(textsize) ;
	if (i==0)
	  o->value(1) ;
	else 
	  o->value(0) ;
      }
      o->end() ;
    }
    { 
      Fl_Group* o = new Fl_Group((int)(180*wscale), 0, (int)(430*wscale), (int)(100*wscale));
      {
	textDisplay = new Fl_Value_Output((int)(350*wscale), (int)(35*wscale), 
					  (int)(55*wscale), (int)(30*wscale));
	textDisplay->textsize(textsize) ;

	scaleButton = new Fl_Light_Button((int)(190*wscale), (int)(5*wscale), 
					  (int)(100*wscale), (int)(25*wscale), 
					  "scale");
	scaleButton->type(FL_RADIO_BUTTON);
	scaleButton->value(0) ;
	scaleButton->callback(scaleCallback) ;
	scaleButton->labelsize(textsize) ;
      }
      {
	responseButton = new Fl_Light_Button((int)(190*wscale), (int)(35*wscale), 
					     (int)(100*wscale), (int)(25*wscale), 
					     "response");
	responseButton->type(FL_RADIO_BUTTON);
	responseButton->value(1) ;
	responseButton->callback(responseCallback) ;
	textDisplay->value(response) ;
	responseButton->labelsize(textsize) ;
      }
      o->end();
    }
    {
      Fl_Button*o = new Fl_Button((int)(495*wscale), (int)((h-30)*wscale), 
				  (int)(90*wscale), (int)(25*wscale), 
				  "Reset");
      o->callback(resetCallback) ;
      o->labelsize(textsize) ;
    }
    {
      roller = new Fl_Roller((int)(305*wscale), (int)(5*wscale), 
			     (int)(135*wscale), (int)(25*wscale));
      roller->type(FL_HORIZONTAL);
      roller->range(0, 1.f) ;
      roller->value(rollerResponse) ;
      roller->step(.0005f) ;
      roller->callback(rollerResponseCallback) ;
      roller->labelsize(textsize) ;
    }
  }

  // set initial values
  // pass NULL, don't update shared memory- not called by a button press, but just to init data
  resetCallback(NULL) ;

  window->end();
  window->show(argc, argv);

  while (Fl::wait()) {
  }
  return 0 ;
}
