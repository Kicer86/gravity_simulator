
#include "ocl_kernel.hpp"

#include <CL/cl.h>

#include <string>
#include <iostream>
#include <sstream>

#include <cstring>
#include <cassert>

// ===============================================
/*
 * Tuning parameter (shared memory in CUDA).
 * The newer your GPU the bigger value you can try.
 * 16kB is guaranteed total amount on all Nvidia HW.
 * Each thread in a group needs upto 16B. 1024 means
 * that 4 group can run in parallel on any HW.
 */
#define SHARED_MEM_SIZE_PER_GROUP 1024

/*
 * Tuning parameter.
 * The max value is limited by resources of your GPU.
 * The min value should be 32.
 */
#define GROUP_SIZE 128

// ===============================================

#define XSTR(x) #x
#define STR(x) XSTR(x)

std::string source1 = " \
  kernel void ocl_kernel1(global const float *objX, global const float *objY, global const float *mass, global float *forceX, global float *forceY, const int count) { \
  const int gid = get_global_id(0); \
  const float G = 6.6732e-11; \
  if (gid < count) { \
    const float xi = objX[gid]; \
    const float yi = objY[gid];  \
    const float mi = mass[gid]; \
    float fx = 0, fy = 0; \
    for (int j = 0; j < count; j++) { \
      const float xj = objX[j]; \
      const float yj = objY[j]; \
      const float mj = mass[j]; \
      const float dx = xj - xi; \
      const float dy = yj - yi; \
      const float len2 = dx * dx + dy * dy; \
      if(len2 == 0) continue; \
      const float Fg = G * mi * mj / len2; \
      const float len = sqrt(len2); \
      fx += dx * Fg / len; \
      fy += dy * Fg / len; \
    } \
    forceX[gid] = fx; \
    forceY[gid] = fy; \
  } \
}";

std::string source2 = " \
  kernel void ocl_kernel2(global const float *objX, global const float *objY, global const float *mass, global float *forceX, global float *forceY, const int count) { \
  const float G = 6.6732e-11; \
  if (get_global_id(0) < count) { \
    const int count_local = ((count + LOCAL_MEM_SIZE - 1) / LOCAL_MEM_SIZE) * LOCAL_MEM_SIZE; \
    local float sx[LOCAL_MEM_SIZE]; \
    local float sy[LOCAL_MEM_SIZE]; \
    local float sm[LOCAL_MEM_SIZE]; \
    const float xi = objX[get_global_id(0)]; \
    const float yi = objY[get_global_id(0)];  \
    const float mi = mass[get_global_id(0)]; \
    float fx = 0, fy = 0; \
    for (int c = 0; c < count_local; c += LOCAL_MEM_SIZE) { \
      int n = count - c; \
      n = n > LOCAL_MEM_SIZE ? LOCAL_MEM_SIZE : n; \
      for(int k = get_local_id(0); k < n; k += get_local_size(0)) { \
        sx[k] = objX[c + k]; \
        sy[k] = objY[c + k]; \
        sm[k] = mass[c + k]; \
      } \
      barrier(CLK_LOCAL_MEM_FENCE); \
      for(int k = 0; k < n; ++k) { \
        const float xk = sx[k]; \
        const float yk = sy[k]; \
        const float mk = sm[k]; \
        const float dx = xk - xi; \
        const float dy = yk - yi; \
        const float len2 = dx * dx + dy * dy; \
        if(len2 == 0) continue; \
        const float Fg = G * mi * mk / len2; \
        const float len = sqrt(len2); \
        fx += dx * Fg / len; \
        fy += dy * Fg / len; \
      } \
      barrier(CLK_LOCAL_MEM_FENCE); \
    } \
    forceX[get_global_id(0)] = fx; \
    forceY[get_global_id(0)] = fy; \
  } \
}";

OpenCL::OpenCL() {
  std::vector<cl::Platform> all_platforms;
  cl::Platform::get(&all_platforms);
  if (all_platforms.size() == 0) {
    std::cout << " No platforms found. Check OpenCL installation!\n";
    exit(1);
  }
  cl::Platform default_platform = all_platforms[0];

  std::vector<cl::Device> all_devices;
  default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
  if (all_devices.size() == 0) {
    std::cout << " No devices found. Check OpenCL installation!\n";
    exit(1);
  }
  cl::Device default_device = all_devices[0];

  context = cl::Context({default_device});
  cl::Program::Sources sources;
  sources.push_back(source1);
  sources.push_back(source2);

  std::ostringstream str;
  str << "-DLOCAL_MEM_SIZE=128";
  //  str << (SHARED_MEM_SIZE_PER_GROUP / sizeof(float) / 4);

  program = cl::Program(context, sources);
  if (program.build({default_device}, " -DLOCAL_MEM_SIZE=128") != CL_SUCCESS) {
    std::cout << "Error building: "
              << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device)
              << std::endl;
    exit(1);
  }

  queue = cl::CommandQueue(context, default_device);
};

void OpenCL::exec(const std::string &progname, const float *objX,
                  const float *objY, const float *mass, float *forcex,
                  float *forcey, const int count) {

  const size_t siz = sizeof(float) * count;
  cl::Buffer x(context, CL_MEM_READ_ONLY, siz);
  cl::Buffer y(context, CL_MEM_READ_ONLY, siz);
  cl::Buffer m(context, CL_MEM_READ_ONLY, siz);
  cl::Buffer fx(context, CL_MEM_WRITE_ONLY, siz);
  cl::Buffer fy(context, CL_MEM_WRITE_ONLY, siz);

  queue.enqueueWriteBuffer(x, CL_TRUE, 0, siz, objX);
  queue.enqueueWriteBuffer(y, CL_TRUE, 0, siz, objY);
  queue.enqueueWriteBuffer(m, CL_TRUE, 0, siz, mass);

  cl::Kernel kernel(program, progname.c_str());
  kernel.setArg(0, x);
  kernel.setArg(1, y);
  kernel.setArg(2, m);
  kernel.setArg(3, fx);
  kernel.setArg(4, fy);
  kernel.setArg(5, count);

  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(std::max<int>(count, GROUP_SIZE)),
                             cl::NDRange(GROUP_SIZE));

  queue.enqueueReadBuffer(fx, CL_TRUE, 0, siz, forcex);
  queue.enqueueReadBuffer(fy, CL_TRUE, 0, siz, forcey);

  queue.finish();
}

void OpenCL::exec1(const float *objX, const float *objY, const float *mass,
                   float *forcex, float *forcey, const int count) {
  exec("ocl_kernel1", objX, objY, mass, forcex, forcey, count);
}

void OpenCL::exec2(const float *objX, const float *objY, const float *mass,
                   float *forcex, float *forcey, const int count) {
  exec("ocl_kernel2", objX, objY, mass, forcex, forcey, count);
}
