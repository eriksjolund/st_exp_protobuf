cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(crick_reader)
find_package(Qt5Core)
find_package(Qt5Gui) # for zipreader
find_package(Qt5Xml)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/crick_reader.cc.cmake ${CMAKE_CURRENT_BINARY_DIR}/crick_reader.cc)


add_library(${PROJECT_NAME}
  SHARED
    ${CMAKE_CURRENT_BINARY_DIR}/crick_reader.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC parse_feature_json::parse_feature_json feature_datamodel::feature_datamodel dzi_helper::dzi_helper vips_tiles::vips_tiles crick_reader_interface::crick_reader_interface PRIVATE Qt5::Core Qt5::Gui Qt5::Xml)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
