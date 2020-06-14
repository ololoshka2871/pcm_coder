#------------------------------------------------------------------------------
# External Project:     SDL2
# Downloads:            https://libsdl.org/release/
#------------------------------------------------------------------------------

include(ExternalProject)

set(SDL2_VERSION "2.0.12")

set(ARCHIVE https://www.libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz)

set(DEPENDENCY_DOWNLOAD_DIR ${DEPENDENCY_DIR}/download)
set(DEPENDENCY_EXTRACT_DIR ${DEPENDENCY_DIR}/extract)
set(DEPENDENCY_INSTALL_DIR ${DEPENDENCY_DIR}/install)

if(MINGW) # download pre-compiled SDL2 dev package for MinGW 32/64-bit

ExternalProject_Add(sdl2
  PATCH_COMMAND ${CMAKE_COMMAND}  -D__TARGET=${DEPENDENCY_EXTRACT_DIR}/src/sdl2 -P ${MODULES_DIR}/fix_sdl2_release.cmake
  PREFIX ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
  URL ${ARCHIVE}
  INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
  CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
  #CONFIGURE_COMMAND ""
  #BUILD_COMMAND ""
  #INSTALL_COMMAND 
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2/lib     ${DEPENDENCY_INSTALL_DIR}/lib
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
)

endif(MINGW)

if(MSVC) # download pre-compiled SDL2 dev package for Visual C++ 32/64-bit

ExternalProject_Add(sdl2
  PATCH_COMMAND ${CMAKE_COMMAND} -D__TARGET=${DEPENDENCY_EXTRACT_DIR}/src/sdl2 -P ${MODULES_DIR}/fix_sdl2_release.cmake
  PREFIX ${DEPENDENCY_EXTRACT_DIR}
  DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR}
  URL ${ARCHIVE}
  INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
  CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}
  #CONFIGURE_COMMAND ""
  #BUILD_COMMAND ""
  #INSTALL_COMMAND 
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2/lib     ${DEPENDENCY_INSTALL_DIR}/lib
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/sdl2/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
)

endif(MSVC)

set(DEPENDENCY_INSTALL_DIR ${DEPENDENCY_INSTALL_DIR} PARENT_SCOPE)

