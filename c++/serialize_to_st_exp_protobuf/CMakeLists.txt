cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(serialize_to_st_exp_protobuf)

add_library(${PROJECT_NAME}
  SHARED
   serialize_to_st_exp_protobuf.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC feature_datamodel::feature_datamodel st_exp_protobuf::st_exp_protobuf reorder_feature_datamodel::reorder_feature_datamodel crick_reader_interface::crick_reader_interface file_format_st_exp_protobuf::file_format_st_exp_protobuf PRIVATE dzi_helper::dzi_helper transform_point_with_alignment_matrix::transform_point_with_alignment_matrix file_format_container::file_format_container  tiles_spec_interface::tiles_spec_interface tiles_interface::tiles_interface)



set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
