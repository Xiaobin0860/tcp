project(Base)

file(GLOB SRCS "./*.cpp")
file(GLOB INCS "./*.h*")

if(WIN32)
else()
add_definitions(
    --std=c++11
)
endif()

add_library(Base STATIC ${SRCS} ${INCS})
source_group("" FILES ${SRCS} ${INCS})
