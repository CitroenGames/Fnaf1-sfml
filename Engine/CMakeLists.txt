cmake_minimum_required(VERSION 3.12)

project(Engine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(deps)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "src/*.h"
    "src/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PUBLIC
    sfml-graphics
    sfml-audio
    ECS
    Pakker
)