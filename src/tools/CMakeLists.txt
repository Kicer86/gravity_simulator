
find_package(Boost REQUIRED COMPONENTS filesystem program_options)

add_executable(file2hex file2hex.cpp)

target_link_libraries(file2hex
                      PRIVATE
                        Boost::filesystem
                        Boost::program_options
)
