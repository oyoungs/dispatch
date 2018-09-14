project(dispatch)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(DISPATCH_SRC main.cc)

add_executable(dispatch ${DISPATCH_SRC})
target_link_libraries(dispatch pthread ev)

set(ANY_SRC any_main.cc)
add_executable(any ${ANY_SRC})
target_link_libraries(any pthread ev)


install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
