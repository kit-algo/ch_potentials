cmake_minimum_required (VERSION 3.15)

project (OSMImport)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-stringop-truncation")
add_link_options("-lz")

find_package (OpenMP REQUIRED)
add_custom_target(routingkit COMMAND make WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../RoutingKit)

add_executable (import_osm src/bin/import_osm.cpp)

target_include_directories (import_osm PRIVATE ../RoutingKit/include)
target_link_libraries (import_osm ${CMAKE_SOURCE_DIR}/../RoutingKit/lib/libroutingkit.a OpenMP::OpenMP_CXX)
add_dependencies (import_osm routingkit)
