
find_package(OpenLibrary 2.2 REQUIRED)

include(${OPENLIBRARY_USE_FILE})

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

set(SRC
        accelerators_tests.cpp
    )

add_executable(accelerators_tests ${SRC})

target_link_libraries(accelerators_tests
                        PRIVATE
                            ${GMOCK_MAIN_LIBRARY}
                            ${GMOCK_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(accelerators_tests
                            SYSTEM PRIVATE
                                ${GMOCK_INCLUDE_DIRS}
                                ${GTEST_INCLUDE_DIRS}
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src
)


enableCodeCoverage(accelerators_tests)

add_test(accelerators accelerators_tests)
