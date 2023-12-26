cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}")
cmake_policy(SET CMP0057 NEW)

project ("mod")
set(developer "user95401")
set(description "mod made for gd 2.2!!! asd")
set(lib_output "D:/Games/user666/2.2 PC GDPS but actually that is lol/fif issue/") # post build copy

#curly-eureka
set(GAME_VERSION "2.200")
include("C:\\Users\\Lenovo\\source\\curly-eureka\\CURLY-EUREKA-INCL.cmake")# "\" must be escaped by \\

#version
set(FILEVERSION "1, 0, 0, 0")
set(LegalCopyright "user666's original")
configure_file("Version.rc" "../../../src/Version.rc")

#mod
file(GLOB_RECURSE SRC
Src/*
Src/Layers/*
#${CURLY_EUREKA_DIR} to include other stuff
)
add_library(${PROJECT_NAME} SHARED ${SRC} ${CURLY_EUREKA_SRC})

#POST_BUILD
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND #POST_BUILD UPDATE USES POWERSHELL!
"${CMAKE_CURRENT_LIST_DIR}/CMakePostBuild.bat" ${developer} ${PROJECT_NAME} ${LIBRARY_OUTPUT_PATH} ${lib_output})
