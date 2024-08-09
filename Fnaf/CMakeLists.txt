cmake_minimum_required(VERSION 3.16)
project(Fnaf-SFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE GameSources src/**.cpp src/**.h src/**.hpp)
add_executable(${PROJECT_NAME} ${GameSources})

target_include_directories(${PROJECT_NAME} PRIVATE src engine)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-audio Engine)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM)
endif()
