project(Glad)

add_library(glad STATIC)

target_include_directories(glad SYSTEM PUBLIC include)

target_sources(glad
    PRIVATE src/glad.c
    PRIVATE include/glad/glad.h
    PRIVATE include/KHR/khrplatform.h
)