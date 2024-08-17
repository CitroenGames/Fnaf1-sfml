cmake_minimum_required(VERSION 3.16)

project(Engine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(deps)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "src/*.h"
    "src/*.cpp"
)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    group_sources(ENGINE_SOURCES)
endif()

add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} PUBLIC
    sfml-graphics
    sfml-audio
    imgui
    ECS
    Pakker
    FunctionSpeaker
)