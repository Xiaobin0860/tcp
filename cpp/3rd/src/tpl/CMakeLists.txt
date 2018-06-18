project(tpl)

if(WIN32)
set(tpl_SRCS
	tpl.cpp
	win/mmap.cpp
	win/nonempty.cpp
)
else()
set(tpl_SRCS
	tpl.cpp
)
endif()

add_library(tpl STATIC ${tpl_SRCS})
