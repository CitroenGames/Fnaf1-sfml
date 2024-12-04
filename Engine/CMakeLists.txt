cmake_minimum_required(VERSION 3.16)

project(Engine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(deps)
if(LINUX)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
endif()

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
    sfml-system 
    sfml-window 
    sfml-graphics 
    sfml-network 
    sfml-audio
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    imgui
    ECS
    Pakker
    FunctionSpeaker
)