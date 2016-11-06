
set(ACC_SRS
        iaccelerator.hpp
)

set(ACC_COMPILATOR_FLAGS)
set(ACC_LINKER_FLAGS)

option(ENABLE_OPENMP "Allows to disable OpenMP even if detected" ON)
option(ENABLE_AVX    "Allows to disable AVX extensions even if detected" ON)


# CPU accelerator (OpenMP friendly)

list(APPEND ACC_SRC
    cpu_accelerator_base.cpp
    cpu_accelerator_base.hpp
    simple_cpu_accelerator.cpp
    simple_cpu_accelerator.hpp
)

#detect OpenMP

find_package(OpenMP)

add_feature_info(Multithread_support OPENMP_FOUND "speeds up calculations.")


# detect AVX

set(AVX_FOUND 0)
if(UNIX)

    try_run(AVX_DETECTION_RUN AVX_DETECTION_COMPILE
            ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/detect_avx.c
            COMPILE_DEFINITIONS -mavx
    )

    if (AVX_DETECTION_COMPILE AND AVX_DETECTION_RUN)
        set(AVX_FOUND 1)
    endif()

endif()

# AVX accelerator (OpenMP friendly)

if (AVX_FOUND AND ENABLE_AVX)

    list(APPEND ACC_SRC
        avx_accelerator.cpp
        avx_accelerator.hpp
    )

    set_source_files_properties(avx_accelerator.cpp PROPERTIES COMPILE_FLAGS "-mavx")

endif()

add_feature_info(AVX_accelerator AVX_FOUND "speeds up calculations.")


#detect OpenCL (OpenMP friendly)

find_package(OpenCL)
find_package(Boost)

set(OPENCL_ENABLED 0)

if(OpenCL_FOUND AND Boost_FOUND)

    include_directories(SYSTEM
                            ${OpenCL_INCLUDE_DIRS}
                            ${Boost_INCLUDE_DIRS}
    )

    list(APPEND ACC_SRC
        opencl_accelerator.cpp
        opencl_accelerator.hpp
    )

    list(APPEND ACC_LINKER_FLAGS ${OpenCL_LIBRARIES})

    set(OPENCL_ENABLED 1)

endif()

add_feature_info(OpenCL_accelerator OPENCL_ENABLED  "speeds up calculations.")


# OpenMP setup

if(OPENMP_FOUND AND ENABLE_OPENMP)

    set_source_files_properties(cpu_accelerator_base.cpp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
    set_source_files_properties(simple_cpu_accelerator.cpp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
    set_source_files_properties(opencl_accelerator.cpp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})

    list(APPEND ACC_LINKER_FLAGS ${OpenMP_CXX_FLAGS})

else()

    # add fake implementation of openmp functions
    list(APPEND ACC_SRC fake_openmp.cpp)

endif()


# It would be nice to directly link related libraries here, but CMake (so far) does not allow
# OBJECT type library to link anything.
# Some knowledge:
# https://bugreports.qt.io/browse/QTBUG-37105
#

add_library(accelerators OBJECT ${ACC_SRC})

# 'Export' flags to parent scope
set(ACC_COMPILATOR_FLAGS ${ACC_COMPILATOR_FLAGS} PARENT_SCOPE)
set(ACC_LINKER_FLAGS ${ACC_LINKER_FLAGS} PARENT_SCOPE)

# unit tests
#add_subdirectory(unit_tests)
