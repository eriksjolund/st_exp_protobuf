cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(transform_point_with_alignment_matrix)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

add_library(${PROJECT_NAME}
  SHARED
  transform_point.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt5::Core
  Qt5::Gui
  common_typedefs::common_typedefs
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
