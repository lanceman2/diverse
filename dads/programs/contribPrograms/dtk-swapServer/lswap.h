#ifndef _lswap_h_
#define _lswap_h_
#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <dtk.h>

//Class to map the shared memory
//from the io server to the cave
//and provide backward compatability
class lswap 
{
public:
  lswap(void);
  virtual ~lswap(void);
  int serve(void);
  void setup();
private:
//Linux shm segments
  dtkSharedMem* headShm;
  dtkSharedMem* buttonShm;
  dtkSharedMem*  joystickShm;
  dtkSharedMem*  wandShm;

//Irix shm segments
dtkSharedMem* ihead;
dtkSharedMem* ibuttons;
dtkSharedMem*  ijoystick;
dtkSharedMem*  iwand;

//Compatability
dtkVRCOSharedMem VRCO;

//Remote connection to the io server
dtkClient* RConnect;

//Temp buffers
	float joystick[2];
	float* VR1;
	unsigned char buttons;
	float temp[6];
};
#endif
