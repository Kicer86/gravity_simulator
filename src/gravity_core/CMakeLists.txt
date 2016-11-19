
add_subdirectory(accelerators)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_library(gravity_core
               object.cpp
               object.hpp
               objects.cpp
               objects.hpp
               simulation_engine.cpp
               simulation_engine.hpp
               types.hpp
               $<TARGET_OBJECTS:accelerators>
)

target_link_libraries(gravity_core
                      PRIVATE
                        ${ACC_LINKER_FLAGS}
)

# unit tests
if(BUILD_TESTS)
    add_subdirectory(unit_tests)
endif()
