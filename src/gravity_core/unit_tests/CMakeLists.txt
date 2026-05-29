
find_package(OpenLibrary 2.2 QUIET)

if(OpenLibrary_FOUND)
    include(${OPENLIBRARY_USE_FILE})
endif()

find_package(GTest REQUIRED)

set(SRC
        accelerators_tests.cpp
)

add_executable(accelerators_tests ${SRC})

target_link_libraries(accelerators_tests
                        PRIVATE
                            GTest::gmock_main
                            ${ACC_LINKER_FLAGS}
                            gravity_core
)

target_include_directories(accelerators_tests
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src
)


if(COMMAND enableCodeCoverage)
    enableCodeCoverage(accelerators_tests)
endif()

add_test(accelerators accelerators_tests)
