
find_package(Boost REQUIRED filesystem program_options system)

include_directories(SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

add_executable(file2hex file2hex.cpp)

target_link_libraries(file2hex PRIVATE ${Boost_LIBRARIES})
