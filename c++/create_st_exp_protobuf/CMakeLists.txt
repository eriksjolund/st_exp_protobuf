cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(create_st_exp_protobuf)

find_package(Qt5Core) # for the command line parsing
add_executable(${PROJECT_NAME} parse_args.cc main.cc vips_tiles_container.cc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
target_link_libraries(${PROJECT_NAME} dataset_reader::dataset_reader feature_datamodel::feature_datamodel dzi_helper::dzi_helper Qt5::Core serialize_to_st_exp_protobuf::serialize_to_st_exp_protobuf  tiles_interface::tiles_interface  vips_tiles::vips_tiles file_format_st_exp_protobuf::file_format_st_exp_protobuf )
