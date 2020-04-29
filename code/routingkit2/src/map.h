#ifndef ROUTING_KIT2_MAP_H
#define ROUTING_KIT2_MAP_H

#include "map_schema.h"
#include "span.h"
#include <stdlib.h>
#include <vector>

namespace RoutingKit2{

	constexpr uint32_t link_to_forward_dlink(uint32_t link){
		return link << 1;
	}

	constexpr uint32_t link_to_backward_dlink(uint32_t link){
		return (link << 1) | 1;
	}

	constexpr uint32_t dlink_to_link(uint32_t dlink){
		return dlink >> 1;
	}

	constexpr uint32_t reverse_dlink(uint32_t dlink){
		return dlink ^ 1;
	}

	constexpr bool is_forward_dlink(uint32_t dlink_id){
		return !(dlink_id & 1);
	}

	constexpr bool is_backward_dlink(uint32_t dlink_id){
		return dlink_id & 1;
	}

	inline uint32_t dlink_head(ConstRefLinkEnds map, uint32_t dlink_id){
		const uint32_t*arr[2] = {map.link_head, map.link_tail};
		return arr[is_backward_dlink(dlink_id)][dlink_to_link(dlink_id)];
	}

	inline uint32_t dlink_tail(ConstRefLinkEnds map, uint32_t dlink_id){
		const uint32_t*arr[2] = {map.link_tail, map.link_head};
		return arr[is_backward_dlink(dlink_id)][dlink_to_link(dlink_id)];
	}

	bool is_dlink_path(ConstRefLinkEnds map, Span<const uint32_t>path);

	VecLinkEndsAdjArray build_adj_array(ConstRefLinkEnds map);

	void assert_link_ends_valid(ConstRefLinkEnds map);
	void throw_if_link_ends_invalid(ConstRefLinkEnds map);

	void assert_link_ends_adj_array_valid(ConstRefLinkEndsAdjArray map);
	void throw_if_link_ends_adj_array_invalid(ConstRefLinkEndsAdjArray map);

	void throw_if_link_ends_and_adj_array_inconsistent(ConstRefLinkEnds map, ConstRefLinkEndsAdjArray adj);
	void assert_link_ends_and_adj_array_consistent(ConstRefLinkEnds map, ConstRefLinkEndsAdjArray adj);

	void assert_link_shapes_valid(ConstRefLinkShapes map);
	void throw_if_link_shapes_invalid(ConstRefLinkShapes map);

	void assert_car_roads_valid(ConstRefCarRoads map);
	void throw_if_car_roads_invalid(ConstRefCarRoads map);

	void assert_osm_car_roads_valid(ConstRefOSMCarRoads map);
	void throw_if_osm_car_roads_invalid(ConstRefOSMCarRoads map);


	struct GeoPosOffsetAndDistance{
		GeoPos pos;
		uint32_t offset_in_cm;
		uint64_t distance_in_sqr_cm;
	};

	GeoPosOffsetAndDistance find_closest_point_offset_and_distance_on_dlink(ConstRefLinkShapes map, uint32_t dlink_id, GeoPos p)noexcept;


	struct GeoPosAndDistance{
		GeoPos pos;
		uint64_t distance_in_sqr_cm;
	};

	inline GeoPosAndDistance find_closest_point_and_distance_on_link(ConstRefLinkShapes map, uint32_t link_id, GeoPos p)noexcept{
		auto q = find_closest_point_offset_and_distance_on_dlink(map, link_to_forward_dlink(link_id), p);
		GeoPosAndDistance r;
		r.pos = q.pos;
		r.distance_in_sqr_cm = q.distance_in_sqr_cm;
		return r;
	}

}

#endif
