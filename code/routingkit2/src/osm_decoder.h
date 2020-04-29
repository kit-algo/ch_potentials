#ifndef ROUTING_KIT2_OSM_DECODER_H
#define ROUTING_KIT2_OSM_DECODER_H

#include "tag_map.h"
#include "osm_types.h"
#include "span.h"
#include "geo_pos.h"

#include <functional>
#include <stdint.h>
#include <string>
#include <vector>

namespace RoutingKit2{

void unordered_read_osm_pbf(
	const std::string&file_name,
	std::function<void(uint64_t osm_node_id, LatLon p, const TagMap&tags)>node_callback,
	std::function<void(uint64_t osm_way_id, Span<const uint64_t>osm_node_id_list, const TagMap&tags)>way_callback,
	std::function<void(uint64_t osm_relation_id, Span<const OSMRelationMember>member_list, const TagMap&tags)>relation_callback,
	std::function<void(std::string msg)>log_message = [](std::string){}
);

void ordered_read_osm_pbf(
	const std::string&file_name,
	std::function<void(uint64_t osm_node_id, LatLon p, const TagMap&tags)>node_callback,
	std::function<void(uint64_t osm_way_id, Span<const uint64_t>osm_node_id_list, const TagMap&tags)>way_callback,
	std::function<void(uint64_t osm_relation_id, Span<const OSMRelationMember>member_list, const TagMap&tags)>relation_callback,
	std::function<void(std::string msg)>log_message = [](std::string){},
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);

} // RoutingKit2

#endif
