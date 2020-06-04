
##############
# CLI11
##############

set(CLI11_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CLI11/include)

add_library(CLI11 INTERFACE)

target_include_directories(CLI11 INTERFACE ${CLI11_DIR})

