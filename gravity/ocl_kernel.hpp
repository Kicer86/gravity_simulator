#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include <CL/cl2.hpp>

class OpenCL {
public:
  OpenCL();
  void exec1(const float *objX, const float *objY, const float *mass,
             float *forcex, float *forcey, const int count);
  void exec2(const float *objX, const float *objY, const float *mass,
             float *forcex, float *forcey, const int count);
  void exec3(const float *objX, const float *objY, const float *mass,
             float *forcex, float *forcey, const int count);

private:
  void exec(const std::string &progname, const float *objX, const float *objY,
            const float *mass, float *forcex, float *forcey, const int count);

private:
  cl::Device device;
  cl::Context context;
  cl::Program program;
  cl::CommandQueue queue;
};
