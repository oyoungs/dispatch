cmake_minimum_required(VERSION 2.8)
project(dispatch)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
)
link_directories(
        ${PROJECT_SOURCE_DIR}/lib
        /usr/local/lib
)

add_subdirectory(extlibs/gtest)


add_executable(any any_main.cc)
add_executable(dispatch dispatch_main.cc)
add_executable(cli cli_main.cc)
add_executable(optional optional_main.cc)
add_executable(gomain go_main.cc)
add_executable(format format_main.cc)
add_executable(emitter event_emitter_main.cc)
add_executable(property property_main.cc)
add_executable(number number_main.cc)
add_executable(defer defer_main.cc)
add_executable(locker locker_main.cc)
add_executable(range range_main.cc)
add_executable(string_view string_view_main.cc)
add_executable(topic topic_main.cc)

#add_executable(tcpserver tcp_server_main.cc)
add_executable(tcpclient tcp_client_main.cc)
#target_link_libraries(tcpserver pthread ev)
target_link_libraries(tcpclient
#        pthread
        )
#target_link_libraries(dispatch pthread ev)
target_link_libraries(gomain
#        pthread
        )
#target_link_libraries(cli pthread spdlog)
target_link_libraries(range
        gtest
        gtest_main
        )

target_link_libraries(defer
        gtest
        gtest_main
        )

target_link_libraries(format
        gtest
        gtest_main
        )

target_link_libraries(topic
        gtest
        gtest_main
        )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/oyoung
        DESTINATION include)
