cmake_minimum_required(VERSION 3.10)
project(ECS VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

option(ECS_BUILD_EXAMPLES "Build examples" OFF)

add_subdirectory(deps)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json)

if(ECS_BUILD_EXAMPLES)
# Example executable
file(GLOB EXAMPLE_SOURCES "example/*.cpp" "example/*.h")
add_executable(ECSExample ${EXAMPLE_SOURCES})
target_link_libraries(ECSExample PRIVATE ${PROJECT_NAME})
endif()