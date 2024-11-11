#include "SignWave.cu.h"

// Cuda kernel for signwave
__global__ void signWaveKernel(float4* pos, int width, int height, float time)
{
	// Code
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	float u = (float)x / (float)width;
	float v = (float)y / (float)height;

	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;

	float frequency = 4.0f;
	float w = sinf(u * frequency + time) * cosf(v * frequency + time) * 0.5f;

	pos[y * width + x] = make_float4(u, w, v, 1.0f);
}

//User defined function to call Cuda Kernel
void launchCUDAKernel(float4* pos, int width, int height, float time)
{
	dim3 block(8, 8, 1);
	dim3 grid(width / block.x,  height / block.y, 1);
	signWaveKernel << < grid,block >> > (pos, width, height, time);
}

