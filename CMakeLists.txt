cmake_minimum_required(VERSION 3.5.0)
project(Shrekrooms VERSION 1.0.0)

set(ENABLE_CONSOLE FALSE)

find_package(OpenGL REQUIRED)

file(
    GLOB SOURCES
    src/*.h
    src/*.c
    src/*.hpp
    src/*.cpp
)

add_compile_options("-g")

if(ENABLE_CONSOLE)
    add_executable(${PROJECT_NAME} ${SOURCES})
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
endif()

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE dependencies
    ${CMAKE_SOURCE_DIR}/../.public-include
)

target_link_libraries(
    ${PROJECT_NAME}
    
    ${CMAKE_SOURCE_DIR}/dependencies/libglfw3.a
    ${CMAKE_SOURCE_DIR}/dependencies/freetype.lib
    ${CMAKE_SOURCE_DIR}/dependencies/glad.a
    OpenGL::GL
    
    -static
)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Shrekrooms")
