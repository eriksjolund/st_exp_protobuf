cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(reorder_feature_datamodel)

add_library(${PROJECT_NAME}
  SHARED
    reorder.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC  feature_datamodel::feature_datamodel)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
