
project(Demo)

#---------------------------- TcpServer --------------------------------
add_executable(TcpServer ./TcpServer.cpp)

if(WIN32)

target_link_libraries(TcpServer
    Base
)

else()

target_link_libraries(TcpServer
    Base
    boost_system
)

endif()
source_group("" FILES "TcpServer.cpp")
#-----------------------------------------------------------------------

#---------------------------- TcpClient --------------------------------
add_executable(TcpClient ./TcpClient.cpp)

if(WIN32)

target_link_libraries(TcpClient
    Base
)

else()

target_link_libraries(TcpClient
    Base
    boost_system
)

endif()
source_group("" FILES "TcpClient.cpp")
#-----------------------------------------------------------------------

#---------------------------- TestLua --------------------------------
set(TestLua_files
    ./TestLua.cpp
    ./addressbook.pb.cc
    ./addressbook.pb.h
    ./demo_types.cpp
    ./demo_types.h

)
add_executable(TestLua ${TestLua_files})

if(WIN32)

target_link_libraries(TestLua
    Base
    lua
    libprotobuf
    gtest
    zlibstatic
    upb
    thrift
    LuaIntf
    pbc
    luapb
)

else()

target_link_libraries(TestLua
    Base
    lua
    libprotobuf
    gtest
    z
    upb
    thrift
    LuaIntf
    pbc
    luapb
)

endif()

source_group("" FILES ${TestLua_files})
#-----------------------------------------------------------------------
