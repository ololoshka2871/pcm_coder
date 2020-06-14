set(SRC
    main.cpp

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

    pixeldublicator.h
    pixeldublicator.cpp
)

if (USE_PLAYER)
list(APPEND SRC
    sdl2display.h
    sdl2display.cpp
)
endif()

add_executable(${PROJECT_NAME} ${SRC})


target_link_libraries(${PROJECT_NAME}
    PUBLIC
        CLI11
        FFmpegVideoReader
        PlayerWriter
        pcm_utils
        progresscpp
)


if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            stdc++
    )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CPP_STD})

install(TARGETS ${PROJECT_NAME})

add_custom_target(run ${CMAKE_CURENT_BINARY_DIR}/${PROJECT_NAME})
