cmake_minimum_required(VERSION 3.17)

set(CPP_STD 17)

project(pcm_coder)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_subdirectory(libs)
add_subdirectory(src)


