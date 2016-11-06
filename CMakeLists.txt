
project(gravity_simulator)

cmake_minimum_required(VERSION 3.0)

include(FeatureSummary)

add_subdirectory(src)


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
