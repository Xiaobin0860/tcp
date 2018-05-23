project(Base)

file(GLOB SRCS "./*.cpp")
file(GLOB INCS "./*.h*")

add_library(Base STATIC ${SRCS} ${INCS})
source_group("" FILES ${SRCS} ${INCS})
