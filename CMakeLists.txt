cmake_minimum_required(VERSION 3.16)

set(CPP_STD 17)

project(pcm_coder)

set(MODULES_DIR ${CMAKE_SOURCE_DIR}/cmake)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULES_DIR})

add_subdirectory(libs)
add_subdirectory(src)


