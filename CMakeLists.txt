project(dispatch)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(${PROJECT_SOURCE_DIR}/include)


add_executable(tcpserver tcp_server_main.cc)
add_executable(tcpclient tcp_client_main.cc)

add_executable(any any_main.cc)

add_executable(dispatch dispatch_main.cc)
add_executable(cli cli_main.cc)
add_executable(optional optional_main.cc)
add_executable(gomain go_main.cc)
add_executable(format format_main.cc)
add_executable(emitter event_emitter_main.cc)
add_executable(property property_main.cc)
add_executable(number number_main.cc)


target_link_libraries(tcpserver pthread ev)
target_link_libraries(tcpclient pthread)
target_link_libraries(dispatch pthread ev)
target_link_libraries(gomain pthread)
#target_link_libraries(cli pthread spdlog)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
