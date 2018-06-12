project(lua)

file(GLOB SRCS "./*.c")
file(GLOB INCS "./*.h")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(lua STATIC ${SRCS} ${INCS})

source_group("" FILES ${SRCS} ${INCS})
