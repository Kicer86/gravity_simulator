
set(ACC_SRS
        iaccelerator.hpp
)

set(ACC_COMPILATOR_FLAGS)
set(ACC_LINKER_FLAGS)

#detect OpenMP

find_package(OpenMP)

if(OPENMP_FOUND)

    list(APPEND ACC_SRC
        cpu_accelerator.cpp
        cpu_accelerator.hpp
    )

    set_source_files_properties(cpu_accelerator.cpp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})

endif()

add_feature_info(OpenMP_accelerator OPENMP_FOUND "speeds up calculations.")


# detect AVX (not nice solution)
set(AVX_ENABLED 0)
if(UNIX)

    try_run(AVX_DETECTION_RUN AVX_DETECTION_COMPILE
            ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/detect_avx.c
            COMPILE_DEFINITIONS -mavx
    )

    if (AVX_DETECTION_COMPILE AND AVX_DETECTION_RUN)
        set(AVX_ENABLED 1)
    endif()

endif()

if (AVX_ENABLED)

    list(APPEND ACC_SRC
        avx_accelerator.cpp
        avx_accelerator.hpp
    )

    set_source_files_properties(avx_accelerator.cpp PROPERTIES COMPILE_FLAGS "-mavx")

endif()

add_feature_info(AVX_accelerator AVX_ENABLED "speeds up calculations.")


#detect OpenCL

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


# Thread base accelerators base

if(AVX_ENABLED OR OPENMP_FOUND)

    list(APPEND ACC_SRC
        mt_accelerator_base.cpp
        mt_accelerator_base.hpp
    )

    if (OPENMP_FOUND)
        set_source_files_properties(mt_accelerator_base.cpp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
        list(APPEND ACC_LINKER_FLAGS ${OpenMP_CXX_FLAGS})
    endif()

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
