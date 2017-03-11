
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

include(FeatureSummary)

find_package(Qt5Widgets REQUIRED)

include_directories(SYSTEM
                    ${Qt5_Widgets}
)

include_directories(${PROJECT_SOURCE_DIR}/src/gravity_core)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_cpp(moc_wrapped
             main_window.hpp
             objects_scene.hpp
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
               simulation_info_widget.cpp
               simulation_info_widget.hpp
               ${moc_wrapped}
               ${ui_wrapped}
               $<TARGET_OBJECTS:accelerators>
)

target_link_libraries(gravity_simulator
                      PRIVATE
                        Qt5::Widgets
                        gravity_core
)
