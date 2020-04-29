#ifndef ROUTING_KIT2_OSM_IMPORT_H
#define ROUTING_KIT2_OSM_IMPORT_H

#include "map.h"

#include <string>

namespace RoutingKit2{

VecOSMCarRoads import_car_roads_from_osm_pbf_file(const std::string&file_name, std::vector<uint64_t>extra_node_osm_id, std::function<void(std::string)>log_message);

} // RoutingKit2

#endif
