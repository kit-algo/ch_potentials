cmake_minimum_required (VERSION 3.15)

project (OSMImport)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-stringop-truncation")
add_link_options("-lz")

find_package (OpenMP REQUIRED)
add_custom_target(routingkit COMMAND make WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../RoutingKit)

add_executable (import_osm src/bin/import_osm.cpp)
add_executable (latlon_box_around_node src/bin/latlon_box_around_node.cpp)
add_executable (make_bfs_node_seq src/bin/make_bfs_node_seq.cpp)
add_executable (render_node_sequence src/bin/render_node_sequence.cpp)

target_include_directories (import_osm PRIVATE ../RoutingKit/include)
target_include_directories (latlon_box_around_node PRIVATE ../RoutingKit/include)
target_include_directories (make_bfs_node_seq PRIVATE ../RoutingKit/include)
target_include_directories (render_node_sequence PRIVATE ../RoutingKit/include)
target_link_libraries (import_osm ${CMAKE_SOURCE_DIR}/../RoutingKit/lib/libroutingkit.a OpenMP::OpenMP_CXX)
target_link_libraries (latlon_box_around_node ${CMAKE_SOURCE_DIR}/../RoutingKit/lib/libroutingkit.a OpenMP::OpenMP_CXX)
target_link_libraries (make_bfs_node_seq ${CMAKE_SOURCE_DIR}/../RoutingKit/lib/libroutingkit.a OpenMP::OpenMP_CXX)
target_link_libraries (render_node_sequence ${CMAKE_SOURCE_DIR}/../RoutingKit/lib/libroutingkit.a OpenMP::OpenMP_CXX)
add_dependencies (import_osm routingkit)
add_dependencies (latlon_box_around_node routingkit)
add_dependencies (make_bfs_node_seq routingkit)
add_dependencies (render_node_sequence routingkit)
