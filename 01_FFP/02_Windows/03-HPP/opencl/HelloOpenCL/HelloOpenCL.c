// header files 
// standard headers
#include <stdlib.h>
#include <stdio.h>

// OpenCL headers
#include <CL/opencl.h>

// global variables 
const int iNumberOfArrayElements = 5;

cl_platform_id oclPlatformID;
cl_device_id oclDeviceID;

cl_context oclContext;
cl_command_queue oclCommandQueue;

cl_program oclProgram;
cl_kernel oclKernel;

float *hostInput1 = NULL;
float *hostInput2 = NULL;
float *hostOutput = NULL;

cl_mem deviceInput1 = NULL;
cl_mem deviceInput2 = NULL;
cl_mem deviceOutput = NULL;

/* OpenCL kernel */
const char *oclSourceCode =
						"__kernel void vectAdd_GPU (__global float *a, __global float *b, __global float *out, int len)" \
						"{" \
							"int i = get_global_id(0);" \
							"if (i < len)" \
							"{" \
								"out[i] = a[i] + b[i];" \
							"}" \
						"}";

// entry-point function 
int main(void)
{
	// function prototypes
	void cleanup(void);

	// variable declarations
	int size = iNumberOfArrayElements * sizeof(float);
	cl_int result;

	// code
	//host memory
	hostInput1 = (float *)malloc(size);
	if (hostInput1 == NULL)
	{
		printf("Host Memory allocation is failed for hostInput1\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostInput2 = (float *)malloc(size);
	if (hostInput2 == NULL)
	{
		printf("Host Memory allocation is failed for hostInput2\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostOutput = (float *)malloc(size);
	if (hostOutput == NULL)
	{
		printf("Host Memory allocation is for hostOutput\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	//filling values
	hostInput1[0] = 101.0;
	hostInput1[1] = 102.0;
	hostInput1[2] = 103.0;
	hostInput1[3] = 104.0;
	hostInput1[4] = 105.0;

	hostInput2[0] = 201.0;
	hostInput2[1] = 202.0;
	hostInput2[2] = 203.0;
	hostInput2[3] = 204.0;
	hostInput2[4] = 205.0;

	result = clGetPlatformIDs(1, &oclPlatformID, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clGetPlatformIDs() failed : %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// get OpenCL supporting cpu device's ID
	result = clGetDeviceIDs(oclPlatformID, CL_DEVICE_TYPE_GPU, 1, &oclDeviceID, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clGetDeviceIDs() failE %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// create an OpenCL compute context
	oclContext = clCreateContext(NULL, 1, &oclDeviceID, NULL, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateContext() faile: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// create command queue
	oclCommandQueue = clCreateCommandQueue(oclContext, oclDeviceID, 0, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateCommandQueue() failed :%d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// create OpenCL program from .cl
	oclProgram = clCreateProgramWithSource(oclContext, 1, (const char **)&oclSourceCode, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateProgramWithSource() failed: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// build Opencl program
	result = clBuildProgram(oclProgram, 0, NULL, NULL, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		size_t len;
		char buffer[2048];
		clGetProgramBuildInfo(oclProgram, oclDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("program build log : %s\n", buffer);
		printf("clBuildProgram() failed: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// create OpenCL kernel by passing kernel function name that we used in .cl file
	oclKernel = clCreateKernel(oclProgram, "vectAdd_GPU", &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateKernel: failed with code %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// memory allocation
	deviceInput1 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for 1st input array: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	deviceInput2 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for 2st input array: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	deviceOutput = clCreateBuffer(oclContext, CL_MEM_WRITE_ONLY, size, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for output array: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// set 0 based 0th argument i.e deviceInput1
	result = clSetKernelArg(oclKernel, 0, sizeof(cl_mem), (void *)&deviceInput1);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() failed for 1st argument: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// set 0 based 1st argument i.e deviceInput2
	result = clSetKernelArg(oclKernel, 1, sizeof(cl_mem), (void *)&deviceInput2);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() failed for 2st argument: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// set 0 based 2nd argument i.e deviceOutput
	result = clSetKernelArg(oclKernel, 2, sizeof(cl_mem), (void *)&deviceOutput);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() failed for 3rd argument: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// set 0 based 3rd argument i.e len
	result = clSetKernelArg(oclKernel, 3, sizeof(cl_int), (void *)&iNumberOfArrayElements);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() failed for 4th argument %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// write above input device buffer to device memory
	result = clEnqueueWriteBuffer(oclCommandQueue, deviceInput1, CL_FALSE, 0, size, hostInput1, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueWriteBuffer() failed for 1st input device buffer: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = clEnqueueWriteBuffer(oclCommandQueue, deviceInput2, CL_FALSE, 0, size, hostInput2, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueWriteBuffer() failed for 2nd input device buffer: %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// kernel
	size_t global_size = 5;	// 1-D 5 element operation
	result = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueNDRangeKernel() failed : %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	//finish OpenCL command queue
	clFinish(oclCommandQueue);

	// read back result from the device into cpu variable
	result = clEnqueueReadBuffer(oclCommandQueue, deviceOutput, CL_TRUE, 0, size, (void *)hostOutput, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueReadBuffer() failed : %d\n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// display results
	int i;
	for (i = 0; i < iNumberOfArrayElements; i++)
	{
		printf("%f + %f = %f\n", hostInput1[i], hostInput2[i], hostOutput[i]);
	}

	cleanup();

	return (0);
}

void cleanup(void)
{
	// code
	if (deviceOutput)
	{
		clReleaseMemObject(deviceOutput);
		deviceOutput = NULL;
	}

	if (deviceInput2)
	{
		clReleaseMemObject(deviceInput2);
		deviceInput2 = NULL;
	}

	if (deviceInput1)
	{
		clReleaseMemObject(deviceInput1);
		deviceInput1 = NULL;
	}

	if (oclKernel)
	{
		clReleaseKernel(oclKernel);
		oclKernel = NULL;
	}

	if (oclProgram)
	{
		clReleaseProgram(oclProgram);
		oclProgram = NULL;
	}

	if (oclCommandQueue)
	{
		clReleaseCommandQueue(oclCommandQueue);
		oclCommandQueue = NULL;
	}

	if (oclContext)
	{
		clReleaseContext(oclContext);
		oclContext = NULL;
	}

	if (hostOutput)
	{
		free(hostOutput);
		hostOutput = NULL;
	}

	if (hostInput2)
	{
		free(hostInput2);
		hostInput2 = NULL;
	}

	if (hostInput1)
	{
		free(hostInput1);
		hostInput1 = NULL;
	}
}
