/**************************************************************************
DIVERSE DADS v1.0
released: April 7th
The DIVERSE HOMEPAGE is at: diverse.sourceforge.net

Copyright (C) 2000 - 2004  Andrew A. Ray, Patrick Shinpaugh, and Ron Kriz

This is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (GPL) as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This software is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software, in the top level source directory in a file
named "COPYING.GPL"; if not, see it at:
http://www.gnu.org/copyleft/gpl.html or write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA.
**************************************************************************/
//Navigation reader DSO for the DADS cluster
//Written 3/2/2004
//By Patrick Shinpaugh
#include <stdio.h>
#include <dtk.h>
#include <unistd.h>
#include <stdlib.h>
#include <dtk/dtkDSO_loader.h>
#include <string>
#include <X11/Xlib.h>
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glxext.h>

using namespace std;

class nvswapbarrier : public dtkAugment
{
public:

	nvswapbarrier(dtkManager* m);
	~nvswapbarrier(void) ;
	int postConfig(void);

private:
	dtkManager*   m_manager;
	Display*      m_dpy;
	GLXDrawable   m_drawable;
	unsigned int  m_swapgroup;
	unsigned int  m_swapbarrier;
};

// create object and set description.
// be sure to validate it!
nvswapbarrier::nvswapbarrier(dtkManager *m):dtkAugment("nvswapbarrier")
{
	m_dpy = NULL;
	m_drawable = 0;
	m_swapgroup = 1;
	m_swapbarrier = 1;
	m_manager = m;

	setDescription("Swap barrier DSO for NVIDIA Quadro FX 3000G and DADS cluster");
	validate();
}

// destructor
nvswapbarrier::~nvswapbarrier(void) 
{
	if( m_drawable && m_dpy )
	{
		if( !glXQuerySwapGroupNV( m_dpy, m_drawable, &m_swapgroup, &m_swapbarrier ) )
			printf( "ERROR - unable to query swap group for this drawable!!!\n" );
		else if( !glXBindSwapBarrierNV( m_dpy, m_swapgroup, 0 ) )
			printf( "ERROR - could not unbind the swap group from the barrier!!!\n" );
		else if( !glXJoinSwapGroupNV( m_dpy, m_drawable, 0 ) )
			printf( "ERROR - could not remove drawable from swap group!!!\n" );
		else
			printf( "Swap group removed successfully!!!\n" );
	}
}

// called only once by dpf, after pfConfig() and dpfDisplay:postConfig()
// pfConfig()
int nvswapbarrier::postConfig(void) 
{
	m_dpy = XOpenDisplay( NULL );
	if( !m_dpy )
	{
		printf( "ERROR - Display not defined!!!\n" );
		return DTK_CONTINUE;
	}
	m_drawable = glXGetCurrentDrawable();
	if( !m_drawable )
	{
		printf( "ERROR - GLXDrawable does not exist!!!\n" );
		return DTK_CONTINUE;
	}
	unsigned int screen = 0, maxbarriers = 0, maxgroups = 0;
	const char* extensions = glXQueryExtensionsString( m_dpy, screen );
	const char* extloc = strstr( extensions, "GLX_NV_swap_group" );
	printf( "%s\n\n", extensions );
	if( extloc )
		printf( "%s\n", extloc );
	if( glXQueryMaxSwapGroupsNV( m_dpy, screen, &maxgroups, &maxbarriers ) )
		printf( "ERROR - Unable to query max swap groups!!!\n"
				"Swap sync NOT enabled\n" );
	printf( "Max groups: %d    Max barriers: %d\n\n", maxgroups, maxbarriers );
	if( maxgroups > 0 && maxbarriers > 0 )
	{
		if( glXJoinSwapGroupNV( m_dpy, m_drawable, m_swapgroup ) )
			printf( "ERROR - Unable to join drawable to swap group!!!\n" );
		else
		{
			if( glXBindSwapBarrierNV( m_dpy, m_swapgroup, m_swapbarrier ) )
				printf( "ERROR - Unable to bind swap group to barrier!!!\n" );
			else
				printf( "Bind to swap barrier successful!!!\n" );
		}
	}
	fflush( stdout );
	return DTK_CONTINUE;
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
	return new nvswapbarrier(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
	delete augment;
	return DTKDSO_UNLOAD_CONTINUE;
}
