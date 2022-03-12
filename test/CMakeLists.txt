set(CMAKE_CXX_COMPILER /usr/local/bin/g++)

project(test)

include_directories(../include)
link_directories(../linux)

include_directories(../3rd/the_essence_of_computing/tools)
link_directories(../3rd/tools_lib)


add_executable(net_server_main  net_server_main.cpp)
target_link_libraries(net_server_main  net_tools tools)