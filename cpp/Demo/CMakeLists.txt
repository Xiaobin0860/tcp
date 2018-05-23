
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
