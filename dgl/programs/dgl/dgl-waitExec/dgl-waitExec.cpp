#include <dtk.h>
#include <stdio.h>
#ifndef DTK_ARCH_WIN32_VCPP
#  include <unistd.h>
#endif
#include <stdlib.h>

int main(int argc, char **argv) 
{

    int numFrames = 3 ; //default minimum value of frame counter
    char **eargv = NULL ;
    bool init = false ;

    if (argc<2)
    {
	dtkMsg.add(DTKMSG_ERROR,"Usage dgl-waitExec [ --init ] [ --frames n ] file [arg ...]\n") ;
	return 1 ;
    }
  
    int c=1 ;
    while (!strcmp(argv[c],"--frames") || !strcmp(argv[c],"--init"))
    {
	if (!strcmp(argv[c],"--frames"))
	{
	    c++ ;
	    if (sscanf(argv[c], "%d", &numFrames) != 1)
	    {
		dtkMsg.add(DTKMSG_ERROR,"dgl-waitExec: invalid frame option \"%s\"\n",argv[c]) ;
		return 1 ;
	    }
	    c++ ;
	}
	else if (!strcmp(argv[c],"--init"))
	{
	    c++ ;
	    init = true ;
	}
    }


    //printf("waiting for %d frames\n",numFrames) ;
    eargv = (char **) malloc(sizeof(char*)*(argc-c+1)) ;
    std::string command ;
    for (int i=c; i<argc; i++)
    {
	//printf("argv[%d] = %s\n",i-c,argv[i]) ;
	eargv[i-c] = argv[i] ;
	command += argv[i] ;
	if (i<argc-1) command += " " ;
    }
    eargv[argc-c] = NULL ;
    //printf("command = \"%s\"\n",command.c_str()) ;

    dtkSharedMem *shm = NULL ;
    unsigned int frame = 0 ;
    while (1) 
    {
	if (!shm)
	    shm =  new dtkSharedMem("dgl/state/frame") ; 
	if (!shm || shm->isInvalid()) 
	    shm = NULL ; 
      
	if (shm)
	{
	    if (init)
	    {
		//printf("initializing\n") ;
		frame = 0 ;
		shm->write(&frame) ;
		init = false ;
	    }
	    
	    shm->read(&frame) ;
	    //printf("frame = %d\n",frame) ;
	    if (frame>=numFrames)
	    {
		execvp(eargv[0],eargv) ;
		dtkMsg.add(DTKMSG_ERROR,"%s: exec failed! command: \"%s\"\n",argv[0],command.c_str()) ;
		return 1 ;
	    }
	}
	usleep(100000) ;
    }
  
    return 0 ;
}


