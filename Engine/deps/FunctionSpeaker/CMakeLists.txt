cmake_minimum_required(VERSION 3.5)
project(FunctionSpeaker VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

option(BUILD_EXAMPLES "Build examples" ON)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(BUILD_EXAMPLES)
# Example executable
file(GLOB EXAMPLE_SOURCES "example/*.cpp" "example/*.h")
add_executable(FunctionSpeakerExample ${EXAMPLE_SOURCES})
target_link_libraries(FunctionSpeakerExample PRIVATE ${PROJECT_NAME})
endif()