
find_package(SDL2)

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
    AbastractPCMFinalStage.h
    AbastractPCMFinalStage.cpp

    ffmpegvideocoder.h
    ffmpegvideocoder.cpp
)

if (SDL2_FOUND)
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
        stdc++
)

if (SDL2_FOUND)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            CLI11
            FFmpegVideoReader
            PlayerWriter
            pcm_utils
            SDL2::SDL2main SDL2::SDL2
            stdc++
    )
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            -DSDL2
    )
else()
    message(STATUS "SDL2 not found display pcm not supported!")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CPP_STD})



