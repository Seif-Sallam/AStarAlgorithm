cmake_minimum_required(VERSION 3.16)

FILE(GLOB SOURCES ./*.cpp)
FILE(GLOB HEADERS ./*.h)

add_library(
    imgui
    STATIC
    ${SOURCES}
    ${HEADERS}
)

set(OS_LIBRARIES
    opengl32
)

if (UNIX)
    set(OS_LIBRARIES
        GL
    )
endif()

target_link_libraries(imgui
    PRIVATE
    ${OS_LIBRARIES}
    sfml-graphics
    sfml-window
    sfml-system
)