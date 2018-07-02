project(luajit)

file(GLOB SRCS "./src/*.c*")
file(GLOB INCS "./src/*.h*")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(luajit STATIC ${SRCS} ${INCS})

source_group("" FILES ${SRCS} ${INCS})
