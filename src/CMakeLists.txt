
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

include(FeatureSummary)

add_subdirectory(accelerators)

find_package(Qt5Widgets REQUIRED)
find_package(OpenCL REQUIRED)
find_package(Boost REQUIRED)

include_directories(SYSTEM
                    ${Qt5_Widgets}
                    ${OpenCL_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    )

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
               object.cpp
               object.hpp
               objects.cpp
               objects.hpp
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
               accelerators/opencl_accelerator.cpp
               accelerators/opencl_accelerator.hpp
               ${moc_wrapped}
               ${ui_wrapped}
               $<TARGET_OBJECTS:accelerators>
)

target_link_libraries(gravity_simulator
                      PRIVATE
                        Qt5::Widgets
                        ${ACC_LINKER_FLAGS}
                        ${OpenCL_LIBRARIES}
)

feature_summary(WHAT ALL)
