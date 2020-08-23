set(SRC
    audioreader.h
    audioreader.cpp
    print_duration.h
    print_duration.cpp
    samplegenerator.h
    samplegenerator.cpp
    audiosample.h

    pcmline.h
    pcmline.cpp
    pcmlinegenerator.h
    pcmlinegenerator.cpp
    pcmframe.h
    pcmframe.cpp
    frame.h
    frame.cpp
    pcmfrmagemanager.h
    pcmfrmagemanager.cpp
    samplestairsitherator.h
    samplestairsitherator.cpp
    PCMFinalStage.h
    PCMFinalStage.cpp

    ffmpegvideocoder.h
    ffmpegvideocoder.cpp

    ipcmframeprocessingpolicy.h
    iframe.h

    frameextender.h
    frameextender.cpp

    argparser.h
    argparser.cpp

    IConsumer.h
    result.h
    abstractprocessingstage.h
    samplespack.h

    AudioProdusser.h
    AudioProdusser.cpp

    BitWidthConverter.h
    BitWidthConverter.cpp

    ctrlc_listener.h
    ctrlc_listener.cpp

    LineGeneratorStage.h
    LineGeneratorStage.cpp

    PCMFrmageStage.h
    PCMFrmageStage.cpp

    PixelDuplicatorStage.h
    PixelDuplicatorStage.cpp

    FrameProxy.h
    FrameProxy.cpp

    SDL2DisplayConsumerBase.h
    SDL2DisplayConsumerBase.cpp

    SDL2DisplayConsumer.h
    SDL2DisplayConsumer.cpp

    RPIFbDisplayConsumer.h
    RPIFbDisplayConsumer.cpp

    FFmpegVideoCoderConsumer.h
    FFmpegVideoCoderConsumer.cpp

    Splitter.h

    PlayerConsumer.h
    PlayerConsumer.cpp
)

if (USE_PLAYER)
list(APPEND SRC
    sdl2display.h
    sdl2display.cpp
)
endif()

add_library(common STATIC ${SRC})

target_link_libraries(common
    PUBLIC
        CLI11
        FFmpegVideoReader
        PlayerWriter
        pcm_utils
        progresscpp
)


if (NOT WIN32)
    target_link_libraries(common
        PUBLIC
            stdc++
            pthread
    )
endif()

set_property(TARGET common PROPERTY CXX_STANDARD ${CPP_STD})

add_executable(${PROJECT_NAME}-mt main.cpp)

target_link_libraries(${PROJECT_NAME}-mt common)

add_executable(${PROJECT_NAME}-st main-st.cpp)

target_link_libraries(${PROJECT_NAME}-st common)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
set_target_properties(${PROJECT_NAME}-mt PROPERTIES LINK_FLAGS "/SAFESEH:NO")
set_target_properties(${PROJECT_NAME}-st PROPERTIES LINK_FLAGS "/SAFESEH:NO")
endif()

set_property(TARGET ${PROJECT_NAME}-mt PROPERTY CXX_STANDARD ${CPP_STD})
set_property(TARGET ${PROJECT_NAME}-st PROPERTY CXX_STANDARD ${CPP_STD})

install(TARGETS ${PROJECT_NAME}-mt ${PROJECT_NAME}-st)

add_custom_target(run ${CMAKE_CURENT_BINARY_DIR}/${PROJECT_NAME}-mt)
