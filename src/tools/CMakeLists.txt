
find_package(Boost REQUIRED program_options)

include_directories(SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

add_executable(file2hex file2hex.cpp)

target_link_libraries(file2hex PRIVATE ${Boost_LIBRARIES})
