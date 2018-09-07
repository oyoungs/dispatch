project(dispatch)
cmake_minimum_required(VERSION 2.8)

add_compile_options(-std=c++11 )

include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} pthread ev)

