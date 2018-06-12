project(luapb)

file(GLOB SRCS "./*.cc")
file(GLOB INCS "./*.h")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(luapb STATIC ${SRCS} ${INCS})

source_group("" FILES ${SRCS} ${INCS})