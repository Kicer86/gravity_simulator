
cmake_minimum_required (VERSION 2.8)
project (gravity-opencl)
set(CMAKE_CXX_FLAGS "-Wall -O3 -std=c++11")
add_executable(gravity-opencl main.cpp ocl_kernel.cpp)
target_link_libraries(gravity-opencl OpenCL)
