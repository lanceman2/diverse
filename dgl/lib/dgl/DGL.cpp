//Here because of DTK magic numbers
#include <dtk/__manager_magicNums.h>
#include <OpenThreads/Mutex>
#include <dtk.h>
#include <dgl.h>

#ifdef DGL_ARCH_WIN32
#  define SEPARATOR ";"
#else
#  define SEPARATOR ":"
#endif

bool                    DGL::m_init             = false;
bool                    DGL::m_madedisplay      = false;
bool                    DGL::m_trackhead        = true;
bool                    DGL::m_dosetupgl		= true;
bool                    DGL::m_doinitgl			= true;
bool                    DGL::m_diversecoord     = true;
bool                    DGL::m_discreteStereo	= false;
dglPostConfigCB         DGL::m_postconfig       = NULL;
dglPreConfigCB          DGL::m_preconfig        = NULL;
dglDisplayCB            DGL::m_display          = NULL;
dglDisplayCB            DGL::m_display_discrete_left	= NULL;
dglDisplayCB            DGL::m_display_discrete_right	= NULL;
dglPostFrameCB        	DGL::m_preframe			= NULL;
dglPreFrameCB         	DGL::m_postframe		= NULL;
dglPreNavCB         	DGL::m_prenavcb			= NULL;
dglOverlayCB         	DGL::m_overlaycb		= NULL;
dglSetupGLCB         	DGL::m_setupglcb		= NULL;
dglInitGLCB         	DGL::m_initglcb			= NULL;
void*                   DGL::m_data             = NULL;
DGL*			DGL::m_instance		            = NULL;
dtkNavList*		DGL::m_navlist                  = NULL;
DGLDisplay*             DGL::m_dglDisplay       = NULL;
vector<dglAugment*>     DGL::m_overlays;
vector<dglAugment*>     DGL::m_augments;
vector<dglAugment*>     DGL::m_prenav;
float                   DGL::m_scale            =1.0;
float                   DGL::m_meters           =1.0;
dtkCoord                DGL::m_origin;
bool                    DGL::m_shutdownonescape = true ;
bool			DGL::m_multiThreaded = true;
float*			DGL::m_clearColor = NULL;
vector<OpenThreads::Mutex*>      DGL::m_locks;
vector<string>      		DGL::m_lockNames;

DGL::DGL(int argc, char** argv)
:dtkManager( argc, argv )
{
	m_instance = this;
	createLock("DEFAULT_LOCK");
	createLock("INITGL_LOCK");
	createLock("CALLBACK_LOCK");
	m_origin.zero();
	m_init = true;	
	m_madedisplay = false;

	if (isInvalid())
		dtkMsg.add(DTKMSG_DEBUG, "DGL::DGL failed.\n");
	setType(TYPE_DGL);
	m_display = NULL;
	setNoSyncForcing(__DTKMANAGER_NOSYNCFORCING_);
	setPublicPrePostFrames(__DTKMANAGER_PREPOSTFRAME_MAGIC_);

	//Set up the DSO search path
	char* DSOpath = NULL;
	dtkConfigure dtkConfig;

	//Get the path for the environmental variables
	char* DSOenvPath = getenv(DGL_DSO_PATH_ENVVAR);
	//Not going to be NULL supposedly...famous last words
	char* dtkDSOdir = dtkConfig.getString(dtkConfigure::AUGMENT_DSO_DIR);

	//CHECK ME w/ testing...This probably needs to be switched around
	//with the DSOenvPath being executed in the !=NULL clause
	//Build a path to the DSO load directories
	if (DSOenvPath == NULL )
	{
		DSOpath = (char*) dtk_malloc(strlen("." SEPARATOR) + strlen(DGL_DSO_DIR) + strlen(SEPARATOR) + strlen(dtkDSOdir) + 1);
		sprintf(DSOpath, "." SEPARATOR "%s" SEPARATOR "%s", DGL_DSO_DIR, dtkDSOdir);
	}
	else
	{
		DSOpath = (char*) dtk_malloc(strlen(DSOenvPath) + strlen(SEPARATOR "." SEPARATOR) + strlen(DGL_DSO_DIR) + strlen(SEPARATOR) + strlen(dtkDSOdir) + 1);
		sprintf(DSOpath, "%s" SEPARATOR "." SEPARATOR "%s" SEPARATOR "%s", DSOenvPath, DGL_DSO_DIR, dtkDSOdir);
	}

	//DTK manager call
	path(DSOpath);

	char* DSOfiles = NULL;
	char* DSOenvFiles = getenv(DGL_DSO_FILES_ENVVAR);
	
	if (DSOenvFiles)
		DSOfiles = dtk_strdup(DSOenvFiles);
	else if (argc > 0)
	{
		int len =0;
		int i=0;
		for (i=0; i< argc; i++)
			len += strlen(argv[i]) + 1;
		DSOfiles = (char*) dtk_malloc(len + 1);
		DSOfiles[0] = '\0';
		for (i=0; i<argc && argv[i]; i++)
		{
			strcat(DSOfiles, argv[i]);
			strcat(DSOfiles, SEPARATOR);
		}
		DSOfiles[len-1] = '\0';
	}
	dtkMsg.add(DTKMSG_DEBUG, "DGL:  DGL path = \"%s\"\n", path());
	dtkMsg.add(DTKMSG_DEBUG, "DGL:  DSO files = \"%s\"\n", DSOfiles);

	//Allocate the memory used for the clear color
	m_clearColor = new float[4];
	//Make black as the default color
	memset(m_clearColor, 0, sizeof(float)*4);

	validate();


	if (DSOfiles && load((const char*) DSOfiles))
	{
		free(DSOfiles);
		DSOfiles = NULL;
		dtkMsg.add(DTKMSG_ERROR, "DGL::DGL() failed on loading DSO's.\n");
		invalidate();
		return;
	}
	if(DSOfiles != NULL )
		free(DSOfiles);

	dtkAugment *a = NULL;
	if (getNext() == NULL)
	{
		dtkMsg.add(DTKMSG_DEBUG, "DGL::preConfig():  Found no dtkAugment objects loaded.  Loading the Default DSOs:\"%s\".\n", DEFAULT_DSOS);

		if (load(DEFAULT_DSOS))
		{
			invalidate();
			return ;
		}
		
	}
	do 
	{
		if((m_dglDisplay = dynamic_cast<DGLDisplay*>(a = getNext(a))))
			break;
	}
	while(a);

	if (m_dglDisplay == NULL)
	{
		dtkMsg.add(DTKMSG_DEBUG, "DGL::preConfig():  There is no DGLDisplay object loading %s the default.\n", DEFAULT_DISPLAY_DSO);
		if (load(DEFAULT_DISPLAY_DSO))
		{
			invalidate();
			return ;
		}

	}
        #ifdef __APPLE__
        #undef check
        #endif
        m_navlist = (dtkNavList *) check("dtkNavList", DTKNAVLIST_TYPE) ;
	if (!m_navlist)
	{
		m_navlist = new dtkNavList;
		if (add(m_navlist))
		{
			delete m_navlist;
			m_navlist = NULL;
			invalidate();
			return ;
		}
	}

	dtkMsg.add(DTKMSG_DEBUG,"%s-------- DGL::DGL() finished --------%s\n", dtkMsg.color.yel, dtkMsg.color.end);
}

DGL::~DGL()
{
	if(m_clearColor != NULL)
		delete m_clearColor;
	removeAll();
	if (m_madedisplay && m_dglDisplay)
	{
		delete m_dglDisplay;
		m_dglDisplay = NULL;
	}
}

DGLDisplay* DGL::display()
{
	if (m_instance && m_instance->isInvalid()) return NULL;
	if (m_dglDisplay)
	{
		return m_dglDisplay;
	}
	else 
	{
		m_dglDisplay = new DGLDisplay(m_instance);
		m_instance->add(m_dglDisplay);
		return m_dglDisplay;
	}
}

void DGL::init()
{
	if (!m_init)
	{
		new DGL(0, NULL);
	}
}

void DGL::init(int argc, char** argv)
{
	if (!m_init)
	{
		new DGL(argc, argv);
	}
}

void DGL::init(char* DSOFile0, ...)
{
	if (!m_init)
	{
		char** argv= (char**) dtk_malloc(2*sizeof(char*));
		argv[0] = (char*)DSOFile0;
		va_list ap;
		va_start(ap, DSOFile0);
		int argc=1; argv[argc] = (char*)va_arg(ap, char*);
		while ((argv[argc]))
		{
			argc++;
			argv = (char **) dtk_realloc(argv,sizeof(char *)*(argc+1));
			argv[argc] = (char *) va_arg(ap, char *);
		}
		va_end(ap);
		m_instance = new DGL(argc, argv);
		free(argv);
	}
}

void DGL::start()
{
	m_instance->preConfig();
	m_instance->config();
	m_instance->postConfig();
	m_instance->frame();
}

DGL* DGL::getApp()
{
	return m_instance;
}

void DGL::setData(void* data)
{
	m_data = data;
}

void* DGL::getData()
{
	return m_data;
}

int DGL::addAugment(dglAugment* newAugment)
{
	for (unsigned int i=0; i< m_augments.size(); i++)
	{
		if (newAugment == m_augments[i])
		{
			return -1;
		}
	}
	m_augments.push_back(newAugment);
	return 0;
}
int DGL::addPreNavAugment(dglAugment* newAugment)
{
	for (unsigned int i=0; i< m_prenav.size(); i++)
	{
		if (newAugment == m_prenav[i])
		{
			return -1;
		}
	}
	m_prenav.push_back(newAugment);
	return 0;
}
int DGL::removePreNavAugment(dglAugment* newAugment)
{
	for (unsigned int i=0; i< m_prenav.size(); i++)
	{
		if (newAugment == m_prenav[i])
		{
			m_prenav.erase(m_prenav.begin()+i);
			return 0;
		}
	}
	return -1;
}

int DGL::removeAugment(dglAugment* newAugment)
{
	for (unsigned int i=0; i< m_augments.size(); i++)
	{
		if (newAugment == m_augments[i])
		{
			m_augments.erase(m_augments.begin()+i);
			return 0;
		}
	}
	return -1;
}

int DGL::addOverlay(dglAugment* newAugment)
{
	for (unsigned int i=0; i< m_overlays.size(); i++)
	{
		if (newAugment == m_overlays[i])
		{
			return -1;
		}
	}
	m_overlays.push_back(newAugment);
	return 0;
}
int DGL::removeOverlay(dglAugment* newAugment)
{
	
	for (unsigned int i=0; i< m_overlays.size(); i++)
	{
		if (newAugment == m_overlays[i])
		{
			m_overlays.erase(m_overlays.begin()+i);
			return 0;
		}
	}
	return -1;
}
int DGL::preConfig()
{
	for (unsigned int i=0; i< m_prenav.size(); i++)
		m_prenav[i]->preConfig();
	for (unsigned int i=0; i< m_augments.size(); i++)
		m_augments[i]->preConfig();
	for (unsigned int i=0; i< m_overlays.size(); i++)
		m_overlays[i]->preConfig();
	if (m_preconfig != NULL)
		(m_preconfig)();

	int Return = dtkManager::preConfig();
	return Return;
	return 0;
}

int DGL::config()
{
	int Return = dtkManager::config();
	return Return;
}

int DGL::postConfig()
{

	for (unsigned int i=0; i< m_prenav.size(); i++)
		m_prenav[i]->postConfig();
	for (unsigned int i=0; i< m_augments.size(); i++)
		m_augments[i]->postConfig();
	for (unsigned int i=0; i< m_overlays.size(); i++)
		m_overlays[i]->postConfig();
	if (m_postconfig != NULL)
		(m_postconfig)();
	return dtkManager::postConfig();
}

int DGL::preFrame()
{
        DGL::lock("CALLBACK_LOCK");
	for (unsigned int i=0; i< m_prenav.size(); i++)
		m_prenav[i]->preFrame();
	for (unsigned int i=0; i< m_augments.size(); i++)
		m_augments[i]->preFrame();
	for (unsigned int i=0; i< m_overlays.size(); i++)
		m_overlays[i]->preFrame();
	if (m_preframe != NULL)
		(m_preframe)();
	int Return = dtkManager::preFrame(__DTKMANAGER_PREFRAME_MAGIC_);
        DGL::unlock("CALLBACK_LOCK");
	return Return;
}

int DGL::preNavDraw()
{
	for (unsigned int i=0; i< m_prenav.size(); i++)
		m_prenav[i]->draw();
	if (m_prenavcb != NULL)
		(m_prenavcb)();
	return 0;
}

int DGL::draw()
{
	//Draw the prenav aguments
	for (unsigned int i=0; i< m_prenav.size(); i++)
	{
		if (m_prenav[i]->isActive())
			m_prenav[i]->draw();
	}

	if (m_prenavcb != NULL)
		(m_prenavcb)();
	
	//Navigate
	dtkNav* nav = NULL;
	if  (m_navlist)
		nav = m_navlist->current();
	if (nav != NULL)
	{
		glTranslatef(nav->location.x, nav->location.y, nav->location.z);
		glRotatef(nav->location.h, 0.0f, 0.0f, 1.0f);
		glRotatef(nav->location.p, 1.0f, 0.0f, 0.0f);
		glRotatef(nav->location.r, 0.0f,1.0f,0.0f);
	}

	//Draw regular aguments
	for (unsigned int i=0; i< m_augments.size(); i++)
	{
		if (m_augments[i]->isActive())
		{
			m_augments[i]->draw();
		}
	}

	//Draw the C callback function
	if (m_display !=NULL)
		(m_display)();

	//Call the overlays
	for (unsigned int i=0; i< m_overlays.size(); i++)
	{
		if (m_overlays[i]->isActive())
		{
			m_overlays[i]->draw();
		}
	}

	//Call the overlay callback function
	if (m_overlaycb !=NULL)
		(m_overlaycb)();
	return DTK_CONTINUE;
}
int DGL::frame()
{
	int Return =0; 
	Return = preFrame();
	if (Return != 0)
	{
		return Return;
	}
	Return = dtkManager::frame();
	if (Return != 0)
	{
		return Return;
	}
	Return = postFrame();
	return Return;
}

void DGL::run()
{
	while(state & DTK_ISRUNNING)
	{
		m_instance->frame();	
	}
}

int DGL::postFrame()
{
        DGL::lock("CALLBACK_LOCK");
	for (unsigned int i=0; i< m_prenav.size(); i++)
		m_prenav[i]->postFrame();
	for (unsigned int i=0; i< m_augments.size(); i++)
		m_augments[i]->postFrame();
	for (unsigned int i=0; i< m_overlays.size(); i++)
		m_overlays[i]->postFrame();
	if (m_postframe != NULL)
		(m_postframe)();
	int Return = dtkManager::postFrame(__DTKMANAGER_POSTFRAME_MAGIC_);
        DGL::unlock("CALLBACK_LOCK");
	return Return;
}
int DGL::initGL()
{
	for (unsigned int i=0; i< m_prenav.size(); i++)
		m_prenav[i]->initGL();
	for (unsigned int i=0; i< m_augments.size(); i++)
		m_augments[i]->initGL();
	for (unsigned int i=0; i< m_overlays.size(); i++)
		m_overlays[i]->initGL();
	if (m_initglcb != NULL)
		(m_initglcb)();
	return 0;
}
int DGL::setupGL()
{
	for (unsigned int i=0; i< m_prenav.size(); i++)
		m_prenav[i]->setupGL();
	for (unsigned int i=0; i< m_augments.size(); i++)
		m_augments[i]->setupGL();
	for (unsigned int i=0; i< m_overlays.size(); i++)
		m_overlays[i]->setupGL();
	if (m_setupglcb != NULL)
		(m_setupglcb)();
	return 0;
}

vector<dglAugment*> DGL::getStandardAugments()
{
	return m_augments;
}
vector<dglAugment*> DGL::getOverlayAugments()
{
	return m_overlays;
}
vector<dglAugment*> DGL::getPreNavAugments()
{
	return m_prenav;
}

int DGL::setHeadTracking(bool value)
{
	m_trackhead = value;
	return 0;
}

bool DGL::getHeadTracking()
{
	return m_trackhead;
}

int DGL::setDiverseCoord(bool value)
{
	m_diversecoord = value;
	return 0;
}

bool DGL::getDiverseCoord()
{
	return m_diversecoord;
}

float DGL::getScale()
{
	return m_scale;
}

int DGL::setScale(float scale)
{
	m_scale = scale;
	return 0;
}

dtkCoord DGL::getOrigin()
{
	return m_origin;
}

int DGL::setOrigin(dtkCoord org)
{
	m_origin = org;
	return 0;
}

float DGL::getMeters()
{
	return m_meters;
}

int DGL::setMeters(float meters)
{
	m_meters = meters;
	return 0;
}

int DGL::setInitGL(bool value)
{
	m_doinitgl = value;
	return 0;
}

bool DGL::getInitGL()
{
	return m_doinitgl;

}

int DGL::setSetupGL(bool value)
{
	m_dosetupgl = value;
	return 0;
}

bool DGL::getSetupGL()
{
	return m_dosetupgl;
}

bool DGL::getDiscreteStereo()
{
	return m_discreteStereo;
}

int DGL::setDiscreteStereoLeft()
{
	m_display = m_display_discrete_left;
	return 0;
}

int DGL::setDiscreteStereoRight()
{
	m_display = m_display_discrete_right;
	return 0;
}

#if 0
//Contributed by John Kelso
int DGL::dglToWorld(dtkCoord& coord)
{
  printf("%s %d\n",__FILE__,__LINE__) ;
	dtkMatrix world(coord);
	dtkMatrix e = ether() ;
	dtkMatrix e_inv ;
	e_inv.copy(&e) ;
	e_inv.invert() ;
	world.mult(&e);
	dtkNav* current;
	current = m_navlist->current();
	if (current)
	{
		dtkMatrix* temp = m_navlist->current()->matrix();
		dtkMatrix navMatrixInv = * temp;
		navMatrixInv.invert();
		world.mult(&navMatrixInv);
	}
	world.mult(&e_inv);
	world.coord(&coord);
	return 0;	
}

int DGL::worldToDgl(dtkCoord& coord)
{
  printf("%s %d\n",__FILE__,__LINE__) ;
	dtkMatrix world(coord);
	dtkMatrix e = ether() ;
	world.mult(&e);
	dtkNav* current;
	current = m_navlist->current();
	if (current)
	{
		dtkMatrix* temp = m_navlist->current()->matrix();
		dtkMatrix navMatrix = * temp;
		world.mult(&navMatrix);
	}
	world.coord(&coord);
	return 0;	
}

dtkMatrix DGL::ether(void)
{
  dtkMatrix mat ;
  mat.scale(m_meters/m_scale);
  mat.coord(m_origin);
  mat.invert() ;
  return mat ;
}
#endif

int DGL::createLock(const string& name)
{
	//Check for duplicates
	for (unsigned int i=0; i< m_lockNames.size(); i++)
	{
		if (m_lockNames[i] == name)
		{
			return -1;
		}
	}
	
	//Create the lock
	OpenThreads::Mutex* temp = new OpenThreads::Mutex;
	m_locks.push_back(temp);
	m_lockNames.push_back(name);
	return 0;

}
int DGL::deleteLock(const string& name)
{
	//Check for duplicates
	for (unsigned int i=0; i< m_lockNames.size(); i++)
	{
		if (m_lockNames[i] == name)
		{
			delete m_locks[i];
			m_lockNames.erase(m_lockNames.begin() + i);
			m_locks.erase(m_locks.begin() + i);
			return 0;
		}
	}
	return -1;
}
int DGL::unlock(const string& name)
{
	for (unsigned int i=0; i< m_lockNames.size(); i++)
	{
		if (m_lockNames[i] == name)
		{
			int Return = m_locks[i]->unlock();
			return 0;
		}
	}
	return -1;
}
int DGL::lock(const string& name)
{
	for (unsigned int i=0; i< m_lockNames.size(); i++)
	{
		if (m_lockNames[i] == name)
		{
			int Return = m_locks[i]->lock();
			return 0;
		}
	}
	return -1;
}

int DGL::setShutdownOnEscape(bool value)
{
  m_shutdownonescape = value ;
  return 0;
}

bool DGL::getShutdownOnEscape()
{
  return m_shutdownonescape ;

}

bool DGL::getMultiThreaded()
{
	return m_multiThreaded;
}
int DGL::setMultiThreaded(bool val)
{
	m_multiThreaded = val;
	return 0;
}
int DGL::setClearColor(const float& r, const float& g, const float& b, const float& a)
{
	m_clearColor[0] = r;
	m_clearColor[1] = g;
	m_clearColor[2] = b;
	m_clearColor[3] = a;
	return 0;
}
int DGL::getClearColor(float& r, float& g, float& b, float& a)
{
	r = m_clearColor[0];
	g = m_clearColor[1];
	b = m_clearColor[2];
	a = m_clearColor[3];
	return 0;
}

// we get these from dtkManager, but do a check before we do a load,
// and only load it not already loaded
// this way group DSOs won't trip over themselves by trying to load the same DSO multiple times
#ifdef DTK_ARCH_WIN32_VCPP
# define LIST_SEPERATOR  ";"
#else
# define LIST_SEPERATOR  ":"
#endif

int DGL::load(const char *file, void *arg)
{
  if (!file)
    return 0 ;
  // break into bite sized pieces
  std::string all = file ;

  std::string::size_type idx = all.find(LIST_SEPERATOR) ;
  if (idx == std::string::npos)
    {
      int ret = m_load(all.c_str(), arg) ;
      if (ret)
	return ret ;
    }
  else
    {
      int ret = m_load(all.substr(0,idx).c_str(), arg) ;
      if (ret)
	return ret ;
      load(all.substr(idx+1).c_str(), arg) ;
    }
  
  return 0 ;
}

int DGL::m_load(const char *file, void *arg)
{
  #ifdef __APPLE__
    #ifdef check
        #undef check
    #endif 
  #endif
  if (check(file))
    {
      dtkMsg.add(DTKMSG_INFO,"DGL::load: DSO \"%s%s%s\" %salready loaded%s, skipping.\n",dtkMsg.color.grn,file,dtkMsg.color.end,dtkMsg.color.red,dtkMsg.color.end);
      return 0 ;
    }
  else
    {
      return (dtkManager::load(file, arg)) ;
    }
}
