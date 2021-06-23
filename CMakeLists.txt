cmake_minimum_required(VERSION 3.5.1)
project(dfw-jumpstart)

option(BUILD_WITH_DEBUG "Adds debug code" OFF)
option(BUILD_DEBUG "Build a debug release" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(${BUILD_DEBUG})
        set(CMAKE_BUILD_TYPE Debug)

else()
        set(CMAKE_BUILD_TYPE Release)
endif()


add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)


#In a system where the libs are not installed, add them.
if(WIN32)
	message("WINDOWS BUILD")
	include_directories(../libdansdl2/include)
	include_directories(../libdansdl2-tools/include)
	include_directories(../log/include)
	include_directories(../dfw/include)
	include_directories(../tools/include)
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

if(${BUILD_WITH_DEBUG})
	add_compile_definitions(WDEBUG_CODE)
endif()


#libraries... added manually. Sorry, that's life.
add_library(lm STATIC IMPORTED)
add_library(tools STATIC IMPORTED)
add_library(ldtools STATIC IMPORTED)
add_library(dansdl2 STATIC IMPORTED)
add_library(dfw STATIC IMPORTED)

#If the libraries are not installed, tell them where they are.
if(WIN32)
	set_target_properties(tools PROPERTIES IMPORTED_LOCATION ../../tools/build/libtools.a)
	set_target_properties(lm PROPERTIES IMPORTED_LOCATION ../../log/build/liblm.a)
	set_target_properties(dansdl2 PROPERTIES IMPORTED_LOCATION ../../libdansdl2/build/libdansdl2.a)
	set_target_properties(ldtools PROPERTIES IMPORTED_LOCATION ../../libdansdl2-tools/build/libldtools.a)
	set_target_properties(dfw PROPERTIES IMPORTED_LOCATION ../../dfw/build/libdfw.a)
else()
        set_target_properties(tools PROPERTIES IMPORTED_LOCATION /usr/local/lib/libtools.a)
        set_target_properties(lm PROPERTIES IMPORTED_LOCATION /usr/local/lib/liblm.a)
        set_target_properties(dansdl2 PROPERTIES IMPORTED_LOCATION /usr/local/lib/libdansdl2.a)
        set_target_properties(ldtools PROPERTIES IMPORTED_LOCATION /usr/local/lib/libldtools.a)
        set_target_properties(dfw PROPERTIES IMPORTED_LOCATION /usr/local/lib/libdfw.a)
endif()

#Configure the executable, we want it into the "build directory" and we want all resources copied
#to the build directory.
add_executable(dfw-jumpstart main.cpp ${SOURCE})
set_target_properties(dfw-jumpstart PROPERTIES RUNTIME_OUTPUT_DIRECTORY build)
add_custom_command(TARGET dfw-jumpstart POST_BUILD COMMAND cp -r ../resources/* ./build)

if(WIN32)

	target_link_libraries(dfw-jumpstart dfw tools dansdl2 ldtools lm SDL2 SDL2_ttf SDL2_mixer SDL2_image glew32 glu32 opengl32 stdc++fs)
	target_link_options(dfw-jumpstart PRIVATE -lmingw32 -lSDL2main -lSDL2 -mwindows)
else()

	target_link_libraries(dfw-jumpstart dfw tools dansdl2 ldtools lm SDL2 SDL2_ttf SDL2_mixer SDL2_image GL)
endif()
