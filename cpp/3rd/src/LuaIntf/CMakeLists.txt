
file(GLOB LuaIntf_SRCS "./src/*.cpp")

add_library(LuaIntf STATIC ${LuaIntf_SRCS})
