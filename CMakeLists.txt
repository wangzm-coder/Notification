cmake_minimum_required(VERSION 3.5) # CMake install : https://cmake.org/download/
project(Notification LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED) # Qt COMPONENTS


set(SOURCE_FILE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE ALL_FILES "${SOURCE_FILE_DIR}/*")


source_group("" FILES ${ALL_FILES})

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(${PROJECT_NAME} WIN32 ${ALL_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)