
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
 * that at least 4 group can run in parallel on any HW,
 * provided there is enough resources of other types.
 */
#define SHARED_MEM_SIZE_PER_GROUP (2 * 1024)

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
  kernel void ocl_kernel1(global const float * restrict objX, global const float * restrict objY, global const float * restrict mass, global float * restrict forceX, global float * restrict forceY, const int count) { \
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
      const float Fg = (G * mi) * (mj / len2); \
      const float len = sqrt(len2); \
      fx += dx / len * Fg; \
      fy += dy / len * Fg; \
    } \
    forceX[gid] = fx; \
    forceY[gid] = fy; \
  } \
}";

std::string source2 = " \
  kernel void ocl_kernel2(global const float * restrict objX, global const float * restrict objY, global const float * restrict mass, global float * restrict forceX, global float * restrict forceY, const int count) { \
  const float G = 6.6732e-11; \
    const int count_local = ((count + LOCAL_MEM_SIZE - 1) / LOCAL_MEM_SIZE) * LOCAL_MEM_SIZE; \
    local float sx[LOCAL_MEM_SIZE]; \
    local float sy[LOCAL_MEM_SIZE]; \
    local float sm[LOCAL_MEM_SIZE]; \
    const float xi = objX[get_global_id(0)]; \
    const float yi = objY[get_global_id(0)];  \
    const float mi = mass[get_global_id(0)]; \
    float fx = 0, fy = 0; \
    for (int c = 0; c < count_local; c += LOCAL_MEM_SIZE) { \
      const int n = min(count - c, LOCAL_MEM_SIZE); \
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
        float len2 = dx * dx + dy * dy; \
        const int notzero = (len2 != 0); \
        len2 += (len2 == 0); \
        const float Fg = (G * mi) * (mk / len2); \
        const float len = sqrt(len2); \
        fx += dx * Fg / len * notzero; \
        fy += dy * Fg / len * notzero; \
      } \
      barrier(CLK_LOCAL_MEM_FENCE); \
    } \
  if (get_global_id(0) < count) { \
    forceX[get_global_id(0)] = fx; \
    forceY[get_global_id(0)] = fy; \
  } \
}";

std::string source3 = " \
  kernel void ocl_kernel3(global const float * restrict objX, global const float * restrict objY, global const float * restrict mass, global float * restrict forceX, global float * restrict forceY) { \
    const float G = 6.6732e-11; \
    const int lid = get_local_id(0); \
    const int gid = get_global_id(0); \
    const gsiz = get_global_size(0); \
    local float sx[128]; \
    local float sy[128]; \
    local float sm[128]; \
    const float xi = objX[gid]; \
    const float yi = objY[gid];  \
    const float mi = mass[gid]; \
    float fx = 0, fy = 0; \
    for (int c = lid; c < gsiz; c += 128) { \
      barrier(CLK_LOCAL_MEM_FENCE); \
      sx[lid] = objX[c]; \
      sy[lid] = objY[c]; \
      sm[lid] = mass[c]; \
      barrier(CLK_LOCAL_MEM_FENCE); \
      for(int k = 0; k < 128; ++k) { \
        const float xk = sx[k]; \
        const float yk = sy[k]; \
        const float mk = sm[k]; \
        const float dx = xk - xi; \
        const float dy = yk - yi; \
        float len2 = dx * dx + dy * dy; \
        const int notzero = (len2 != 0); \
        len2 += (len2 == 0); \
        const float Fg = (G * mi) * (mk / len2); \
        const float len = sqrt(len2); \
        fx += dx * Fg / len * notzero; \
        fy += dy * Fg / len * notzero; \
      } \
    } \
    forceX[get_global_id(0)] = fx; \
    forceY[get_global_id(0)] = fy; \
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
  sources.push_back(source3);

  std::ostringstream str;
  str << "-DLOCAL_MEM_SIZE=";
  str << (SHARED_MEM_SIZE_PER_GROUP / sizeof(float) / 4);

  program = cl::Program(context, sources);
  try {
    program.build({default_device}, str.str().c_str());
  } catch (...) {
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
  try {
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

    int local = GROUP_SIZE;
    int global = ((count + local - 1) / local) * local;
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global),
                               cl::NDRange(local));

    queue.enqueueReadBuffer(fx, CL_TRUE, 0, siz, forcex);
    queue.enqueueReadBuffer(fy, CL_TRUE, 0, siz, forcey);

    queue.finish();
  } catch (const cl::Error &e) {
    std::cout << e.what() << ": " << e.err() << std::endl;
    abort();
  }
}

void OpenCL::exec1(const float *objX, const float *objY, const float *mass,
                   float *forcex, float *forcey, const int count) {
  exec("ocl_kernel1", objX, objY, mass, forcex, forcey, count);
}

void OpenCL::exec2(const float *objX, const float *objY, const float *mass,
                   float *forcex, float *forcey, const int count) {
  exec("ocl_kernel2", objX, objY, mass, forcex, forcey, count);
}

void OpenCL::exec3(const float *objX, const float *objY, const float *mass,
                   float *forcex, float *forcey, const int count) {
  try {
    const size_t threads_per_group = 128;
    const size_t total_groups =
        ((count + threads_per_group - 1) / threads_per_group);
    const size_t total_elems = total_groups * threads_per_group;
    const size_t total_size = total_groups * threads_per_group * sizeof(float);

    cl::Buffer x(context, CL_MEM_READ_ONLY, total_size);
    cl::Buffer y(context, CL_MEM_READ_ONLY, total_size);
    cl::Buffer m(context, CL_MEM_READ_ONLY, total_size);
    cl::Buffer fx(context, CL_MEM_WRITE_ONLY, total_size);
    cl::Buffer fy(context, CL_MEM_WRITE_ONLY, total_size);

    const size_t siz = count * sizeof(float);
    const size_t rsiz = (total_elems - count) * sizeof(float);

    queue.enqueueWriteBuffer(x, CL_FALSE, 0, siz, objX);
    queue.enqueueWriteBuffer(y, CL_FALSE, 0, siz, objY);
    queue.enqueueWriteBuffer(m, CL_FALSE, 0, siz, mass);

    queue.enqueueFillBuffer(x, .0f, siz, rsiz);
    queue.enqueueFillBuffer(y, .0f, siz, rsiz);
    queue.enqueueFillBuffer(m, .0f, siz, rsiz);

    cl::Kernel kernel(program, "ocl_kernel3");
    kernel.setArg(0, x);
    kernel.setArg(1, y);
    kernel.setArg(2, m);
    kernel.setArg(3, fx);
    kernel.setArg(4, fy);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange,
                               cl::NDRange(total_groups * threads_per_group),
                               cl::NDRange(threads_per_group));

    queue.enqueueReadBuffer(fx, CL_FALSE, 0, siz, forcex);
    queue.enqueueReadBuffer(fy, CL_FALSE, 0, siz, forcey);

    queue.finish();

  } catch (const cl::Error &e) {
    std::cout << e.what() << ": " << e.err() << std::endl;
    abort();
  }
}
