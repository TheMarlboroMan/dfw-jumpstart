cmake_minimum_required(VERSION 3.5.1)

project(dfw-jumpstart)

set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

#TODO: Should also allow for c++14
add_compile_options(-std=c++11)

#TODO: Add the rest of flags...

#TODO: The ../ should be given in the command line
include_directories(../libdansdl2/include)
include_directories(../libdansdl2-tools/include)
include_directories(../log/include)
include_directories(../dfw/include)
include_directories(../tools/include)

add_subdirectory("${PROJECT_SOURCE_DIR}/class")

#libraries... added manually. Sorry, that's life.
add_library(lm STATIC IMPORTED)
set_target_properties(lm PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../log/build/liblm.a)

add_library(tools STATIC IMPORTED)
set_target_properties(tools PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../tools/build/libtools.a)

add_library(dansdl2 STATIC IMPORTED)
set_target_properties(dansdl2 PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../libdansdl2/build/libdansdl2.a)

add_library(ldtools STATIC IMPORTED)
set_target_properties(ldtools PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../libdansdl2-tools/build/libldtools.a)

add_library(dfw STATIC IMPORTED)
set_target_properties(dfw PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../dfw/build/libdfw.a)

add_executable(dfw-jumpstart main.cpp ${SOURCE})


target_link_libraries(dfw-jumpstart dfw tools dansdl2 ldtools lm SDL2 SDL2_ttf SDL2_mixer SDL2_image GL)
