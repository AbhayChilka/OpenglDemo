// standard headers
#include <stdio.h>
#include <math.h> // fabs()

// OpenCL headers
#include <CL/opencl.h>

#include "helper_timer.h"

// global variables
// const int iNumberOfArrayElements = 5;
const int iNumberOfArrayElements = 11444777;

cl_platform_id oclPlatformID;
cl_device_id oclDeviceID;

cl_context oclContext;
cl_command_queue oclCommandQueue;

cl_program oclProgram;
cl_kernel oclKernel;

float *hostInput1 = NULL;
float *hostInput2 = NULL;
float *hostOutput = NULL;
float *gold = NULL;

cl_mem deviceInput1 = NULL;
cl_mem deviceInput2 = NULL;
cl_mem deviceOutput = NULL;

float timeOnCPU = 0.0f;
float timeOnGPU = 0.0f;

// OpenCL kernel
const char *oclSourceCode =
    "_kernel void vecAddGPU( __global float *in1, __global float *in2, __global float *out,int len)"
    "{"
    "int i=get_global_id(0);"
    "if(i < len)"
    "{"
    "out[i]=in1[1]+in2[1];"
    "}"
    "}";

// entry-point function
int main(void)
{
    // function declarations
    void fillFloatArrayWithRandomNumbers(float *, int);
    size_t roundGlobalSizeToNearestMultipleOfLocalSize(int, unsigned int);
    void vecAddCPU(float *, float *, float *, int);
    void cleanup(void);

    // variable declarations
    int size = iNumberOfArrayElements * sizeof(float);
    cl_int result;

    // code
    // host memory allocation
    hostInput1 = (float *)malloc(size);
    if (hostInput1 == NULL)
    {
        printf("Host Memory allocation is failed for hostInput1 array.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    hostInput2 = (float *)malloc(size);
    if (hostInput2 == NULL)
    {
        printf("Host Memory allocation is failed for hostInput2 array.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    hostOutput = (float *)malloc(size);
    if (hostOutput == NULL)
    {
        printf("Host Memory allocation is failed for hostOutput array.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    gold = (float *)malloc(size);
    if (gold == NULL)
    {
        printf("Host Memory allocation is failed for gold array.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    // filling values into host arrays
    fillFloatArrayWithRandomNumbers(hostInput1, iNumberOfArrayElements);
    fillFloatArrayWithRandomNumbers(hostInput2, iNumberOfArrayElements);

    // get OpenCL supporting platform's ID
    result = clGetPlatformIDs(1, &oclPlatformID, NULL);
    if (result != CL_SUCCESS)
    {
        printf("clGetPlatformIDs() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    // get OpenCL supporting CPU device's ID
    result = clGetDeviceIDs(oclPlatformID, CL_DEVICE_TYPE_GPU, 1, &oclDeviceID, NULL);
    if (result != CL_SUCCESS)
    {

        printf("clCreateContext() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    // create Command Queue
    oclCommandQueue = clCreateCommandQueue(oclContext, oclDeviceID, 0, &result);
    if (result != CL_SUCCESS)
    {
        printf("clCreateCommandQueue() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    oclProgram = clCreateProgramWithSource(oclContext, 1, (const char **)&oclSourceCode, NULL, &result);
    if (result != CL_SUCCESS)
    {
        printf("clCreateProgramWithSource() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    result = clBuildProgram(oclProgram, 0, NULL, NULL, NULL, NULL);
    if (result != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        clGetProgramBuildInfo(oclProgram, oclDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("program Build Log : %s\n", buffer);
        printf("clBuildProgram() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    oclKernel = clCreateKernel(oclProgram, "vecAddGPU", &result);
    if (result != CL_SUCCESS)
    {
        printf("clCreateKernel() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    // device Memory allocation
    deviceInput1 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &result);
    if (result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    deviceInput2 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &result);
    if (result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    deviceOutput = clCreateBuffer(oclContext, CL_MEM_WRITE_ONLY, size, NULL, &result);
    if (result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    // set Kernel Arguments
    result = clSetKernelArg(oclKernel, 0, sizeof(cl_mem), (void *)&deviceInput1);
    if (result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    result = clSetKernelArg(oclKernel, 1, sizeof(cl_mem), (void *)&deviceInput2);
    if (result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    result = clSetKernelArg(oclKernel, 2, sizeof(cl_mem), (void *)&deviceOutput);
    if (result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    result = clSetKernelArg(oclKernel, 3, sizeof(cl_int), (void *)&iNumberOfArrayElements);
    if (result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    // write host buffer to device Buffer

    result = clEnqueueWriteBuffer(oclCommandQueue, deviceInput1, CL_TRUE, 0, size, hostInput1, 0, NULL, NULL);
    if (result != CL_SUCCESS)
    {
        printf("clEnqueueWriteBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    result = clEnqueueWriteBuffer(oclCommandQueue, deviceInput2, CL_TRUE, 0, size, hostInput2, 0, NULL, NULL);
    if (result != CL_SUCCESS)
    {
        printf("clEnqueueWriteBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    // Kernel Configuration
    size_t localWorkSize = 256;
    size_t globalWorkSize;
    globalWorkSize = roundGlobalSizeToNearestMultipleOfLocalSize(localWorkSize, iNumberOfArrayElements);

    // start timer
    StopWatchInterface *timer = NULL;
    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    result = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
    if (result != CL_SUCCESS)
    {
        printf("clEnqueueNDRangeKernel() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    clFinish(oclCommandQueue);

    sdkStopTimer(&timer);
    timeOnGPU = sdkGetTimerValue(&timer);
    sdkDeleteTimer(&timer);

    // read result from device to host
    result = clEnqueueReadBuffer(oclCommandQueue, deviceOutput, CL_TRUE, 0, size, hostOutput, 0, NULL, NULL);
    if (result != CL_SUCCESS)
    {
        printf("clEnqueueReadBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);
    }

    // Vector Addition On CPU
    vecAddCPU(hostInput1, hostInput2, gold, iNumberOfArrayElements);

    // Comparison
    const float epsilon = 0.000001f;
    int breakValue = -1;
    bool bAccuracy = true;

    for (int i = 0; i < iNumberOfArrayElements; i++)
    {
        float val1 = gold[i];
        float val2 = hostOutput[i];

        if (fabs(val1 - val2) > epsilon)
        {
            bAccuracy = false;
            breakValue = i;
            break;
        }
    }

    char str[128];
    if (bAccuracy == false)
    {
        sprintf(str, "Comparison Of CPU and GPU Vector Addition is not Within Accuracy of %f at array index %d \n", epsilon, breakValue);
    }
    else
    {
        sprintf(str, "Comparison of CPU and GPU Vector Addition is Within accuracy of %f\n", epsilon);
    }

    // OutPut
    printf("Array1 begins from 0th index  %.6f to %dth index %.6f\n", hostInput1[0], iNumberOfArrayElements - 1, hostInput1[iNumberOfArrayElements - 1]);
    printf("Array2 begins from 0th index  %.6f to %dth index %.6f\n", hostInput2[0], iNumberOfArrayElements - 1, hostInput2[iNumberOfArrayElements - 1]);
    printf("OpenCL Kernel Global Work size  = %llu and Local Work Size = %llu\n", globalWorkSize, localWorkSize);

    printf("Output Array begins from 0th index  %.6f to %dth index %.6f\n", hostOutput[0], iNumberOfArrayElements - 1, hostOutput[iNumberOfArrayElements - 1]);

    printf("Time Taken by CPU : %.6f\n", timeOnCPU);
    printf("Time Taken by GPU : %.6f\n", timeOnGPU);
    printf("%s\n", str);
    cleanup();
    return 0;
}

void fillFloatArrayWithRandomNumbers(float *arr, int len)
{
    float fscale = 1.0f / (float)RAND_MAX;

    for (int i = 0; i < len; i++)
        arr[i] = fscale * rand();
}

size_t roundGlobalSizeToNearestMultipleOfLocalSize(int local_size, unsigned int global_size)
{
    // code
    unsigned int r = global_size % local_size;

    if (r == 0)
        return global_size;
    else
        return (global_size + local_size - r);
}

void vecAddCPU(float *in1, float *in2, float *out, int len)
{
    StopWatchInterface *timer = NULL;
    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    for (int i = 0; i < len; i++)
        out[i] = in1[i] + in2[i];

    sdkStopTimer(&timer);
    timeOnCPU = sdkGetTimerValue(&timer);
    sdkDeleteTimer(&timer);
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
    if (gold)
    {
        free(gold);
        gold = NULL;
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
