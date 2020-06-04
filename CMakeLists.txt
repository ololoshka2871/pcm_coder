cmake_minimum_required(VERSION 3.17)

set(CPP_STD 17)

project(pcm_coder LANGUAGES CXX)

# set(CMAKE_CXX_LINK_EXECUTABLE ${CMAKE_CXX_COMPILER})

add_subdirectory(libs)
add_subdirectory(src)


