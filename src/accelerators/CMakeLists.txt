
set(ACC_SRS
        iaccelerator.hpp
)

set(ACC_COMPILATOR_FLAGS)
set(ACC_LINKER_FLAGS)

#detect OpenMP

find_package(OpenMP)

if(OpenMP_FOUND)

    list(APPEND ACC_SRC
        cpu_accelerator.cpp
        cpu_accelerator.hpp
    )

    set_source_files_properties(cpu_accelerator.cpp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
    list(APPEND ACC_LINKER_FLAGS ${OpenMP_CXX_FLAGS})

endif()

add_feature_info(OpenMP_accelerator OpenMP_FOUND "speeds up calculations.")


# detect AVX (not nice solution)
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

if (AVX_FOUND)

    list(APPEND ACC_SRC
        avx_accelerator.cpp
        avx_accelerator.hpp
    )

    set_source_files_properties(avx_accelerator.cpp PROPERTIES COMPILE_FLAGS "-mavx")

endif()

add_feature_info(AVX_accelerator ${AVX_FOUND} "speeds up calculations.")

if(AVX_FOUND OR OpenMP_FOUND)
    list(APPEND ACC_SRC
        mt_accelerator_base.cpp
        mt_accelerator_base.hpp
    )

    if (OpenMP_FOUND)
        set_source_files_properties(mt_accelerator_base.cpp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
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
