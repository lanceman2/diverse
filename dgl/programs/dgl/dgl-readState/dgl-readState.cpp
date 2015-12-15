#include <dtk.h>
#include <stdio.h>
#ifndef DTK_ARCH_WIN32_VCPP
#  include <unistd.h>
#endif
#include <curses.h>
#include <signal.h>

#define angle(X)    ((((int)X+180)%360)-180)

int running = true ;

static void float9(char *s, float f) ;
static void finish(int sig) ;
static void out_int(int r, int c, int i) ;
static void out_float(int r, int c, float f) ;
static dtkSharedMem *open_shm(char *name) ;

int main(int argc, char **argv) 
{

  
  dtkSharedMem *frame_shm = open_shm("dgl/state/frame") ;

  dtkSharedMem *time_shm = open_shm("dgl/state/time") ;

  dtkSharedMem *origin_shm = open_shm("dgl/state/origin") ;
  dtkSharedMem *scale_shm = open_shm("dgl/state/scale") ;
  dtkSharedMem *meters_shm = open_shm("dgl/state/meters") ;

  dtkSharedMem *location_shm = open_shm("dgl/state/location") ;
  dtkSharedMem *pivot_shm = open_shm("dgl/state/pivot") ;
  dtkSharedMem *response_shm = open_shm("dgl/state/response") ;
  dtkSharedMem *navScale_shm = open_shm("dgl/state/navScale") ;
  dtkSharedMem *navName_shm = open_shm("dgl/state/navName") ;

  dtkSharedMem *head_shm = open_shm("head") ;
  dtkSharedMem *wand_shm = open_shm("wand") ;
  dtkSharedMem *joystick_shm = open_shm("joystick") ;
  dtkSharedMem *buttons_shm = open_shm("buttons") ;
  
  initscr(); 
  nonl() ;
  cbreak() ;
  noecho() ;
  signal(SIGINT, finish) ;

  unsigned int frame = 0 ;
  unsigned int last_frame = 0 ;
  float time = 0.f ;
  float last_time = 0.f ;
  float rate = 0 ;

  float origin[6] ;
  float scale ;
  float meters ;

  float location[6] ;
  float pivot[6] ;
  float response ;
  float navScale ;
  char navName[100] ;

  float head[6] ;
  float wand[6] ;
  float joystick[2] ;
  unsigned char buttons ;
  
  int row = 0 ;
  while (running) 
    {
      row = 0 ;
      clear() ;
      timeout(100) ;
 #if 0
      {
	static int i = 0 ;
	int rows, cols ;
	getmaxyx(stdscr,rows,cols);
	mvprintw(row, 0, "frame = %4d, rows = %d, cols = %d\n",i,rows,cols) ;
	row++ ;
	i++ ;
      }
#endif
      
      if (frame_shm)
	{
	  frame_shm->read(&frame) ;
	  mvprintw(row, 2,"Frame %5d",frame) ;
	}

      if (time_shm)
	{
	  time_shm->read(&time) ;
	  mvprintw(row, 15,"Time") ;
	  out_float(row, 20, time) ;
	}


      if (frame_shm && time_shm)
	{
	  if (time-last_time>0.f)
	    {
	      rate = (float)(frame-last_frame)/(time-last_time) ;
	      last_frame = frame ;
	      last_time = time ;
	    }
	  mvprintw(row, 31,"Frame/Time") ;
	  out_float(row, 42, rate) ;
	}
      row++ ;

      mvprintw(row,10,"X         Y         Z         H    P    R    S") ;
      row++ ;
      
      if (origin_shm)
	{
	  mvprintw(row,2,"Origin") ;
	  origin_shm->read(origin) ;
	  out_float(row, 9,origin[0]) ;
	  out_float(row,19,origin[1]) ;
	  out_float(row,29,origin[2]) ;
	  out_int(row,39,angle(origin[3])) ;
	  out_int(row,44,angle(origin[4])) ;
	  out_int(row,49,angle(origin[5])) ;
	}

      if (scale_shm)
	{
	  scale_shm->read(&scale) ;
	  out_float(row, 54,scale) ; 
	}
      row++ ;
      if (meters_shm)
	{
	  mvprintw(row,2,"Meters") ;
	  meters_shm->read(&meters) ;
	  out_float(row,9,meters) ; 
	}
      row+=2 ;


      if (navName_shm)
	{
	  navName_shm->read(&navName) ;
	  mvprintw(row, 0,navName) ; 
	}
      row++ ;
      if (location_shm)
	{
	  mvprintw(row,5,"Nav") ;
	  location_shm->read(location) ;
	  out_float(row, 9,location[0]) ;
	  out_float(row,19,location[1]) ;
	  out_float(row,29,location[2]) ;
	  out_int(row,39,angle(location[3])) ;
	  out_int(row,44,angle(location[4])) ;
	  out_int(row,49,angle(location[5])) ;
	}
      if (navScale_shm)
	{
	  navScale_shm->read(&navScale) ;
	  out_float(row, 54,navScale) ; 
	}
      row++ ;

      if (pivot_shm)
	{
	  mvprintw(row,3,"Pivot") ;
	  pivot_shm->read(pivot) ;
	  out_float(row, 9,pivot[0]) ;
	  out_float(row,19,pivot[1]) ;
	  out_float(row,29,pivot[2]) ;
	}
      if (response_shm)
	{
	  mvprintw(row,45,"Response") ;
	  response_shm->read(&response) ;
	  out_float(row, 54,response) ; 
	}
      row+=2 ;

      mvprintw(row,0,"CAVE devices") ;
      row++ ;
      if (head_shm)
	{
	  mvprintw(row,4,"Head") ;
	  head_shm->read(head) ;
	  out_float(row, 9,head[0]) ;
	  out_float(row,19,head[1]) ;
	  out_float(row,29,head[2]) ;
	  out_int(row,39,angle(head[3])) ;
	  out_int(row,44,angle(head[4])) ;
	  out_int(row,49,angle(head[5])) ;
	}
      row++ ;

      if (wand_shm)
	{
	  mvprintw(row,4,"Wand") ;
	  wand_shm->read(wand) ;
	  out_float(row, 9,wand[0]) ;
	  out_float(row,19,wand[1]) ;
	  out_float(row,29,wand[2]) ;
	  out_int(row,39,angle(wand[3])) ;
	  out_int(row,44,angle(wand[4])) ;
	  out_int(row,49,angle(wand[5])) ;
	}
      row++ ;
      if (joystick_shm)
	{
	  mvprintw(row,0,"Joystick") ;
	  joystick_shm->read(joystick) ;
	  out_float(row, 9,joystick[0]) ;
	  out_float(row,19,joystick[1]) ;
	}

      if (buttons_shm)
	{
	  int col = 30 ;
	  mvprintw(row,col,"Buttons") ;
	  buttons_shm->read(&buttons) ;
	  if (buttons & 1)
	    mvprintw(row,col+8,"L") ;
	  else
	    mvprintw(row,col+8,"-") ;
	  if (buttons & 2)
	    mvprintw(row,col+9,"M") ;
	  else
	    mvprintw(row,col+9,"-") ;
	  if (buttons & 4)
	    mvprintw(row,col+10,"R") ;
	  else
	    mvprintw(row,col+10,"-") ;
	  if (buttons & 8)
	    mvprintw(row,col+11,"4") ;
	  else
	    mvprintw(row,col+11,"-") ;
	}
      row++ ;

#if 0
      mvprintw(row,0,"World") ;
      row++ ;
      {
	// build a matrix to go from diverse to world coordinates
	dtkMatrix worldMat ;
	if (origin_shm)
	  {
	    worldMat.translate(origin[0], origin[1], origin[2]) ;
	    worldMat.rotateHPR(origin[3], origin[4], origin[5]) ;
	  }
	if (scale_shm)
	  {
	    worldMat.scale(scale) ;
	  }
	if (location_shm)
	  {
	    worldMat.translate(location[0], location[1], location[2]) ;
	    worldMat.rotateHPR(location[3], location[4], location[5]) ;
	  }
	if (navScale_shm)
	  {
	    worldMat.scale(navScale) ;
	  }
	worldMat.invert() ;
	dtkMatrix headMat ;

	if (head_shm)
	  {
	    headMat.mult(&worldMat) ;
	    headMat.translate(head[0], head[1], head[2]) ;
	    headMat.rotateHPR(head[3], head[4], head[5]) ;
	    float worldHead[6] ;
	    headMat.translate(&(worldHead[0]), &(worldHead[1]), &(worldHead[2])) ;
	    headMat.rotateHPR(&(worldHead[3]), &(worldHead[4]), &(worldHead[5])) ;
	    mvprintw(row,4,"Head") ;
	    out_float(row, 9,worldHead[0]) ;
	    out_float(row,19,worldHead[1]) ;
	    out_float(row,29,worldHead[2]) ;
	    out_int(row,39,angle(worldHead[3])) ;
	    out_int(row,44,angle(worldHead[4])) ;
	    out_int(row,49,angle(worldHead[5])) ;
	    row++ ;
	  }

      }
#endif

      move(row,0) ;
      refresh() ;
      char c ;
      c = getch() ;
      if (c==033)
	running=false ;
    }
  endwin();
  return 0 ;
}

static void out_int(int r, int c, int i)
{
  mvprintw(r, c, "%-+3.3d", i) ;
}
static void out_float(int r, int c, float f) 
{
  char s[20] ;
  float9(s,f) ;
  mvprintw(r, c, s) ;
}

static dtkSharedMem *open_shm(char *name)
{
  dtkSharedMem *shm = NULL ;
  shm =  new dtkSharedMem(name) ; 
  if (!shm || shm->isInvalid()) 
    {
      dtkMsg.add(DTKMSG_INFO,
		 "dgl-readState:"
		 " couldn't open shared memory segment\"%s\"\n",name) ;
      shm = NULL ; 
    }
  return shm ;
}

static void finish(int sig)
{
endwin() ;
  running = false ;
}

void float9(char *s, float f) {
  float a ; 
  a = f<0?-f:f ;
  
  if (a==0.0f) 
    sprintf(s,"%-+8.6f",f) ;
  else if (a>=100000000.f || a<=.0001f)
    sprintf(s,"%-+8.2e",f) ;
  else if (a<10.f)
    sprintf(s,"%-+8.6f",f) ;
  else if (a<100.f)
    sprintf(s,"%-+8.5f",f) ;
  else if (a<1000.f)
    sprintf(s,"%-+8.4f",f) ;
  else if (a<10000.f)
    sprintf(s,"%-+8.3f",f) ;
  else if (a<100000.f)
    sprintf(s,"%-+8.2f",f) ;
  else if (a<1000000.f)
    sprintf(s,"%-+8.1f",f) ;
  else if (a<10000000.f)
    sprintf(s,"%-+#8.0f",f) ;
  else if (a<100000000.f)
    sprintf(s,"%-+8.0f",f) ;
}
