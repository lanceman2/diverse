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
// DSO to check and pass arguments to daemons running
// on client machines for the DADS cluster
// Written 11/26/03 
// By Patrick Shinpaugh
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <vector>
#include <string>

class dPerformance : public dtkAugment
{
public:
	dPerformance(dtkManager *app) ;
	~dPerformance(void) ;
	int preConfig(void);
	int postConfig(void);
	int preFrame(void);
	int postFrame(void);
	void die();

private:
	FILE* m_fpsfile;
	bool m_first;
	int m_curcount;
	int m_totalcount;
	dtkTime m_time;
	dtkTime m_inittime;
	dtkManager* m_app;
};

// create object and set description.
// be sure to validate it!
dPerformance::dPerformance(dtkManager *app) : dtkAugment("dPerformance")
{
	m_app = app;
	setDescription("Provide performance feedback using fps for DADS cluster");
	validate();
}

// destructor
dPerformance::~dPerformance(void) 
{
	char buffer[256];
	unsigned int outputlen = sprintf( buffer, "OVERALL STATISTICS:\n" );
	if ( outputlen < fwrite( buffer, 1, outputlen, m_fpsfile ) )
	{
		printf( "Error writing to file\n" );
		exit( -1 );
	}
	double delta = (double)m_inittime.delta();
	outputlen = sprintf( buffer, "%12d  %16.6f",
			m_totalcount, m_totalcount/delta );
	if ( outputlen < fwrite( buffer, 1, outputlen, m_fpsfile ) )
	{
		printf( "Error writing to file\n" );
		exit( -1 );
	}
	fputc( '\n', m_fpsfile );
	fclose( m_fpsfile );
}

void dPerformance::die()
{
	char buffer[256];
	unsigned int outputlen = sprintf( buffer, "OVERALL STATISTICS:\n" );
	if ( outputlen < fwrite( buffer, 1, outputlen, m_fpsfile ) )
	{
		printf( "Error writing to file\n" );
		exit( -1 );
	}
	double delta = (double)m_inittime.delta();
	outputlen = sprintf( buffer, "%12d  %16.6f",
			m_totalcount, m_totalcount/delta );
	if ( outputlen < fwrite( buffer, 1, outputlen, m_fpsfile ) )
	{
		printf( "Error writing to file\n" );
		exit( -1 );
	}
	fputc( '\n', m_fpsfile );
	fclose( m_fpsfile );
}

// called only once by dpf, after pfInit() and dpfDisplay:preConfig()
// but before pfConfig()
int dPerformance::preConfig(void) 
{
	timeval time;
	gettimeofday( &time, NULL );
	m_time.reset( &time );
	m_inittime.reset( &time );
	m_first = true;
	m_curcount = m_totalcount = 0;
	umask(0);
	m_fpsfile = fopen( "/tmp/fps.log", "w" );
	umask(022);
	if ( !m_fpsfile )
	{
		printf( "Unable to create fps log file\n" );
		exit( -1 );
	}
	return DTK_CONTINUE;
}

// called only once by dpf, after pfConfig() and dpfDisplay:postConfig()
// pfConfig()
int dPerformance::postConfig(void) 
{
	return DTK_CONTINUE;
}

// called every display loop, as a pfFrame callback, before pfDraw()
// this return value causes it to only be called once
int dPerformance::preFrame(void) 
{
	return DTK_CONTINUE;
}

// called every display loop, as a pfFrame callback, after pfDraw()
// this return value causes it to only be called once, and the object removed
int dPerformance::postFrame(void) 
{
	m_totalcount++;
	m_curcount++;
	if( m_curcount >= 60 )
	{
		char buffer[256];
		double delta = (double)m_time.delta();
		unsigned int outputlen = sprintf( buffer, "%12d  %16.6f",
				m_totalcount, m_curcount/delta );
		if ( outputlen < fwrite( buffer, 1, outputlen, m_fpsfile ) )
		{
			printf( "Error writing to file\n" );
			exit( -1 );
		}
		fputc( '\n', m_fpsfile );
		m_curcount = 0;
		fflush( m_fpsfile );
	}
	return DTK_CONTINUE ; 
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
  //dpf *app = static_cast<dpf *>(manager);
  return new dPerformance(manager);
}

static int dtkDSO_unloader(dtkAugment *augment)
{
  delete augment;
  return DTKDSO_UNLOAD_CONTINUE;
}
