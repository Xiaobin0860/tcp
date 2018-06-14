project(luasocket)

file(GLOB INCS "./*.h")

if(WIN32)
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
    luabpack.cpp
    wsocket.cpp
)
else()
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
    luabpack.cpp
    usocket.cpp
)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(luasocket STATIC ${SRCS} ${INCS})
source_group("" FILES ${SRCS} ${INCS})
