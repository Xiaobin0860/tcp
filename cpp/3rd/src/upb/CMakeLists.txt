project(upb)

set(upb_SRCS
    descriptor.upb.c
    decode.c
    def.c
    encode.c
    handlers.c
    msg.c
    refcounted.c
    sink.c
    table.c
    upb.c
)
set(upb_descriptor_SRCS
    descriptor/descriptor.upbdefs.c
    descriptor/reader.c
)
set(upb_pb_SRCS
    pb/compile_decoder.c
    pb/decoder.c
    pb/encoder.c
    pb/glue.c
    pb/textprinter.c
    pb/varint.c
)
set(lua_SRCS
    bindings/lua/def.c
    bindings/lua/msg.c
    bindings/lua/upb.c
    bindings/lua/upb/pb.c
)

add_library(upb STATIC ${upb_SRCS} ${upb_descriptor_SRCS} ${upb_pb_SRCS} ${lua_SRCS})
