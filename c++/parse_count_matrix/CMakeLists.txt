cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(parse_count_matrix)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


find_package(Qt5Core)
add_library(${PROJECT_NAME}
  SHARED
    parse_count_matrix.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC feature_datamodel::feature_datamodel PRIVATE Qt5::Core)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
