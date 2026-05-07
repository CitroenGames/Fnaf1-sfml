cmake_minimum_required(VERSION 3.16)
project(Fnaf-SFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE GAMESOURCES src/*.cpp src/*.h src/*.hpp)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    group_sources(GAMESOURCES)
endif()
add_executable(${PROJECT_NAME} ${GAMESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine)

# Link Windows debugging libraries for crash handler
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE dbghelp)
endif()

# Handle Assets directory differently depending on platform
if(WIN32)
    if(TARGET SDL3::SDL3)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E rm -f
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/openal32.dll
            COMMENT "Copying SDL3 runtime to build location"
            VERBATIM
        )
    endif()

    # On Windows, copy the assets
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/Assets
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets
        COMMENT "Copying Assets directory to build location"
        VERBATIM
    )
else()
    # On Linux or macOS, create a symbolic link instead
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_CURRENT_SOURCE_DIR}/Assets
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets
        COMMENT "Symlinking Assets directory to build location"
        VERBATIM
    )
endif()

