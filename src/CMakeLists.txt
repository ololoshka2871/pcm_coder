
set(SRC
    main.cpp

    audioreader.h
    audioreader.cpp
    print_duration.h
    print_duration.cpp
    samplegenerator.h
    samplegenerator.cpp
    audiosample.h

    pcmframe14.h
    pcmframe14.cpp

    pcmline.h
    pcmline.cpp
    pcmlinegenerator.h
    pcmlinegenerator.cpp
    pcmframe.h
    pcmframe.cpp
    pcmfrmagemanager.h
    pcmfrmagemanager.cpp
    samplestairsitherator.h
    samplestairsitherator.cpp
    pcmencoder.h
    pcmencoder.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        CLI11
        FFmpegVideoReader
        PlayerWriter
        pcm_utils
        stdc++
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CPP_STD})
