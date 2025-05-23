cmake_minimum_required(VERSION 3.5.1)

project(project-placeholder)

#version
set(MAJOR_VERSION 0)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)

option(BUILD_DEBUG "Build a debug release" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(${BUILD_DEBUG})
	set(CMAKE_BUILD_TYPE Debug)
else()
	set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.22.0")

	add_definitions(-DMAJOR_VERSION=${MAJOR_VERSION})
	add_definitions(-DMINOR_VERSION=${MINOR_VERSION})
	add_definitions(-DPATCH_VERSION=${PATCH_VERSION})
	add_definitions(-DRELEASE_VERSION=\"${RELEASE_VERSION}\")
else()

	add_compile_definitions(MAJOR_VERSION=${MAJOR_VERSION})
	add_compile_definitions(MINOR_VERSION=${MINOR_VERSION})
	add_compile_definitions(PATCH_VERSION=${PATCH_VERSION})
	add_compile_definitions(RELEASE_VERSION=\"${RELEASE_VERSION}\")
endif()


add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

#TODO: Should also allow for c++14

#In a system where the libs are not installed, add them.
#include_directories(../libdansdl2/include)
#include_directories(../libdansdl2-tools/include)
#include_directories(../log/include)
#include_directories(../dfw/include)
#include_directories(../tools/include)

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

#add the framework dependencies
add_library(lm STATIC IMPORTED)
add_library(tools STATIC IMPORTED)
add_library(ldtools STATIC IMPORTED)
add_library(dansdl2 STATIC IMPORTED)
add_library(dfw STATIC IMPORTED)

#If the libraries are not installed, tell them where they are.
set_target_properties(tools PROPERTIES IMPORTED_LOCATION /usr/local/lib/libtools.a)
set_target_properties(lm PROPERTIES IMPORTED_LOCATION /usr/local/lib/liblm.a)
set_target_properties(dansdl2 PROPERTIES IMPORTED_LOCATION /usr/local/lib/libdansdl2.a)
set_target_properties(ldtools PROPERTIES IMPORTED_LOCATION /usr/local/lib/libldtools.a)
set_target_properties(dfw PROPERTIES IMPORTED_LOCATION /usr/local/lib/libdfw.a)

#Configure the executable, we want it into the "build directory" and we want all resources copied
#to the build directory.
add_executable(project-placeholder main.cpp ${SOURCE})
set_target_properties(project-placeholder PROPERTIES RUNTIME_OUTPUT_DIRECTORY build)
add_custom_command(TARGET project-placeholder POST_BUILD COMMAND cp -r ../resources/* ./build)
add_custom_command(TARGET project-placeholder POST_BUILD COMMAND mkdir ./build/logs)
target_link_libraries(project-placeholder dfw tools dansdl2 ldtools lm SDL2 SDL2_ttf SDL2_mixer SDL2_image GL)
