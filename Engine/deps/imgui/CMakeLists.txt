cmake_minimum_required(VERSION 3.10)
project(imgui)

add_library(imgui)
target_sources(${PROJECT_NAME} PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_demo.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_draw.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_tables.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_widgets.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/backends/imgui_impl_opengl3.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/multiPlot.cpp"
 )

target_include_directories(${PROJECT_NAME} PUBLIC src)