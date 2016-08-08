
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Qt5Widgets REQUIRED)
find_package(OpenMP)

include_directories(SYSTEM ${Qt5_Widgets})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_cpp(moc_wrapped
             main_window.hpp
             simulation_controller.hpp
)

qt5_wrap_ui(ui_wrapped main_window.ui)

add_executable(gravity_simulator
               main.cpp
               main_window.cpp
               main_window.hpp
               objects_scene.cpp
               objects_scene.hpp
               objects_view.cpp
               objects_view.hpp
               simulation_controller.cpp
               simulation_controller.hpp
               simulation_engine.cpp
               simulation_engine.hpp
               simulation_info_widget.cpp
               simulation_info_widget.hpp
               accelerators/iaccelerator.hpp
               accelerators/openmp_accelerator.cpp
               accelerators/openmp_accelerator.hpp
               ${moc_wrapped}
               ${ui_wrapped}
)

target_link_libraries(gravity_simulator Qt5::Widgets ${OpenMP_CXX_FLAGS})
target_compile_options(gravity_simulator PRIVATE ${OpenMP_CXX_FLAGS})
