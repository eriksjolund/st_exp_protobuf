cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(file_format_container)

add_library(${PROJECT_NAME}
  SHARED
    after_header_data.cc
)

find_package(Qt5Core)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core file_format_common_protobuf::file_format_common_protobuf)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})



