cmake_minimum_required(VERSION 3.10)
project(NESulator)
set(CMAKE_C_STANDARD 11)
add_subdirectory(src)
add_subdirectory(test)
enable_testing()


#for SDL 2
include_directories(${PROJECT_SOURCE_DIR}/lib/SDL2/include)
link_directories(${PROJECT_SOURCE_DIR}/lib/SDL2/lib)

add_test(Testulator test/TestMain.c)