option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(BUILD_EXAMPLES "Build examples" OFF FORCE CACHE BOOL "Build examples")

add_subdirectory(SFML-2.6.1)

# pakker is made by me
add_subdirectory(Pakker)
# ECS is also made by me
add_subdirectory(ECS)