set(SRC
    ipcmframeprocessingpolicy.h
    iframe.h
    Splitter.h
    IConsumer.h
    result.h
    abstractprocessingstage.h
    samplespack.h
    audiosample.h

    audioreader.h
    audioreader.cpp

    print_duration.h
    print_duration.cpp

    samplegenerator.h
    samplegenerator.cpp

    pcmline.h
    pcmline.cpp

    pcmframe.h
    pcmframe.cpp

    frame.h
    frame.cpp

    samplestairsitherator.h
    samplestairsitherator.cpp

    ffmpegvideocoder.h
    ffmpegvideocoder.cpp

    argparser.h
    argparser.cpp

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

    FFmpegVideoCoderConsumer.h
    FFmpegVideoCoderConsumer.cpp
)

add_library(common STATIC ${SRC})
set_property(TARGET common PROPERTY CXX_STANDARD ${CPP_STD})
target_link_libraries(common
    PUBLIC
        CLI11
        FFmpegVideoReader
        PlayerWriter
        pcm_utils
        progresscpp
)

if (USE_PLAYER)
    # https://github.com/zanac/OpenPB4/blob/master/RetroPie/EmulationStation/CMakeLists.txt
    if(EXISTS "/opt/vc/include/bcm_host.h")
        # ref: https://github.com/ali1234/raspi-teletext
        add_executable(rpi-fb-shifter rpi_fb_shifter.cpp)

        set_property(TARGET rpi-fb-shifter PROPERTY CXX_STANDARD ${CPP_STD})
        target_link_libraries(rpi-fb-shifter
            PUBLIC
                CLI11
        )
        install(TARGETS rpi-fb-shifter)

        SET(RPI ON)
    endif()

    set(PLAY_LIN_SRC
        PlayerConsumer.h
        PlayerConsumer.cpp

        SDL2DisplayConsumerBase.h
        SDL2DisplayConsumerBase.cpp

        SDL2DisplayConsumer.h
        SDL2DisplayConsumer.cpp
    )

    if (RPI)
        list(APPEND PLAY_LIN_SRC
            RPIFbDisplayConsumer.h
            RPIFbDisplayConsumer.cpp
        )
    endif()

    add_library(playlib STATIC ${PLAY_LIN_SRC})

    target_link_libraries(playlib
        PUBLIC
            PlayerWriter
            pcm_utils
    )

    if (RPI)
        target_compile_definitions(playlib
            PUBLIC
                -DRPI
        )
        target_compile_definitions(common
            PUBLIC
                -DRPI
        )
        target_include_directories(playlib
            PRIVATE
                "/opt/vc/include"
        )
        target_link_libraries(playlib
            PUBLIC
                -L/opt/vc/lib/ -lbcm_host
        )
    endif()

    set_property(TARGET playlib PROPERTY CXX_STANDARD ${CPP_STD})

endif()

add_executable(${PROJECT_NAME} main-st.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC common)

if (USE_PLAYER)
    target_link_libraries(${PROJECT_NAME} PUBLIC playlib)
endif()

if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            stdc++
            pthread
    )
endif()


#if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:playlib /NODEFAULTLIB:common")
#endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SAFESEH:NO")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CPP_STD})

install(TARGETS ${PROJECT_NAME})

add_custom_target(run ${CMAKE_CURENT_BINARY_DIR}/${PROJECT_NAME})
