#include <stdio.h>

int main(void)
{
    //function declarations
    void PrintCUDADeviceProperties(void);

    //code
    PrintCUDADeviceProperties();
}

void PrintCUDADeviceProperties(void)
{
    //code
    printf("CUDA INFORMATION : \n");
    printf("===================================================\n");

    cudaError_t ret_cuda_rt;
    int dev_count;
    ret_cuda_rt = cudaGetDeviceCount(&dev_count);
    if (ret_cuda_rt != cudaSuccess)
    {
        printf("CUDA Runtime API Error - cudaGetDeviceCount \n", cudaGetErrorString(ret_cuda_rt));
    }
    else if (dev_count == 0)
    {
        printf("There is no CUDA Supported Device On this System\n");
        return;
    }
    else
    {
        printf("Total Number of CUDA Supporting GPU Device\Devices on this System : %d\n", dev_count);
        for (int i = 0; i < dev_count; i++)
        {
            cudaDeviceProp dev_prop;
            int driverVersion = 0, runtimeVersion = 0;

            ret_cuda_rt = cudaGetDeviceProperties(&dev_prop, i);
            if (ret_cuda_rt != cudaSuccess)
            {
                printf("%s in %s at line %d\n", cudaGetErrorString (ret_cuda_rt), __FILE__, __LINE__);
                return;
            }
            printf("\n");
            cudaDriverGetVersion(&driverVersion);
            cudaRuntimeGetVersion(&runtimeVersion);
            printf("******CUDA DRIVER AND RUNTIME INFORMATION******\n");
            printf("==========================================================");
            printf("CUDA Driver Version               :%d.%d\n", driverVersion / 1000, (driverVersion % 100) / 10);
            printf("CUDA Runtime Version              :%d.%d\n", runtimeVersion / 1000, (runtimeVersion % 100) / 10);
            printf("\n");
            printf("=========================================================");
            printf("******GPU DEVICE GENERAL INFORMATION******\n");
            printf("===========================================================");
            printf("GPU Device Number                 :%d\n", i);
            printf("GPU Device Name                   :%s\n", dev_prop.name);
            printf("GPU Device Compute Capability            :%d\n", dev_prop.major, dev_prop.minor);
            printf("GPU Device Clock Rate              :%d\n", dev_prop.clockRate);
            printf("GPU Device Type                    :");
            if (dev_prop.integrated)
                printf("Integrated (on-Board)\n");
            else
                printf("Discrete (card)\n");
            printf("\n");
            printf("****** GPU DEVICE MEMORY INFORMATION******\n");
            printf("==========================================================\n");
            printf("GPU Device Total Memory              :%.0fGB = %.0f MB = %llu Bytes\n", ((float)dev_prop.totalGlobalMem / 1048576.0f) / 1024.0f, (unsigned long  long) dev_prop.totalGlobalMem);
            printf("GPU Device Constant Memory                %lu Bytes\n", (unsigned long)dev_prop.totalConstMem);
            printf("GPU Device Shared Memory Per SMProcessor         :%lu\n", (unsigned long)dev_prop.sharedMemPerBlock);
            printf("\n");
            printf("******GPU DEVICE MULTIPROCESSOR INFORMATION******\n");
            printf("===========================================================\n");
            printf("GPU Device Number of SMProcessors                   :%d\n", dev_prop.multiProcessorCount);
            printf("GPU Device Number Of Registers per SMProcessor           :%d\n", dev_prop.regsPerBlock);
            printf("\n");
            printf("******GPU DEVICE THREAD INFORMATION******\n");
            printf("============================================================\n");
            printf("GPU Device Maximum Number Of Threads Per SMProcessor   %d\n", dev_prop.maxThreadsPerMultiProcessor);
            printf("GPU Device Maximum Number of Threads per block     %d\n", dev_prop.maxThreadsPerBlock);
            printf("GPU Device Threads in Warp            :%d\n", dev_prop.warpSize);
            printf("GPU Device Maximun Threads Dimensions        :(%d, %d, %d)\n", dev_prop.maxThreadsDim[0], dev_prop.maxThreadsDim[1], dev_prop.maxThreadsDim[2]);
            printf("GPU Device Maximum Grid Dimensions         :(%d, %d, %d)\n", dev_prop.maxGridSize[0], dev_prop.maxGridSize[1], dev_prop.maxGridSize[2]);
            printf("\n");
            printf("******GPU DEVICE DRIVER INFORMATION******\n");
            printf("=============================================\n");
            printf("GPU Device has ECC support \n", dev_prop.ECCEnabled ? "Enabled" : "Disabled");
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
        printf("GPU Device CUDA Driver Mode (TCC Or WDDM)  : %s\n", dev_prop.tccDriver ? "TCC (Tesla Compute Cluster Driver)" : "WDMM (Windows Display Driver Model)");
#endif 
    printf("***********************************************************************************************************************************************************************\n");
        }    
    }
}
