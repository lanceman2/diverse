//=============================================================================
// helix.cpp
// - This class demonstrates how to build a simple DGL application with
// animation.  
//
// Created by Andrew A. Ray 2/5/04
//=============================================================================
#include<iostream>
#include<math.h>
#include<dgl.h>
#include <GL/gl.h>
using namespace std;

#define DELTA_THETA .174532292f
#define DELTA_HEIGHT 0.4
#define R1 1000
#define R2 4000
#define NUM_TURNS 1000
#define REVS_PER_MINUTE 15.f

float current_angle;
float data[4];
GLuint helix_list;

void drawHelix();

void draw()
{
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    //This is if you want a wireframe model
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_BLEND);

    float light_ambient[]   = {.3f, .3f, .3f, 1.f};
    float light_diffuse[]   = {.3f, .3f, .3f, 1.f};
    float light_direction[] = {0.f, -1.f, 1.f, 1.f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);

  
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glScalef( .025, .025, .025 );
    glTranslatef(0, 20000.f, 7000.f);
    glRotatef(current_angle, 0.f, 0.f, 1.f); 
    drawHelix();
    glRotatef(180.f, 0.f, 0.f, 1.f);
    drawHelix();
    glPopMatrix();
}

void postconfig()
{
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_BLEND);
}

void drawHelix()
{
    
    float theta, h;
    float x1, y1, x2, y2, z, td;
    int i;

    theta = 0.f;
    h = 50;
    glBegin(GL_TRIANGLE_STRIP);
    for (i=0; i < NUM_TURNS * 36; i++) {
	z = h;
	x1 = R1 * sin (theta);
	y1 = R1 * cos (theta);
	x2 = R2 * sin (theta);
	y2 = R2 * cos (theta);
	td = theta * 180.f / M_PI;
	glColor3f(fabs(180.f - td) / 180.f,
		  fabs(180.f - fabs(td - 240.f)) / 180.f,
		  fabs(180.f - fabs(td - 120.f)) / 180.f);
	glVertex3f(x1, y1, z);
	glVertex3f(x2, y2, z);
	theta += DELTA_THETA;
	if (theta > 2 * M_PI)
	    theta -= 2 * M_PI;
	h -= DELTA_HEIGHT;    
    }
    glEnd();
}

void update()
{
    current_angle += 1;
    if( current_angle > 360 ) current_angle -= 360;
}


int main( int argc, char** argv )
{
    //Global variable init
    current_angle = 0;

    //Create DGL
    dglInit();

    //Now set the draw callbacks
    //If you want to have a draw callback that is not navigated
    //Use uncomment the following call and put your draw callback here
    //dglPreNavCallback( draw );
    dglDisplayCallback( draw );
    //If you want to have an overlay that is drawn after everything else then
    //put it here
    //dglOverlayCallback(draw);

    //Set the update callback
    dglPostFrameCallback( update );
    //Start the DGL system
    dglStart();

    /*Code 
    //These are code examples on how to get navigation information, mouse information, and button information
    dtkNavList* navList;
    navList = (dtkNavList *) DGL::getApp()->check("dtkNavList", DTKNAVLIST_TYPE) ;
    int numNav=0;
    dtkNav* test = navList->first();
    while (test != NULL)
    {
    numNav++;
    test = navList->next(test);
    }
	
    printf("There are %d navigations\n", numNav);
    dtkInValuator* pointer;
    pointer = (dtkInValuator*)DGL::getApp()->get("pointer", DTKINVALUATOR_TYPE);
    printf("There are %d buttons on the pointer\n", pointer->number());
    float pointpos[2];

    dtkInButton* button;
    button = (dtkInButton*) DGL::getApp()->get("buttons", DTKINBUTTON_TYPE);
    printf("There are %d buttons in the button\n", button->number());
    */

    while (dglIsRunning())
    {
	dglFrame();
	/*
	//Example code on how to read the navigations position and whether or not a button was pressed.  Also includes how to read the mouses position.
	dtkNav* nav =NULL;
	if (navList != NULL)
	nav = navList->current();
	if (nav)
	{
			
	printf("Name is %s Position is %f %f %f %f %f %f\n", nav->getName(), nav->location.x, nav->location.y, nav->location.z, nav->location.h, nav->location.p, nav->location.r);
	}
	if(button->read(0))
	{
	printf("Button 0 pressed!\n");
	} 
	if(button->read(1))
	{
	printf("Button 1 pressed!\n");
	} 
	if(button->read(2))
	{
	printf("Button 2 pressed!\n");
	} 
	*/
	//pointer->read(pointpos);
	//printf("%f %f\n", pointpos[0], pointpos[1]);
	//This reduces CPU usage, but decreases performance
	//usleep(1);
    }
}
