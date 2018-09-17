project(dispatch)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)


add_executable(tcp tcp_main.cc)

add_executable(any any_main.cc)

add_executable(dispatch dispatch_main.cc)


target_link_libraries(tcp pthread ev)
target_link_libraries(dispatch pthread ev)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
