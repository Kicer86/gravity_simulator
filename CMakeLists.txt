
project(gravity_simulator)

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

include(FeatureSummary)

option(BUILD_TESTS "Build unit tests" OFF)
add_feature_info("Build tests" BUILD_TESTS "Enables build of tests. Feature controled by BUILD_TESTS variable.")

if(BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(src)
#add_subdirectory(gravity)


if(UNIX)

    find_program(OBJ_DUMP objdump)

    if(OBJ_DUMP)

        add_custom_command(OUTPUT gravity_simulator.lst
                           COMMAND ${OBJ_DUMP} -M intel -d $<TARGET_FILE:gravity_simulator> > gravity_simulator.lst
                           DEPENDS gravity_simulator
        )

        add_custom_target(assembly_listing ALL
                          DEPENDS gravity_simulator.lst
        )


    endif()

endif()


feature_summary(WHAT ALL)

#unit tests
if(BUILD_TESTS)
    add_custom_target(RunUnitTests ALL
                      COMMAND ctest
                      DEPENDS accelerators_tests
                     )
endif()
