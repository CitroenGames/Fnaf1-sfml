option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(BUILD_EXAMPLES "Build examples" OFF FORCE CACHE BOOL "Build examples")

add_subdirectory(SFML-2.6.2)
add_subdirectory(imgui)

# Libraries created by me
add_subdirectory(Pakker)
add_subdirectory(ECS)
add_subdirectory(FunctionSpeaker)