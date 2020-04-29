#include "map.h"
#include "prefix_sum.h"
#include "polyline.h"
#include <string.h>
#include <iostream>
#include <algorithm>

using namespace std;

namespace RoutingKit2{

	bool is_dlink_path(ConstRefLinkEnds map, Span<const uint32_t>path){
		for(size_t i=1; i<path.size(); ++i)
			if(dlink_head(map, path[i-1]) != dlink_tail(map, path[i]))
				return false;
		return true;
	}

	VecLinkEndsAdjArray build_adj_array(ConstRefLinkEnds map){
		assert_link_ends_valid(map);

		VecLinkEndsAdjArray adj(map.node_count, map.link_count);

		for(uint32_t x=0; x<=map.node_count; ++x)
			adj.first_outgoing_dlink_index_of_node[x] = 0;

		for(uint32_t l=0; l<map.link_count; ++l)
			++adj.first_outgoing_dlink_index_of_node[map.link_tail[l]];

		for(uint32_t l=0; l<map.link_count; ++l)
			++adj.first_outgoing_dlink_index_of_node[map.link_head[l]];

		compute_prefix_sum(adj.first_outgoing_dlink_index_of_node_as_ref());

		for(uint32_t l=0; l<map.link_count; ++l){
			adj.outgoing_dlink[adj.first_outgoing_dlink_index_of_node[map.link_tail[l]]++] = link_to_forward_dlink(l);
			adj.outgoing_dlink[adj.first_outgoing_dlink_index_of_node[map.link_head[l]]++] = link_to_backward_dlink(l);
		}

		for(uint32_t x=map.node_count; x != 0; --x)
			adj.first_outgoing_dlink_index_of_node[x] = adj.first_outgoing_dlink_index_of_node[x-1];
		adj.first_outgoing_dlink_index_of_node[0] = 0;

		adj.assert_correct_size();
		assert_link_ends_and_adj_array_consistent(map, adj.as_cref());

		return adj;
	}

	namespace {
		template<class A>
		void my_assert(bool cond, std::string msg, const A&my_abort){
			if(!cond)
				my_abort(msg);
		}

		void abort_with_msg(std::string msg){
			std::cerr << std::move(msg) << std::endl;
			abort();
		}

		void throw_with_msg(std::string msg){
			throw std::runtime_error(std::move(msg));
		}

		template<class A>
		void check_link_ends(ConstRefLinkEnds map, const A&my_abort){
			for(uint32_t l=0; l<map.link_count; ++l){
				my_assert(
					map.link_head[l] < map.node_count,
					"link "+std::to_string(l)+" has an out of bounds head "+std::to_string(map.link_head[l]),
					my_abort
				);
				my_assert(
					map.link_tail[l] < map.node_count,
					"link "+std::to_string(l)+" has an out of bounds tail "+std::to_string(map.link_tail[l]),
					my_abort
				);

			}
		}


		template<class A>
		void check_link_ends_adj_array(ConstRefLinkEndsAdjArray adj, const A&my_abort){
			my_assert(
				adj.first_outgoing_dlink_index_of_node_as_cref().front() == 0,
				"first element of first_outgoing_dlink_index_of_node must be 0",
				my_abort
			);
			my_assert(
				adj.first_outgoing_dlink_index_of_node_as_cref().back() == 2*adj.link_count,
				"last element of first_outgoing_dlink_index_of_node must be 2*link_count",
				my_abort
			);
			my_assert(
				std::is_sorted(adj.first_outgoing_dlink_index_of_node_as_cref().begin(), adj.first_outgoing_dlink_index_of_node_as_cref().end()),
				"first_outgoing_dlink_index_of_node must be sorted",
				my_abort
			);
		}

		template<class A>
		void check_link_ends_and_adj_array_consistent(ConstRefLinkEnds map, ConstRefLinkEndsAdjArray adj, const A&my_abort){
			check_link_ends(map, my_abort);
			check_link_ends_adj_array(adj, my_abort);
			my_assert(
				map.node_count == adj.node_count,
				"node_count must be the same",
				my_abort
			);
			my_assert(
				map.link_count == adj.link_count,
				"link_count must be the same",
				my_abort
			);
			for(uint32_t x=0; x<map.node_count; ++x){
				for(uint32_t i=adj.first_outgoing_dlink_index_of_node[x]; i!=adj.first_outgoing_dlink_index_of_node[x+1]; ++i){
					my_assert(
						dlink_tail(map, adj.outgoing_dlink[i]) == x,
						"node " +std::to_string(x)+" has dlink "+std::to_string(adj.outgoing_dlink[i])+" as outgoing dlink but the dlink's tail does not match",
						my_abort
					);
				}
			}
		}

		template<class A>
		void check_link_shapes(ConstRefLinkShapes map, const A&my_abort){
			check_link_ends(map, my_abort);

			for(uint32_t x=0; x<map.node_count; ++x){
				my_assert(
					map.node_pos[x] != invalid_lat_lon,
					"node "+std::to_string(x)+" has an invalid node pos",
					my_abort
				);
			}

			my_assert(
				map.first_shape_pos_of_link_as_cref().front() == 0,
				"first element of first_shape_pos_of_link must be 0",
				my_abort
			);
			my_assert(
				map.first_shape_pos_of_link_as_cref().back() == map.shape_pos_count,
				"last element of first_shape_pos_of_link must be shape_pos_count",
				my_abort
			);
			my_assert(
				std::is_sorted(map.first_shape_pos_of_link_as_cref().begin(), map.first_shape_pos_of_link_as_cref().end()),
				"first_shape_pos_of_link must be sorted",
				my_abort
			);

			for(uint32_t l=0; l<map.link_count; ++l){
				uint32_t length_in_cm = 0;
				LatLon prev = map.node_pos[map.link_tail[l]];
				for(uint s=map.first_shape_pos_of_link[l]; s!=map.first_shape_pos_of_link[l+1]; ++s){
					LatLon now = map.shape_pos[s];
					my_assert(
						now != invalid_lat_lon,
						"link "+std::to_string(l)+" has an invalid shape pos",
						my_abort
					);
					length_in_cm += compute_distance_in_cm(GeoPos(prev), GeoPos(now));
					prev = now;
				}
				length_in_cm += compute_distance_in_cm(GeoPos(prev), GeoPos(map.node_pos[map.link_head[l]]));
				my_assert(
					map.link_length_in_cm[l] == length_in_cm,
					"link "+std::to_string(l)+" has a length that is inconsistent with geometry. link_length_in_cm is "+std::to_string(map.link_length_in_cm[l])+" cm but geometry is "+std::to_string(length_in_cm)+" cm long.",
					my_abort
				);
			}
		}

		template<class A>
		void check_car_roads(ConstRefCarRoads map, const A&my_abort){
			check_link_shapes(map, my_abort);

			my_assert(
				map.first_dlink_of_forbidden_maneuver_as_cref().front() == 0,
				"first element of first_dlink_of_forbidden_maneuver must be 0",
				my_abort
			);
			my_assert(
				map.first_dlink_of_forbidden_maneuver_as_cref().back() == map.forbidden_maneuver_dlink_count,
				"last element of first_shape_pos_of_link must be the forbidden_maneuver_dlink_count",
				my_abort
			);
			my_assert(
				std::is_sorted(map.first_dlink_of_forbidden_maneuver_as_cref().begin(), map.first_dlink_of_forbidden_maneuver_as_cref().end()),
				"first_dlink_of_forbidden_maneuver must be sorted",
				my_abort
			);

			for(uint32_t m=0; m<map.forbidden_maneuver_count; ++m){
				my_assert(
					is_dlink_path(map, {
						map.forbidden_maneuver_dlink + map.first_dlink_of_forbidden_maneuver[m],
						map.forbidden_maneuver_dlink + map.first_dlink_of_forbidden_maneuver[m+1]
					}),
					"forbidden maneuver "+std::to_string(m)+" is not a valid path",
					my_abort
				);
			}
		}

		template<class A>
		void check_osm_car_roads(ConstRefOSMCarRoads map, const A&my_abort){
			check_car_roads(map, my_abort);

			my_assert(
				map.first_link_of_way_as_cref().front() == 0,
				"first element of first_link_of_way must be 0",
				my_abort
			);
			my_assert(
				map.first_link_of_way_as_cref().back() == map.link_count,
				"last element of first_link_of_way must be link_count",
				my_abort
			);
			my_assert(
				std::is_sorted(map.first_link_of_way_as_cref().begin(), map.first_link_of_way_as_cref().end()),
				"first_link_of_way must be sorted",
				my_abort
			);


			my_assert(
				std::is_sorted(map.node_osm_id, map.node_osm_id + map.node_count),
				"relative OSM node ID order should be preserved",
				my_abort
			);
			my_assert(
				std::is_sorted(map.way_osm_id, map.way_osm_id + map.way_count),
				"relative OSM way ID order should be preserved",
				my_abort
			);
			my_assert(
				std::is_sorted(map.forbidden_maneuver_osm_id, map.forbidden_maneuver_osm_id + map.forbidden_maneuver_count),
				"relative OSM forbidden maneuver ID order should be preserved",
				my_abort
			);
		}
	}

	void throw_if_link_ends_invalid(ConstRefLinkEnds map){
		check_link_ends(map, throw_with_msg);
	}

	void assert_link_ends_valid(ConstRefLinkEnds map){
		#ifndef NDEBUG
		check_link_ends(map, abort_with_msg);
		#endif
	}


	void throw_if_link_ends_adj_array_invalid(ConstRefLinkEndsAdjArray map){
		check_link_ends_adj_array(map, throw_with_msg);
	}

	void assert_link_ends_adj_array_valid(ConstRefLinkEndsAdjArray map){
		#ifndef NDEBUG
		check_link_ends_adj_array(map, abort_with_msg);
		#endif
	}



	void throw_if_link_ends_and_adj_array_inconsistent(ConstRefLinkEnds map, ConstRefLinkEndsAdjArray adj){
		check_link_ends_and_adj_array_consistent(map, adj, throw_with_msg);
	}

	void assert_link_ends_and_adj_array_consistent(ConstRefLinkEnds map, ConstRefLinkEndsAdjArray adj){
		#ifndef NDEBUG
		check_link_ends_and_adj_array_consistent(map, adj, abort_with_msg);
		#endif
	}


	void throw_if_link_shapes_invalid(ConstRefLinkShapes map){
		check_link_shapes(map, throw_with_msg);
	}

	void assert_link_shapes_valid(ConstRefLinkShapes map){
		#ifndef NDEBUG
		check_link_shapes(map, abort_with_msg);
		#endif
	}

	void throw_if_car_roads_invalid(ConstRefCarRoads map){
		check_car_roads(map, throw_with_msg);
	}

	void assert_car_roads_valid(ConstRefCarRoads map){
		#ifndef NDEBUG
		check_car_roads(map, abort_with_msg);
		#endif
	}

	void throw_if_osm_car_roads_invalid(ConstRefOSMCarRoads map){
		check_osm_car_roads(map, throw_with_msg);
	}

	void assert_osm_car_roads_valid(ConstRefOSMCarRoads map){
		#ifndef NDEBUG
		check_osm_car_roads(map, abort_with_msg);
		#endif
	}

	GeoPosOffsetAndDistance find_closest_point_offset_and_distance_on_dlink(ConstRefLinkShapes map, uint32_t dlink_id, GeoPos point)noexcept{
		assert(dlink_id < 2*map.link_count);
		assert(point != invalid_geo_pos);

		DLinkPointEnumerator enumerator(map, dlink_id);
		int32_t best_offset = 0;
		int64_t best_distance_in_sqr_cm = std::numeric_limits<int64_t>::max();
		GeoPos best_point = invalid_geo_pos;

		int32_t prev_offset = 0;
		GeoPos prev(*enumerator.next());
		while(auto now_lat_lon = enumerator.next()){
			GeoPos now(*now_lat_lon);
			GeoPos candidate = find_closest_point_on_segment(point, prev, now);

			int64_t candidate_distance_in_sqr_cm = compute_distance_in_sqr_cm(point, candidate);

			if(candidate_distance_in_sqr_cm < best_distance_in_sqr_cm){
				best_point = candidate;
				best_distance_in_sqr_cm = candidate_distance_in_sqr_cm;
				best_offset = prev_offset + compute_distance_in_cm(prev, candidate);
			}

			prev_offset += compute_distance_in_cm(now, prev);
			prev = now;
		};

		GeoPosOffsetAndDistance ret;
		ret.offset_in_cm = best_offset;
		ret.distance_in_sqr_cm = best_distance_in_sqr_cm;
		ret.pos = best_point;
		return ret;
	}

}
