#include <cuda.h>
#include <iostream>
#include <iomanip>

extern "C" void* mgpuModuleLoad(void* data, long long size) {
    CUmodule mod;
    CUdevice device;
    CUcontext ctx;


    CUresult res = cuInit(0);
    if (res != CUDA_SUCCESS) {
        std::cerr << "cuInit failed with error code " << res << "\n";
        return nullptr;
    }

    // Get CUDA device
    res = cuDeviceGet(&device, 0);  // Use device 0
    if (res != CUDA_SUCCESS) {
        std::cerr << "cuDeviceGet failed with error code " << res << "\n";
        return nullptr;
    }

    // Create context
    res = cuCtxCreate(&ctx, 0, device);
    if (res != CUDA_SUCCESS) {
        std::cerr << "cuCtxCreate failed with error code " << res << "\n";
        return nullptr;
    }

    //CUresult initRes = cuInit(0);  
    res = cuModuleLoadData(&mod, data);

    std::cout<<"RES: "<<res<<"\n";

    if (res != CUDA_SUCCESS) std::cerr << "cuModuleLoadData failed\n";
    return mod;
}

extern "C" void mgpuModuleUnload(void* mod) {
    cuModuleUnload((CUmodule)mod);
}

extern "C" void* mgpuModuleGetFunction(void* mod, const char* name) {
    CUfunction func;


    CUresult res = cuModuleGetFunction(&func, (CUmodule)mod, name);
    std::cout<<"Res "<<res <<"\n";
    
    if (res != CUDA_SUCCESS) std::cerr << "cuModuleGetFunction failed\n";
    return func;
}

extern "C" void* mgpuStreamCreate() {
    CUstream stream;
    cuStreamCreate(&stream, 0);
    return stream;
}

extern "C" void mgpuLaunchKernel(void* func,
                                 long long gx, long long gy, long long gz,
                                 long long bx, long long by, long long bz,
                                 int shared, void* stream,
                                 void* params, void*, long long) {
    void** param_array = (void**)params;
    cuLaunchKernel((CUfunction)func,
                   gx, gy, gz,
                   bx, by, bz,
                   shared, (CUstream)stream, param_array, nullptr);
}

extern "C" void mgpuStreamSynchronize(void* stream) {
    cuStreamSynchronize((CUstream)stream);
}

extern "C" void mgpuStreamDestroy(void* stream) {
    cuStreamDestroy((CUstream)stream);
}
