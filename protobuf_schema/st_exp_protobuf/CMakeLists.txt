cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(st_exp_protobuf)

find_package(Protobuf REQUIRED)

set(PROTOBUF_IMPORT_DIRS ${CMAKE_SOURCE_DIR}/protobuf_schema/file_format_common_protobuf ${CMAKE_CURRENT_SOURCE_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS st_exp.proto tiled_image.proto fullsize_image.proto)

add_library(${PROJECT_NAME}
  SHARED
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

get_filename_component(parent_binary_dir ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS} ${parent_binary_dir})

target_link_libraries(${PROJECT_NAME} PUBLIC ${PROTOBUF_LITE_LIBRARIES} ${PROTOBUF_LIBRARIES} file_format_common_protobuf::file_format_common_protobuf)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
