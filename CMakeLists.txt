cmake_minimum_required(VERSION 3.5.1)

project(dfw-jumpstart)

set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

#TODO: Should also allow for c++14
add_compile_options(-std=c++11)

#TODO: Add the rest of flags...

#TODO: This should be given in the command line
include_directories(../libdansdl2/)
include_directories(../libdansdl2-tools/)
include_directories(../log/)
include_directories(../dfw/)
include_directories(../tools/)

add_subdirectory("${PROJECT_SOURCE_DIR}/class")

#TODO: Find package???

#libraries...
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


#target_link_libraries(dansdl2 SDL2)
#target_link_libraries(dansdl2 SDL2_ttf)
#target_link_libraries(dansdl2 SDL2_image)
#target_link_libraries(dansdl2 SDL2_mixer)
#target_link_libraries(dansdl2 SDL2_image)

#target_link_libraries(dfw log)
#target_link_libraries(dfw tools)
#target_link_libraries(dfw dansdl2)
#target_link_libraries(dfw dansdl2-tools)

add_executable(dfw-jumpstart main.cpp ${SOURCE})
target_link_libraries(dfw-jumpstart dfw log tools dansdl2 dansdl2-tools SDL2 SDL2_ttf SDL2_mixer SDL2_image GL)
