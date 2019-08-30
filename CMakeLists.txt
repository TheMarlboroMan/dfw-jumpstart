cmake_minimum_required(VERSION 3.5.1)

project(dfw-jumpstart)

set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

#TODO: Should also allow for c++14
add_compile_options(-std=c++11)

#TODO: Add the rest of flags...

#TODO: The ../ should be given in the command line
include_directories(../libdansdl2/)
include_directories(../libdansdl2-tools/)
include_directories(../log/)
include_directories(../dfw/)
include_directories(../tools/)

add_subdirectory("${PROJECT_SOURCE_DIR}/class")

#libraries... added manually. Sorry, that's life.
add_library(log STATIC IMPORTED)
set_target_properties(log PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../log/build/liblog.a)


add_library(tools STATIC IMPORTED)
set_target_properties(tools PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../tools/build/libtools.a)

add_library(dansdl2 STATIC IMPORTED)
set_target_properties(dansdl2 PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../libdansdl2/build/libdansdl2.a)

add_library(dansdl2-tools STATIC IMPORTED)
set_target_properties(dansdl2-tools PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../libdansdl2-tools/build/libdansdl2-tools.a)

add_library(dfw STATIC IMPORTED)
set_target_properties(dfw PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../dfw/build/libdfw.a)

add_executable(dfw-jumpstart main.cpp ${SOURCE})


target_link_libraries(dfw-jumpstart dfw log tools dansdl2 dansdl2-tools SDL2 SDL2_ttf SDL2_mixer SDL2_image GL)
