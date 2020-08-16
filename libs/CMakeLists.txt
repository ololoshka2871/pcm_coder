
find_package(FFMPEG REQUIRED)

##############
# CLI11
##############

set(CLI11_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CLI11/include)

add_library(CLI11 INTERFACE)

target_include_directories(CLI11 INTERFACE ${CLI11_DIR})

##############
# FFmpegVideoReader
##############

set(FFmpegVideoReader_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pcm_utils/FFmpegVideoReader)

set(FFmpegVideoReader_SRC
    ${FFmpegVideoReader_DIR}/Context.cpp
    ${FFmpegVideoReader_DIR}/FFmpegVideoReader.cpp
    ${FFmpegVideoReader_DIR}/FFmpegVideoWriter.cpp
)

add_library(FFmpegVideoReader STATIC ${FFmpegVideoReader_SRC})

target_include_directories(FFmpegVideoReader
    PUBLIC
        ${FFMPEG_INCLUDE_DIRS}
        ${FFmpegVideoReader_DIR}
)

# message(STATUS ${FFMPEG_INCLUDE_DIRS})

target_link_libraries(FFmpegVideoReader
    PUBLIC
        ${FFMPEG_LIBRARIES}
)

##############
# PlayerWriter
##############

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

set(CMAKE_DEBUG_POSTFIX d)

set(DEPENDENCY_DIR ${CMAKE_CURRENT_BINARY_DIR}/dependencies)

include(get-sdl2)
include(get-portaudio)

ExternalProject_Get_Property(sdl2 install_dir)

set(sdl2_install_dir ${install_dir})

add_library(SDL2 STATIC IMPORTED)
set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION_RELEASE ${sdl2_install_dir}/lib/SDL2${CMAKE_STATIC_LIBRARY_SUFFIX})
set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION_DEBUG ${sdl2_install_dir}/lib/SDL2d${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(SDL2 sdl2)

ExternalProject_Get_Property(project_portaudio install_dir)

set(portaudio_install_dir ${install_dir})

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(portaudio_libName portaudio_static_x86${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
	set(portaudio_libName portaudio_static_x64${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

add_library(Portaudio STATIC IMPORTED)
set_target_properties(Portaudio PROPERTIES IMPORTED_LOCATION ${portaudio_install_dir}/lib/${portaudio_libName})
add_dependencies(Portaudio project_portaudio)

set(ADD_LIBS SDL2 Portaudio)
set(ADD_INCLUDES_PATHS 
	${sdl2_install_dir}/include
	${portaudio_install_dir}/include
)

set(SDL2_FOUND TRUE)
set(PORTAUDIO_FOUND TRUE)

else()

find_package(SDL2)
find_package(PORTAUDIO)

endif()

set(PlayerWriter_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pcm_utils/PlayerWriter)

if ((SDL2_FOUND) AND (PORTAUDIO_FOUND) AND (PORTAUDIO_VERSION EQUAL 19))
    set(ADD_LIBS ${PORTAUDIO_LIBRARIES})
    set(ADD_INCLUDES_PATHS ${PORTAUDIO_INCLUDE_DIRS})
    if (SDL2_LIBRARIES)
    	list(APPEND ADD_LIBS ${SDL2_LIBRARIES})
    	list(APPEND ADD_INCLUDES_PATHS ${SDL2_INCLUDE_DIRS})
    else()
    	list(APPEND ADD_LIBS SDL2::SDL2main SDL2::SDL2)
    endif()
    
    message(STATUS "Include: " ${ADD_INCLUDES_PATHS})
    
    set(PlayerWriter_SRC
        ${PlayerWriter_DIR}/player.cpp
    )

    add_library(PlayerWriter STATIC ${PlayerWriter_SRC})

    target_include_directories(PlayerWriter
        PUBLIC
            ${ADD_INCLUDES_PATHS}
            ${PlayerWriter_DIR}
    )

    target_link_libraries(PlayerWriter
        PUBLIC
            ${FFMPEG_LIBRARIES}
            ${ADD_LIBS}
    )
    target_compile_definitions(PlayerWriter PUBLIC -DPLAYER)
    set(USE_PLAYER TRUE PARENT_SCOPE)
else()
    message(STATUS "SDL2 or Portaudio not found. Display pcm not supported!")
    add_library(PlayerWriter INTERFACE)
    target_include_directories(PlayerWriter INTERFACE ${PlayerWriter_DIR})
    set(USE_PLAYER FALSE PARENT_SCOPE)
endif()

##############
# pcm_utils
##############

set(pcm_utils_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pcm_utils/pcm_utils)

set(pcm_utils_SRC
    ${pcm_utils_DIR}/PQ.cpp
    ${pcm_utils_DIR}/matrix_constants.cpp
    ${pcm_utils_DIR}/matrix_equations.cpp
    ${pcm_utils_DIR}/DataBlock.cpp
    ${pcm_utils_DIR}/Sample.cpp
    ${pcm_utils_DIR}/testlib.cpp
)

add_library(pcm_utils STATIC ${pcm_utils_SRC})

target_include_directories(pcm_utils PUBLIC ${pcm_utils_DIR})

##############
# progress-cpp
##############

add_subdirectory(progress-cpp)
