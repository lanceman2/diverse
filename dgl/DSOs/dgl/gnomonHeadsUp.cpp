#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>
#include <math.h>

/*!
 * @class gnomonHeadsUp
 *
 * @brief DSO which displays the navigational orientation in the lower right
 * corner
 * 
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
 */
class gnomonHeadsUp : public dglAugment
{
    public:
        gnomonHeadsUp(char* Name);
	virtual int draw();
private:
	DGL*  m_app;
};

gnomonHeadsUp::gnomonHeadsUp( char* Name)
:dglAugment( Name, dglAugment::OVERLAY)
{
    setDescription( "Displays the navigation orientation in the lower right corner." );
    m_app = DGL::getApp();
}
#define SIZE 40.0f
int gnomonHeadsUp::draw()
{
    GLint winsize[4];
    glGetIntegerv( GL_VIEWPORT, winsize );
    float width = winsize[2], height = winsize[3];

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0.0f, width, 0.0f, height, -1.0f, 1.0f );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable( GL_LIGHTING );
    glDisable( GL_BLEND );
	glDisable( GL_TEXTURE_1D );
	glDisable( GL_TEXTURE_2D );
#ifdef GL_TEXTURE_3D
	glDisable( GL_TEXTURE_3D );
#endif
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glLineWidth( 2.0f );
    
    glScalef( SIZE, SIZE, 1.0f );
    glTranslatef( width/SIZE - 1.0f, 1.0f, 0.0f);
    glPushMatrix();

    //dVec3 x(1.0f, 0.0f, 0.0f), y(0.0f, 0.0f, 1.0f), z(0.0f, 1.0f, 0.0f);
    dglVec3<float> x(1.0f,0.0f,0.0f);
    dglVec3<float> y(0.0f,1.0f,0.0f);
    dglVec3<float> z(0.0f,0.0f,1.0f);
    #ifdef __APPLE__
     #ifdef check
      #undef check
     #endif // check
    #endif // __APPLE__
    dtkNavList* navList = (dtkNavList*)m_app->check("dtkNavList", DTKNAVLIST_TYPE);
    dtkNav* nav = NULL;
    if (navList !=NULL)
    	nav = navList->current();
    if (nav !=NULL)
    {
	glRotatef(90,1,0,0);
        glRotatef(nav->location.h, 0.0f, 0.0f, 1.0f);
        glRotatef(nav->location.p, 1.0f, 0.0f, 0.0f);
        glRotatef(nav->location.r, 0.0, 1.0f, 0.0f);
    }
    	

    /*
    dMatrix mat = NavManager::getMatrix();
    mat.setTrans( 0.0f, 0.0f, 0.0f );
	dMatrix rot;
	rot = rot.rotate( -DEG2RAD( 90.0f ), 1.0f, 0.0f, 0.0f );
	mat *= rot;
    float* p_mat = mat.inverse( mat ).ptr();
    glMultMatrixf( p_mat );
    */

    glColor3f(1.0f, 0.0f, 0.0f) ;
    glBegin(GL_LINES) ;
    glVertex3f(0.f, 0.f, 0.f) ;
    glVertex3f(x.x, x.z, -x.y) ;

    glColor3f(0.0f, 1.0f, 0.0f) ;
    glVertex3f(0.f, 0.f, 0.f) ;
    glVertex3f(y.x, y.z, -y.y) ;

    glColor3f(0.0f, 0.0f, 1.0f) ;
    glVertex3f(0.f, 0.f, 0.f) ;
    glVertex3f(z.x, z.z, -z.y) ;
    glEnd();

    glPopMatrix();

    float angle;
    glColor3f(1.0f, 1.0f, 1.0f) ;
    glBegin(GL_LINE_LOOP);
    for (angle = 0; angle <= 360; angle += 5)
    {
      glVertex2f(cosf(DEG2RAD(angle)), sinf(DEG2RAD(angle)));
    }
    glEnd();

    glEnable(GL_BLEND) ;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
    glColor4f(1.f, 1.f, 1.f, .5f) ;
    glBegin(GL_POLYGON);
    for (angle = 0; angle <= 360; angle += 5)
    {
      glVertex3f(cosf(DEG2RAD(angle)), sinf(DEG2RAD(angle)), 0.0f);
    }
    glEnd();

    glPopAttrib() ;
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
	return dtkAugment::CONTINUE;
}

/*************** DTK C++ DSO loader/unloader functions ***********/
/*
 * All DTK DSO files are required to declare these two functions.
 * These function are called by the loading program to get your
 * C++ objects loaded.
 *
 *****************************************************************/
static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
	DGL::init();
	DGL* app = static_cast<DGL*>(manager); 
	gnomonHeadsUp* temp = new gnomonHeadsUp("gnomonDisplay");
	app->addOverlay(temp);
  
    return DTKDSO_LOAD_CONTINUE;
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  return DTKDSO_UNLOAD_CONTINUE;
}
