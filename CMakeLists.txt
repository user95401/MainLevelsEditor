cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

project(MainLevelsEditor VERSION 3.0.0)

#mod

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_LIST_DIR}/_Src/*.cpp)
include_directories(_Headers)
if (WIN32)
    file(GLOB_RECURSE SRC_WIN32 ${CMAKE_CURRENT_LIST_DIR}/_Src/win32/*.cpp)
    include_directories(_Headers/win32)
endif()

add_library(${PROJECT_NAME} SHARED ${SRC} ${SRC_WIN32})
message("SRC: ${SRC}")

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
