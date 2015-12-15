#ifndef _dgl_cuda_manager_h_
#define _dgl_cuda_manager_h_

#include <dgl/DGL-header.h>
#include <dgl/DGLCudaDevice.h>
#include <dgl/DGLCudaConfig.h>

class DGLAPI DGLCudaManager
{
public:
	/*!
	 * @brief Set the display number of this pipe
	 *
	 */

	/*!
	 * @brief Initialize the DGLCudaManager by retrieving device info
	 *
	 */
	static void init()
	{
		if( !m_inited )
		{
			int num;
			if( cudaGetDeviceCount( &num ) )
			{
				printf( "ERROR: Unable to get CUDA device count - %s\n", cudaGetErrorString( cudaGetLastError() ) );
			}
			else
			{
				for( int i = 0; i < num; i++ )
				{
					DGLCudaDevice* device = new DGLCudaDevice( i );
					if( !device )
					{
						printf( "ERROR: Unable to create DGLCudaDevice for device ID %d\n", i );
						continue;
					}
					m_devices.push_back( device );
				}
			}
		}
	}

	/*!
	 * @brief Get the number of CUDA devices available
	 *
	 * @return the number of CUDA devices available
	 */
	static int getDeviceCount()
	{
		init();
		return m_devices.size();
	}

	/*!
	 * @brief Get the number of CUDA devices available
	 *
	 * @return the number of CUDA devices available
	 */
	static DGLCudaDevice* getDevice( int device_id )
	{
		init();
		return m_devices[device_id];
	}

	/*!
	 * @brief Get the number of CUDA devices available
	 *
	 * @return the number of CUDA devices available
	 */
	static cudaError_t setDevice( int device_id )
	{
		init();
		cudaError_t error = cudaSetDevice( device_id );
		if( error )
		{
			printf( "ERROR: cudaSetDevice( %d ) failed with error %d - \"%s\"\n", device_id, cudaGetLastError(), cudaGetErrorString( cudaGetLastError() ) );
		}
		return error;
	}

	/*!
	 * @brief Get the number of CUDA devices available
	 *
	 * @return the number of CUDA devices available
	 */
	static cudaError_t setGLDevice( int device_id )
	{
		init();
		cudaError_t error = cudaGLSetGLDevice( device_id );
		if( error )
		{
			printf( "ERROR: cudaGLSetGLDevice( %d ) failed with error %d - \"%s\"\n", device_id, cudaGetLastError(), cudaGetErrorString( cudaGetLastError() ) );
		}
		return error;
	}

	/*!
	 * @brief Get the number of CUDA devices available
	 *
	 * @return the number of CUDA devices available
	 */
	static DGLCameraThread* getCurrentCameraThread()
	{
		int cur_thread = OpenThreads::Thread::CurrentThread()->getThreadId();
		std::vector<DGLCameraThread*> camera_threads = DGL::display()->getCameraThreads();
		DGLCameraThread* cur_camera_thread = NULL;
		bool found = false;
		for( unsigned int i = 0; i < camera_threads.size(); i++ )
		{
			cur_camera_thread = camera_threads[i];
			if( cur_camera_thread->getThreadId() == cur_thread )
			{
				found = true;
				break;
			}
		}
		if( !found )
		{
			printf( "ERROR: Unable to find DGLCameraThread matching current Thread\n" );
			return NULL;
		}

		return cur_camera_thread;
}

	static DGLWindow* getCurrentWindow()
	{
		DGLCameraThread* camera_thread = getCurrentCameraThread();
		if( !camera_thread )
			return NULL;

		DGLWindow* window = camera_thread->getWindow();
		if( !window )
		{
			printf( "ERROR: Unable to find DLGWindow for current DGLCameraThread\n" );
			return NULL;
		}

		return window;
	}

	static DGLPipe* getCurrentPipe()
	{
		DGLWindow* window = getCurrentWindow();
		if( !window )
			return NULL;

		DGLPipe* pipe = window->getPipe();
		if( !pipe )
		{
			printf( "ERROR: Unable to find DGLPipe for current DGLWindow\n" );
			return NULL;
		}

		return pipe;
	}

	static int getMatchedGLDevice( unsigned int& display_id, unsigned int& screen_id, int& bus_id, int& device_id )
	{
		bool error = false;
		display_id = screen_id = bus_id = device_id = -1;
		init();
		DGLPipe* pipe = getCurrentPipe();
		if( !pipe )
			return -1;
printf( "pipe: %p\n", pipe );
		display_id = pipe->getDisplayNum();
		screen_id = pipe->getScreenNum();
printf( "display: %d    screen: %d\n", display_id, screen_id );

		std::string device_string;

		if( DGLCudaConfig::getDeviceFromDisplay( bus_id, device_id, display_id, screen_id ) )
		{
			printf( "WARNING: Unable to get device for display from config\n" );
		}
		if( bus_id == -1 && device_id == -1 )
		{
			bus_id = pipe->getPCIBusID();
			device_id = pipe->getPCIDeviceID();
		}

		if( display_id == -1 )
		{
			error = true;
			printf( "ERROR: DGLCudaManager::getMatchedGLDevice is unable to determine current X11 display\n" );
		}
		if( screen_id == -1 )
		{
			error = true;
			printf( "ERROR: DGLCudaManager::getMatchedGLDevice is unable to determine current X11 screen\n" );
		}
		if( bus_id == -1 )
		{
			error = true;
			printf( "ERROR: DGLCudaManager::getMatchedGLDevice is unable to determine current PCI bus ID\n" );
		}
		if( device_id == -1 )
		{
			error = true;
			printf( "ERROR: DGLCudaManager::getMatchedGLDevice is unable to determine current PCI device ID\n" );
		}

		if( error )
			return -1;

		return 0;
	}

private:
	/*!
	 * Private constructor to prevent instantiation
	 *
	 */
	DGLCudaManager()
	{
	};

	// tidy up!
	virtual ~DGLCudaManager()
	{
		for( int i = 0; i < m_devices.size(); i++ )
		{
			delete m_devices[i];
			
		}
		m_devices.clear();
	};

	static bool m_inited;
	static std::vector<DGLCudaDevice*> m_devices;
	static DGLCudaConfig* m_config;
};

bool DGLCudaManager::m_inited = false;
std::vector<DGLCudaDevice*> DGLCudaManager::m_devices;
DGLCudaConfig* m_config = NULL;

#endif
