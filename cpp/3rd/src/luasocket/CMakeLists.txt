project(luasocket)

file(GLOB INCS "./*.h")

set(SRCS
    auxiliar.cpp
    buffer.cpp
    except.cpp
    inet.cpp
    io.cpp
    luasocket.cpp
    options.cpp
    select.cpp
    tcp.cpp
    timeout.cpp
    udp.cpp
    wsocket.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(luasocket STATIC ${SRCS} ${INCS})
source_group("" FILES ${SRCS} ${INCS})
