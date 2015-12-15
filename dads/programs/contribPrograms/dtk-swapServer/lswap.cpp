#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <dtk.h>
#include "lswap.h"

lswap::lswap(void) 
{
	//Someone set us up the shared memory
	headShm = new dtkSharedMem(sizeof(float)*6, "linux-head");
	wandShm = new dtkSharedMem(sizeof(float)*6, "linux-wand");
	joystickShm = new dtkSharedMem(sizeof(float)*2, "linux-joystick");
	buttonShm = new dtkSharedMem(sizeof(char)*1, "linux-buttons");

	ihead = new dtkSharedMem(sizeof(float)*6, "head");
	iwand = new dtkSharedMem(sizeof(float)*6, "wand");
	ijoystick = new dtkSharedMem(sizeof(float)*2, "joystick");
	ibuttons = new dtkSharedMem(sizeof(char)*1, "buttons");

	VR1 = new float[6];
	memset(VR1, sizeof(float)*6, 0);
	memset(joystick, sizeof(float)*2, 0);
	memset(&buttons, sizeof(unsigned char), 0);
	if (VRCO.isInvalid())
		fprintf(stderr, "Bad VRCO SHARED MEMORY ARGH\n");

}


/**********************************************************************
  read a record from the tracker.  returns 1 if data updated,
  0 if no change, and -1 if an error
 **********************************************************************/

#define CHUCK  ((size_t) 120)  /* about the largest buffer read size */

/* data record offsets */

#define X 3
#define Y 7
#define Z 11


#define H 15 /* heading */
#define P 19 /* pitch */
#define R 23 /* roll */

#define BUTTONS 27
#define JOYSTICK 28

void lswap::setup()
{
		//Remote connector to the io server
		RConnect = new dtkClient();
		if (RConnect->isInvalid())
		{
			fprintf(stderr,"Error with connector\n");
		}
		//The computer we want to connect to
		printf("Connecting to io now\n");
		RConnect->connectServer("io.sv.vt.edu");
		printf("Connected\n");
		//The shared memory segements we want to connect to
		fprintf(stderr,"Connecting segments\n");
		RConnect->connectRemoteSharedMem("linux-head", "io.sv.vt.edu", "head");
		//printf("1");
		RConnect->connectRemoteSharedMem("linux-wand", "io.sv.vt.edu", "wand");
		//printf("2");
		RConnect->connectRemoteSharedMem("linux-joystick", "io.sv.vt.edu", "joystick");
		//printf("3");
		RConnect->connectRemoteSharedMem("linux-buttons", "io.sv.vt.edu", "buttons");
		//printf("4");
		//fd =2;
}

			
int lswap::serve(void)
{
//Read linux shm and convert if necessary
	buttonShm->read(&buttons);
	if (buttonShm->differentByteOrder())
		dtk_swapBytes(&buttons, sizeof(unsigned char));


	headShm->read(VR1);
	if (headShm->differentByteOrder())
	{
		dtk_swapBytes(VR1, sizeof(float)*6);
	for (int j=0; j<6; j++)
		temp[5-j] = VR1[j];
	for (int j=0; j<6; j++)
		VR1[j] = temp[j];
	ihead->write(VR1);
	VRCO.writeHeadTracker((const float*)VR1);
	}

	wandShm->read(VR1);
	if (wandShm->differentByteOrder())
	{
	dtk_swapBytes(VR1, sizeof(float)*6);
	for (int j=0; j<6; j++)
		temp[5-j] = VR1[j];
	for (int j=0; j<6; j++)
		VR1[j] = temp[j];
	}
	//Read joystick
	joystickShm->read(joystick);
	//Swap the bytes
	if (joystickShm->differentByteOrder())
	{
		dtk_swapBytes(joystick, sizeof(float)*2);
		//Swap the position in the array
		for (int j=0; j<2; j++)
			temp[1-j] = joystick[j];
		for (int j=0; j<2; j++)
			joystick[j] = temp[j];
	iwand->write(VR1);
	ijoystick->write(joystick);
	ibuttons->write(&buttons);
	VRCO.writeWand((const float*)VR1, (const float*)joystick, buttons);
	}
//Write Irix shm
//WTF test
	//printf("Value = %f %f %f %f %f %f\n", VR1[0], VR1[1], VR1[2], VR1[3], VR1[4], VR1[5]);
	//printf("After = %f %f %f %f %f %f\n", head[0], head[1], head[2], head[3], head[4], head[5]);
	return DTKSERVICE_CONTINUE;
}

lswap::~lswap(void)
{
	fprintf(stderr, "Ending lswap service\n");
}
