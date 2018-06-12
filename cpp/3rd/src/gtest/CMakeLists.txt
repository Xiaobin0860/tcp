project(gtest)
if(WIN32)
else()
add_definitions(
    --std=c++11
)
endif()
add_library(gtest STATIC ./gtest-all.cc)
