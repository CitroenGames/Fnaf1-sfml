option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(BUILD_EXAMPLES "Build examples" OFF FORCE CACHE BOOL "Build examples")

if(LINUX)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
endif()

if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL3-3.4.4")
    set(SDL3_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/SDL3-3.4.4")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL3_SDK_ARCH "x64")
    else()
        set(SDL3_SDK_ARCH "x86")
    endif()

    add_library(SDL3::SDL3 SHARED IMPORTED GLOBAL)
    set_target_properties(SDL3::SDL3 PROPERTIES
        IMPORTED_LOCATION "${SDL3_SDK_ROOT}/lib/${SDL3_SDK_ARCH}/SDL3.dll"
        IMPORTED_IMPLIB "${SDL3_SDK_ROOT}/lib/${SDL3_SDK_ARCH}/SDL3.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL3_SDK_ROOT}/include"
    )
else()
    find_package(SDL3 REQUIRED CONFIG)
endif()

set(MINIAUDIO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MINIAUDIO_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MINIAUDIO_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(MINIAUDIO_INSTALL OFF CACHE BOOL "" FORCE)
set(MINIAUDIO_NO_LIBVORBIS ON CACHE BOOL "" FORCE)
set(MINIAUDIO_NO_LIBOPUS ON CACHE BOOL "" FORCE)
add_subdirectory(miniaudio-0.11.25)

add_library(imgui STATIC
    imgui-1.92.7/imgui.cpp
    imgui-1.92.7/imgui_demo.cpp
    imgui-1.92.7/imgui_draw.cpp
    imgui-1.92.7/imgui_tables.cpp
    imgui-1.92.7/imgui_widgets.cpp
    imgui-1.92.7/backends/imgui_impl_sdl3.cpp
    imgui-1.92.7/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(imgui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.92.7"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.92.7/backends"
)
target_link_libraries(imgui PUBLIC SDL3::SDL3)


# add_subdirectory(box2d-main) # will add it back later :D (TM)

# Libraries created by me
add_subdirectory(Pakker)
add_subdirectory(ECS)
add_subdirectory(FunctionSpeaker)
