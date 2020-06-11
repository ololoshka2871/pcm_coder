
find_package(FFmpeg REQUIRED)
find_package(Portaudio REQUIRED)

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
        ${FFmpegVideoReader_DIR}
)

target_link_libraries(FFmpegVideoReader
    PUBLIC
        ${FFMPEG_LIBRARIES}
)

##############
# PlayerWriter
##############

set(PlayerWriter_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pcm_utils/PlayerWriter)

set(PlayerWriter_SRC
    ${PlayerWriter_DIR}/player.cpp
)

add_library(PlayerWriter STATIC ${PlayerWriter_SRC})

target_include_directories(PlayerWriter
    PUBLIC
        ${PlayerWriter_DIR}
)

target_link_libraries(PlayerWriter
    PUBLIC
        ${FFMPEG_LIBRARIES}
        ${PORTAUDIO_LIBRARIES}
)

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
