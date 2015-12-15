/************************************************************************
* dtk-masterServer
*
* daemon to start, stop, and reset other DTK and DADS daemons.
*
* by Patrick Shinpaugh 9/15/2004
************************************************************************/

//#include "config.h"
//#include <fstream>
//#include <vector>
//#include <map>
//#include <netdb.h>
#include <dtk.h>
#include <string>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fstream>

using namespace std;

dtkSharedMem* headShm;
dtkSharedMem* wandShm;
dtkSharedMem* joystickShm;
dtkSharedMem* buttonShm;

dtkVRCOSharedMem VRCO;

float headdata[6];
float wanddata[6];
float joystickdata[2];
unsigned char buttondata;
float scale_factor = 1.0f;
string dtk_shm_name_head = "head";
string dtk_shm_name_wand = "wand";
string dtk_shm_name_joystick = "joystick";
string dtk_shm_name_buttons = "buttons";
string config_file;

void sigchild_handler( int s )
{
    while( wait( NULL ) > 0 )
        ;
}

void usage( int code )
{
	printf( "Usage: dtk-VRCOServer [ options ]\n\n" );
	printf( "-h|--help          Prints out this usage information.\n" );
	printf( "-c|--config file   This is used to specify a configuration file. The\n"
			"                   default location is $INSTALL_PREFIX/etc/dads\n" );
	printf( "Configuration Options:\n" );
	printf( "scale factor       The scale option specifies the conversion factor by\n"
			"                   which the the position(s) of the tracker (head) and/or\n"
			"                   controller (wand) will be multiplied.\n" );
	printf( "dtkShm_head head   Specifies the name of the dtkSharedMem file to be used\n"
			"                   for the VRCO shm.\n" );
	printf( "dtkShm_wand wand joystick buttons\n"
			"                   Specifies the names of the dtkSharedMem wand, joystick,\n"
			"                   and buttons files to be used for the VRCO shm.\n" );
	exit( code );
}

int serverLoop()
{
    headShm = new dtkSharedMem( sizeof(float)*6, dtk_shm_name_head.c_str() );
    wandShm = new dtkSharedMem( sizeof(float)*6, dtk_shm_name_wand.c_str() );
    joystickShm = new dtkSharedMem( sizeof(float)*2, dtk_shm_name_joystick.c_str() );
    buttonShm = new dtkSharedMem( sizeof(char)*1, dtk_shm_name_buttons.c_str() );

    memset( headdata, sizeof(float) * 6, 0 );
    memset( wanddata, sizeof(float) * 6, 0 );
    memset( joystickdata, sizeof(float) * 2, 0 );
    memset( &buttondata, sizeof(unsigned char), 0 );

    if (VRCO.isInvalid())
    {
        fprintf(stderr, "ERROR: Bad VRCO shared memory\n");
        return -1;
    }

    while( 1 )
    {
        headShm->read( headdata );
        wandShm->read( wanddata );
        joystickShm->read( joystickdata );
        buttonShm->read( &buttondata );

		headdata[0] *= scale_factor;
		headdata[1] *= scale_factor;
		headdata[2] *= scale_factor;
		headdata[2] -= 1.0f;
		headdata[5] += 90.0f;

		wanddata[0] *= scale_factor;
		wanddata[1] *= scale_factor;
		wanddata[2] *= scale_factor;
		wanddata[2] -= 1.0f;

        VRCO.writeHeadTracker( (const float*)headdata );
        VRCO.writeWand( (const float*)wanddata, (const float*)joystickdata, buttondata );
        usleep( 100 );
    }
    return 0;
}

void interpretArgs( int argc, char** argv )
{
	vector<string> args;
	for( int i=1;i<argc;i++ )
	{
		args.push_back( string( argv[i] ) );
	}
	for( int i=0;i<args.size();i++ )
	{
		if( args[i] == "-h" || args[i] == "--help" )
			usage( 0 );
		else if( args[i] == "-c" || args[i] == "--config" )
		{
			i++;
			if( i < args.size() )
				config_file = args[i];
			else
			{
				printf( "The -c|--config option requires the name of a configuration file\n" );
				usage( -1 );
			}
		}
		else
			usage( -1 );
	}
}

int readConfig()
{
	ifstream file( config_file.c_str(), ifstream::in );
	if( !file.is_open() )
	{
		printf( "ERROR - unable to open config file %s\n", config_file.c_str() );
		return -1;
	}

	char temp[1024];
	vector<string> config;
	while( !file.eof() && file.good() )
	{
		file.getline( temp, 1024 );
		config.push_back( string( temp ) );
	}
	file.close();

	bool has_error = false;
	for( int i=0;i<config.size();i++ )
	{
		vector<string> line;
		tokenize( config[i], line );
		if( !line.size() )
			continue;
		if( line[0][0] == '#' )
		{
			continue;
		}
		else if( line[0] == "scale" )
		{
			float value;
			if( line.size() == 1 )
			{
				has_error = true;
				printf( "ERROR - the scale option requires a numerical scale factor\n"
						"		as its parameter\n" );
			}
			else if( line.size() > 2 )
			{
				has_error = true;
				printf( "ERROR - too many parameters passed to the scale option.\n" );
			}
			else if( convertStringToNumber( value, line[1].c_str() ) )
			{
				has_error = true;
				printf( "ERROR - unable to convert %s to a numerical value.\n",
						line[1].c_str() );
			}
			else
			{
				scale_factor = value;
			}
		}
		else if( line[0] == "dtkShm_head" )
		{
			if( line.size() != 2)
			{
				has_error = true;
				printf( "ERROR - the dtkShm_head option requires a dtkSharedMem file\n"
						"        name as its only parameter\n" );
			}
			else
			{
				dtk_shm_name_head = line[1];
			}
		}
		else if( line[0] == "dtkShm_wand" )
		{
			if( line.size() != 4)
			{
				has_error = true;
				printf( "ERROR - the dtkShm_head option requires 3 dtkSharedMem file\n"
						"        namea as its parameters representing the wand position,\n"
						"        joystick, and buttons\n" );
			}
			else
			{
				dtk_shm_name_wand = line[1];
				dtk_shm_name_joystick = line[2];
				dtk_shm_name_buttons = line[3];
			}
		}
		else
		{
			printf( "ERROR - invalid configuration option \"%s\"\n",
					config[i].c_str() );
		}
	}
	if( has_error )
		return -1;

	return 0;
}

int main( int argc, char** argv )
{
	interpretArgs( argc, argv );
	if( config_file.size() )
	{
		if( readConfig() )
		{
			return -1;
		}
	}

	int pid = fork();
	if( pid < 0 )
	{
		printf( "ERROR: Unable to fork process - exiting\n" );
		exit( -1 );
	}
	else if( pid > 0 )
		exit( 0 );
	if( setsid() == -1 )
		printf( "ERROR: Unable to become session leader\n" );

	umask( 0 );
	chdir( "/" );
	struct sigaction sa;
	sa.sa_handler = sigchild_handler;
#ifdef DADS_ARCH_IRIX
	sigemptyset( &sa.sa_mask );
	sigaddset( &sa.sa_mask, SA_NOCLDWAIT );
#else
	sigemptyset( &sa.sa_mask );
#endif
	sa.sa_flags = SA_RESTART;
	if( sigaction( SIGCHLD, &sa, NULL ) == -1 )
	{
		perror( "sigaction" );
		exit(1);
	}
	serverLoop();
}

