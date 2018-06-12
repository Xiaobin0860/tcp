
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)
file(GLOB THRIFT_SRCS "./thrift/*.cpp")
file(GLOB THRIFT_INCS "./thrift/*.h")
file(GLOB ASYNC_SRCS "./thrift/async/*.cpp")
file(GLOB ASYNC_INCS "./thrift/async/*.h")
file(GLOB CURRENCY_SRCS "./thrift/concurrency/*.cpp")
file(GLOB CURRENCY_INCS "./thrift/concurrency/*.h")
file(GLOB PROCE_SRCS "./thrift/processor/*.cpp")
file(GLOB PROCE_INCS "./thrift/processor/*.h")
file(GLOB PROTO_SRCS "./thrift/protocol/*.cpp")
file(GLOB PROTO_INCS "./thrift/protocol/*.h")
file(GLOB SERV_SRCS "./thrift/server/*.cpp")
file(GLOB SERV_INCS "./thrift/server/*.h")
file(GLOB TRANS_SRCS "./thrift/transport/*.cpp")
file(GLOB TRANS_INCS "./thrift/transport/*.h")
file(GLOB WIN_SRCS "./thrift/windows/*.cpp")
file(GLOB WIN_INCS "./thrift/windows/*.h")

if(WIN32)
add_definitions(
    /D HAVE_CONFIG_H=1
    /D _LIB
    #/D HAVE_STDINT_H
    /D THRIFT_NO_CONTENTION_PROFILING
)
add_library(thrift STATIC
    ${THRIFT_SRCS}
    ${THRIFT_INCS}
    ${ASYNC_SRCS}
    ${ASYNC_INCS}
    ${CURRENCY_SRCS}
    ${CURRENCY_INCS}
    ${PROCE_SRCS}
    ${PROCE_INCS}
    ${PROTO_SRCS}
    ${PROTO_INCS}
    ${TRANS_SRCS}
    ${TRANS_INCS}
    ${SERV_SRCS}
    ${SERV_INCS}
    ${WIN_SRCS}
    ${WIN_INCS}
)
else()
add_definitions(
    --std=c++11
)
add_library(thrift STATIC
    ${THRIFT_SRCS}
    ${THRIFT_INCS}
    ${ASYNC_SRCS}
    ${ASYNC_INCS}
    ${CURRENCY_SRCS}
    ${CURRENCY_INCS}
    ${PROCE_SRCS}
    ${PROCE_INCS}
    ${PROTO_SRCS}
    ${PROTO_INCS}
    ${TRANS_SRCS}
    ${TRANS_INCS}
    ${SERV_SRCS}
    ${SERV_INCS}
)
endif()
source_group("" FILES ${THRIFT_SRCS} ${THRIFT_INCS})
source_group("transport" FILES ${TRANS_SRCS} ${TRANS_INCS})
source_group("server" FILES ${SERV_SRCS} ${SERV_INCS})
source_group("protocol" FILES ${PROTO_SRCS} ${PROTO_INCS})
source_group("processor" FILES ${PROCE_SRCS} ${PROCE_INCS})
source_group("concurrency" FILES ${CURRENCY_SRCS} ${CURRENCY_INCS})
if(WIN32)
source_group("windows" FILES ${WIN_SRCS} ${WIN_INCS})
endif()

