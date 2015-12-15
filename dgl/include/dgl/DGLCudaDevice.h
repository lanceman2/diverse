#ifndef _dgl_cuda_device_h_
#define _dgl_cuda_device_h_

class DGLCudaDevice
{
public:
	DGLCudaDevice( int device_id ) : m_device_id( device_id ), m_valid( false )
	{
		if( cudaGetDeviceProperties( &m_properties, device_id ) )
		{
			printf( "ERROR: Unable to obtain properties for device ID %d\n", device_id );
		}
		else
			m_valid = true;
	}

	std::string getName()
	{
		return m_properties.name;
	};

	size_t getTotalGlobalMem()
	{
		return m_properties.totalGlobalMem;
	};

	size_t getSharedMemPerBlock()
	{
		return m_properties.sharedMemPerBlock;
	};

	int getRegsPerBlock()
	{
		return m_properties.regsPerBlock;
	};

	int getWarpSize()
	{
		return m_properties.warpSize;
	};

	size_t getMemPitch()
	{
		return m_properties.memPitch;
	};

	int getMaxThreadsPerBlock()
	{
		return m_properties.maxThreadsPerBlock;
	};

	int* getMaxThreadsDim()
	{
		return m_properties.maxThreadsDim;
	};

	int* getMaxGridSize()
	{
		return m_properties.maxGridSize;
	};

	size_t getTotalConstMem()
	{
		return m_properties.totalConstMem;
	};

	int getMajor()
	{
		return m_properties.major;
	};

	int getMinor()
	{
		return m_properties.minor;
	};

	int getClockRate()
	{
		return m_properties.clockRate;
	};

	size_t getTextureAlignment()
	{
		return m_properties.textureAlignment;
	};

	int getDeviceOverlap()
	{
		return m_properties.deviceOverlap;
	};

	int getMultiProcessorCount()
	{
		return m_properties.multiProcessorCount;
	};

	int getKernelExecTimeoutEnabled()
	{
		return m_properties.kernelExecTimeoutEnabled;
	};

	int getIntegrated()
	{
		return m_properties.integrated;
	};

	int getCanMapHostMemory()
	{
		return m_properties.canMapHostMemory;
	};

	int getComputeMode()
	{
		return m_properties.computeMode;
	};

	int getConcurrentKernels()
	{
		return m_properties.concurrentKernels;
	};

	int getECCEnabled()
	{
		return m_properties.ECCEnabled;
	};

	int getPCIBusID()
	{
		return m_properties.pciBusID;
	};

	int getPCIDeviceID()
	{
		return m_properties.pciDeviceID;
	};

	int getTCCDriver()
	{
		return m_properties.tccDriver;
	};

	void print()
	{
		printf( "CUDA Device ID: %d\n", m_device_id );
		printf( "Name: %s\n", getName().c_str() );
		printf( "Total Global Memory: %zu\n", getTotalGlobalMem() );
		printf( "Shared Memory Per Block: %zu\n", getSharedMemPerBlock() );
		printf( "Regs Per Block: %d\n", getRegsPerBlock() );
		printf( "Warp Size: %d\n", getWarpSize() );
		printf( "Memory Pitch: %zu\n", getMemPitch() );
		printf( "Max Threads Per Block: %d\n", getMaxThreadsPerBlock() );
		printf( "Max Threads Dimension - x: %d    y: %d    z: %d\n", getMaxThreadsDim()[0], getMaxThreadsDim()[1], getMaxThreadsDim()[2] );
		printf( "Max Grid Size - x: %d    y: %d    z: %d\n", getMaxGridSize()[0], getMaxGridSize()[1], getMaxGridSize()[2] );
		printf( "Total Const Memory: %zu\n", getTotalConstMem() );
		printf( "Major: %d\n", getMajor() );
		printf( "Minor: %d\n", getMinor() );
		printf( "Clock Rate: %d\n", getClockRate() );
		printf( "Texture Alignment: %zu\n", getTextureAlignment() );
		printf( "Device Overlap: %d\n", getDeviceOverlap() );
		printf( "Multi Processor Count: %d\n", getMultiProcessorCount() );
		printf( "Kernel Exec Timeout Enabled: %d\n", getKernelExecTimeoutEnabled() );
		printf( "Integrated: %d\n", getIntegrated() );
		printf( "Can Map Host Memory: %d\n", getCanMapHostMemory() );
		printf( "Compute Mode: %d\n", getComputeMode() );
		printf( "Concurrent Kernels: %d\n", getConcurrentKernels() );
		printf( "ECC Enabled: %d\n", getECCEnabled() );
		printf( "PCI Bus ID: %d\n", getPCIBusID() );
		printf( "PCI Device ID: %d\n", getPCIDeviceID() );
		printf( "TCC Driver: %d\n", getTCCDriver() );
		printf( "\n" );
	}

private:
	bool m_valid;
	int m_device_id;
	struct cudaDeviceProp m_properties;
};

#endif

