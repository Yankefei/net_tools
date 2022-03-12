set(CMAKE_CXX_COMPILER /usr/local/bin/g++)
project(net_tools)

include_directories(.)
include_directories(../include)
include_directories(../3rd/the_essence_of_computing/tools)
link_directories(../3rd/tools_lib)

aux_source_directory(. SRCS)

add_library(net_tools STATIC ${SRCS})
target_link_libraries(net_tools  tools)
target_link_libraries(net_tools  pthread)
