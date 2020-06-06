
set(SRC
    main.cpp

    audioreader.h
    audioreader.cpp
    print_duration.h
    print_duration.cpp
    samplegenerator.h
    samplegenerator.cpp
    audiosample.h
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        CLI11
        FFmpegVideoReader
        PlayerWriter
        stdc++
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CPP_STD})
