add_compile_definitions(GEODE_EXPOSE_SECRET_INTERNALS_IN_HEADERS_DO_NOT_DEFINE_PLEASE)
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

project("main")

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

# my bindings... https://github.com/user95401/bindings
include("$ENV{GEODE_SDK}/cmake/CPM.cmake")
CPMAddPackage(NAME "bindings"
	GITHUB_REPOSITORY "user95401/bindings"
	GIT_TAG "main"
	DOWNLOAD_ONLY YES
	NO_CACHE YES
)

set(GEODE_BINDINGS_REPO_PATH ${bindings_SOURCE_DIR})

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

#mod

file(GLOB_RECURSE SRC src/*.c*)
add_library(${PROJECT_NAME} SHARED ${SRC})
message("SRC: ${SRC}")

include_directories(src)

setup_geode_mod(${PROJECT_NAME})
