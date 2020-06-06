
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

add_library(FFmpegVideoReader ${FFmpegVideoReader_SRC})

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

add_library(PlayerWriter ${PlayerWriter_SRC})

target_include_directories(PlayerWriter
    PUBLIC
        ${PlayerWriter_DIR}
)

target_link_libraries(PlayerWriter
    PUBLIC
        ${FFMPEG_LIBRARIES}
        ${PORTAUDIO_LIBRARIES}
)
