option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(BUILD_EXAMPLES "Build examples" OFF FORCE CACHE BOOL "Build examples")

# First, try to find the system-installed SFML
find_package(SFML 2.6 COMPONENTS graphics window system QUIET)

if(LINUX)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
endif()

# If system SFML is not found, use the local directory
if(NOT SFML_FOUND)
    # Check if the local SFML directory exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SFML")
        add_subdirectory(SFML)
        set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system)
        set(SFML_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/include)
    else()
        message(FATAL_ERROR "SFML not found. Please install SFML or provide the SFML-2.6.2 directory.")
    endif()
endif()

set(MINIAUDIO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MINIAUDIO_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MINIAUDIO_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(MINIAUDIO_INSTALL OFF CACHE BOOL "" FORCE)
set(MINIAUDIO_NO_LIBVORBIS ON CACHE BOOL "" FORCE)
set(MINIAUDIO_NO_LIBOPUS ON CACHE BOOL "" FORCE)
add_subdirectory(miniaudio-0.11.25)

add_subdirectory(imgui)


# add_subdirectory(box2d-main) # will add it back later :D (TM)

# Libraries created by me
add_subdirectory(Pakker)
add_subdirectory(ECS)
add_subdirectory(FunctionSpeaker)
