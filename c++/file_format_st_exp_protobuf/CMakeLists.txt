cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(file_format_st_exp_protobuf)

add_library(${PROJECT_NAME}
 SHARED
  file_format_st_exp_protobuf.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})


