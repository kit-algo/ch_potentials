#ifndef ROUTING_KIT2_OSM_TYPES_H
#define ROUTING_KIT2_OSM_TYPES_H

namespace RoutingKit2{

enum class OSMIDType{
	node,
	way,
	relation
};

struct OSMRelationMember{
	OSMIDType type;
	uint64_t id;
	const char*role;
};

enum class OSMWayDirectionCategory{
	open_in_both,
	only_open_forwards,
	only_open_backwards,
	closed
};

} // RoutingKit2

#endif
