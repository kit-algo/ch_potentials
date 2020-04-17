cmake_minimum_required (VERSION 3.15)

project (ComputeCH)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-stringop-truncation")
add_link_options("-lz")

find_package (OpenMP REQUIRED)
add_custom_target(routingkit COMMAND make WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../RoutingKit)

add_executable (compute_ch src/bin/compute_contraction_hierarchy_and_order.cpp)

target_include_directories (compute_ch PRIVATE ../RoutingKit/include)
target_link_libraries (compute_ch ${CMAKE_SOURCE_DIR}/../RoutingKit/lib/libroutingkit.a OpenMP::OpenMP_CXX)
add_dependencies (compute_ch routingkit)
