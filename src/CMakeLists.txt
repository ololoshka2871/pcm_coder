
set(SRC
    main.cpp

    audioreader.h
    audioreader.cpp
    print_duration.h
    print_duration.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        CLI11
        FFmpegVideoReader
        stdc++
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CPP_STD})
