cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(vips_tiles)
find_package(Qt5Core)
find_package(Qt5Xml)
find_package(Qt5Gui)

find_program(vips_prog vips)

if (NOT vips_prog)
  message(FATAl_ERROR "Could not find the program vips")
endif()

add_library(${PROJECT_NAME}
  SHARED
    vips_tiles.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC dzi_helper::dzi_helper tiles_spec_interface::tiles_spec_interface tiles_interface::tiles_interface PRIVATE Qt5::Core Qt5::Xml Qt5::Gui)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
