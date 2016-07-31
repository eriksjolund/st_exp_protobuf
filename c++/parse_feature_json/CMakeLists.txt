cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(parse_feature_json)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Yajl REQUIRED)
find_package(Qt5Core)
add_library(${PROJECT_NAME}
  SHARED
    json_parse.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC  ${YAJL_INCLUDE_DIRS} ${YAJL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC ${YAJL_LIBRARIES} feature_datamodel::feature_datamodel PRIVATE Qt5::Core)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
