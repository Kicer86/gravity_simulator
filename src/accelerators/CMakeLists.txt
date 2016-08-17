
find_package(OpenMP)

set(ACC_SRS
        iaccelerator.hpp
)

set(ACC_COMPILATOR_FLAGS)
set(ACC_LINKER_FLAGS)

if(OpenMP_FOUND)

    list(APPEND ACC_SRC
        openmp_accelerator.cpp
        openmp_accelerator.hpp
    )

    set_source_files_properties(openmp_accelerator.cpp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
    list(APPEND ACC_LINKER_FLAGS ${OpenMP_CXX_FLAGS})

endif()
               #accelerators/avx_accelerator.cpp
               #accelerators/avx_accelerator.hpp


# It would be nice to directly link related libraries here, but CMake (so far) does not allow
# OBJECT type library to link anything.
# Some knowledge:
# https://bugreports.qt.io/browse/QTBUG-37105
#

add_library(accelerators OBJECT ${ACC_SRC})

# 'Export' flags to parent scope
set(ACC_COMPILATOR_FLAGS ${ACC_COMPILATOR_FLAGS} PARENT_SCOPE)
set(ACC_LINKER_FLAGS ${ACC_LINKER_FLAGS} PARENT_SCOPE)
