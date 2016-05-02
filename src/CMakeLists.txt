
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

add_executable(gravity_simulator
               main.cpp
               simulation_engine.cpp
               simulation_engine.hpp
)
