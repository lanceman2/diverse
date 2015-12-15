#ifndef _dgl_cuda_config_h_
#define _dgl_cuda_config_h_

#include <dgl/config.h>
#include <unistd.h>
#include <stdio.h>

class DGLCudaConfig
{
public:
	static int getDeviceFromDisplay( std::string& device_string, std::string display_string )
	{
		init();
		std::map<std::string, std::string>::iterator itr = m_display_device_map.find( display_string );
		if( itr == m_display_device_map.end() )
		{
			return -1;
		}
		device_string = itr->second;

		return 0;
	}

	static int getDeviceFromDisplay( std::string& device_string, int display, int screen )
	{
		char display_string[256];
		sprintf( display_string, ":%d.%d", display, screen );
		return getDeviceFromDisplay( device_string, display_string );
	}

	static int getDeviceFromDisplay( int& pci_bus_id, int& pci_device_id, int display, int screen )
	{
		std::string device_string;
		int error;
		if( ( error = getDeviceFromDisplay( device_string, display, screen ) ) )
		{
			return error;
		}
printf( "device_string: \"%s\"\n", device_string.c_str() );
		sscanf( device_string.c_str(), "%d:%d.0", &pci_bus_id, &pci_device_id );
printf( "pci_bus_id: %d    pci_device_id: %d\n", pci_bus_id, pci_device_id );

		return 0;
	}

	static int getDisplayFromDevice( std::string& display_string, std::string device_string )
	{
		init();
		std::map<std::string, std::string>::iterator itr = m_device_display_map.find( device_string );
		if( itr == m_device_display_map.end() )
		{
			return -1;
		}
		display_string = itr->second;

		return 0;
	}

	static int getDisplayFromDevice( std::string& display_string, int device_pci_bus, int device_pci_device )
	{
		char device_string[256];
		sprintf( device_string, "%d:%d:0", device_pci_bus, device_pci_device );
		return getDisplayFromDevice( display_string, device_string );
	}

	static void setConfigFile( const std::string& file_name )
	{
		m_file_name = file_name;
	}

	static void reload( bool reset_file = false )
	{
		m_inited = false;
		if( reset_file )
			m_file_name.clear();
		init();
	}

	static void init()
	{
		if( m_inited )
			return;

		std::string cmd = "hostname --fqdn";
		std::string file_name;
		std::string file_path = DGL_DEFAULT_ROOT"/etc/dgl/";

		char* dgl_cuda_conf_env_var = getenv( "DGL_CUDA_CONF" );
		if( dgl_cuda_conf_env_var )
		{
			if( getConfig( dgl_cuda_conf_env_var ) )
			{
				printf( "ERROR: environment variable DGL_CUDA_CONF=\"%s\" failed\n", dgl_cuda_conf_env_var );
				printf( "WARNING: Trying default DGL cudaconfig files.\n" );
			}
			else
			{
				m_file_name = dgl_cuda_conf_env_var;
			}
		}
		if( !m_file_name.size() )
		{
			if( getHostnameOutput( file_name, cmd ) )
			{
				printf( "WARNING: Will try to find config files for \"localhost\"\n" );
				file_name = "localhost";
			}
			else if( file_name == "localhost" )
			{
				cmd = "hostname";
				if( getHostnameOutput( file_name, cmd ) )
				{
					printf( "WARNING: Will try to find config files for \"localhost\"\n" );
					file_name = "localhost";
				}
			}

			char cwd[1024];
			getcwd( cwd, 1024 );

			m_file_name = std::string( cwd ) + std::string( "/" ) + file_name + std::string( ".dglcudaconf" );
			if( getConfig( m_file_name ) == -1 )
			{
				m_file_name = file_path + file_name + std::string( ".dglcudaconf" );
				if( getConfig( m_file_name ) == -1 && file_name != "localhost" )
				{
					cmd = "hostname --short";
					if( getHostnameOutput( file_name, cmd ) )
					{
						printf( "WARNING: Will try to find config files for \"localhost\"\n" );
						file_name = "localhost";
					}
					else
					{
						m_file_name = std::string( cwd ) + std::string( "/" ) + file_name + std::string( ".dglcudaconf" );
						if( getConfig( m_file_name ) == -1 )
						{
							m_file_name = file_path + file_name + std::string( ".dglcudaconf" );
							if( getConfig( m_file_name ) == -1 && file_name != "localhost" )
							{
								file_name = "localhost";
							}
							else
							{
								printf( "WARNING: Unable to find dglcudaconf file\n" );
								return;
							}
							m_file_name = std::string( cwd ) + std::string( "/" ) + file_name + std::string( ".dglcudaconf" );
							if( getConfig( m_file_name ) == -1 )
							{
								m_file_name = file_path + file_name + std::string( ".dglcudaconf" );
								if( getConfig( m_file_name ) == -1 )
								{
									printf( "WARNING: Unable to find dglcudaconf file\n" );
									return;
								}
							}
						}
					}
				}
			}
		}

		m_inited = true;
	}

private:
	DGLCudaConfig()
	{
	}

	static int getHostnameOutput( std::string& output, std::string& cmd )
	{
		FILE* cmd_fd = popen( cmd.c_str(), "r" );
		if( !cmd_fd )
		{
			printf( "ERROR: Unable to popen( \"%s\", \"r\" )\n", cmd.c_str() );
			return -1;
		}
		char cmd_output[256];
		if( !fgets( cmd_output, sizeof(cmd_output), cmd_fd ) )
		{
			printf( "ERROR: Unable to get results for \"%s\"\n", cmd.c_str() );
			return -2;
		}
		pclose( cmd_fd );
		std::string str = cmd_output;
		size_t startpos = str.find_first_not_of( "\n", 0 );
		size_t endpos = str.find_first_of( "\n", startpos );
		output = str.substr( startpos, endpos - startpos);

		return 0;
	}

	static void tokenize( const std::string& str, std::deque<std::string>& tokens, const std::string& delimiters )
	{
		size_t startpos = str.find_first_not_of( delimiters, 0 );
		size_t endpos = str.find_first_of( delimiters, startpos );
		while( startpos != std::string::npos )
		{
			tokens.push_back( str.substr( startpos, endpos - startpos) );
			startpos = str.find_first_not_of( delimiters, endpos );
			endpos = str.find_first_of( delimiters, startpos );
			if( startpos != std::string::npos && endpos == std::string::npos )
				endpos = str.size();
		}
	}

	static int parseConfig( std::string file_contents )
	{
		std::deque<std::string> lines;
		std::deque<std::string> tokens;
		std::string display;
		std::string device;
		tokenize( file_contents, lines, "\n" );
		for( int i = 0; i < lines.size(); i++ )
		{
printf( "line %d: \"%s\"\n", i, lines[i].c_str() );
			tokenize( lines[i], tokens, " \t\n\v\f\r" );
			// Skip comments
			if( tokens[0] == "#" )
			{
				continue;
			}
			if( tokens.size() != 4 )
			{
/*				if( tokens.size() == 1 && tokens[0] == "" )
				{
					continue;
				}*/
				printf( "ERROR: Expected line formatted similarly to \"display :0.0 pci 1:0.0\"\n" );
				printf( "Line is \"%s\"\n", lines[i].c_str() );
				return -10;
			}
			if( tokens[0] == "display" )
			{
				display = tokens[1];
			}
			if( tokens[2] == "pci" )
			{
				device = tokens[3];
			}
			m_display_device_map[display] = device;
			m_device_display_map[device] = display;
		}

		return 0;
	}

	static int getConfig( const std::string& file_name )
	{
		std::string file_contents;
		long buffer_file_length;
		char* buffer_file = NULL;
		int file_error = 0;
		FILE* file_stream = fopen( file_name.c_str(), "r" );
		if( !file_stream )
		{
			printf( "ERROR - unable to open file %s for reading\n", file_name.c_str() );
			file_error = -1;
		}
		else if( fseek( file_stream, 0, SEEK_END ) )
		{
			printf( "ERROR - unable to determine the size of the file %s\n", file_name.c_str() );
			file_error = -2;
		}
		else
		{
			buffer_file_length = ftell( file_stream );
	
			if( fseek( file_stream, 0, SEEK_SET ) )
			{
				printf( "ERROR - unable to determine the size of the file %s\n", file_name.c_str() );
				file_error = -3;
			}
			else
			{
				buffer_file = new char[ buffer_file_length + 2 ];
				if( !buffer_file )
				{
					printf( "ERROR: Unable to allocate memory buffer of size %l\n", buffer_file_length );
					file_error = -4;
				}
				buffer_file[ buffer_file_length ] = '\n';
				buffer_file[ buffer_file_length + 1 ] = '\0';
	
				long buffer_size = fread( buffer_file, 1, buffer_file_length, file_stream );
				if( buffer_size != buffer_file_length )
				{
					printf( "ERROR: Unable to read file %s\n", file_name.c_str() );
					file_error = -5;
				}
			}
		}
	
		if( file_error < -4 )
		{
				delete buffer_file;
		}

		if( file_error < -1 )
		{
			fclose( file_stream );
		}

		if( file_error < 0 )
			return file_error;

printf( "INFO: Loaded \"%s\"\n", file_name.c_str() );
		file_contents = buffer_file;

		delete buffer_file;

		return parseConfig( file_contents );
	}

	static bool m_inited;
	static std::string m_file_name;
	static std::map<std::string, std::string> m_display_device_map;
	static std::map<std::string, std::string> m_device_display_map;
};

bool DGLCudaConfig::m_inited = false;
std::string DGLCudaConfig::m_file_name;
std::map<std::string, std::string> DGLCudaConfig::m_display_device_map;
std::map<std::string, std::string> DGLCudaConfig::m_device_display_map;

#endif

