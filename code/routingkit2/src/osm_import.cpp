#include "osm_import.h"
#include "osm_decoder.h"
#include "osm_profile.h"
#include "permutation.h"
#include "sort.h"
#include "prefix_sum.h"
#include "inverse_func.h"
#include "str.h"
#include "turn.h"
#include "gpoly.h"
#include "map.h"
#include "polyline.h"
#include <algorithm>

#include <iostream>
using namespace std;

namespace RoutingKit2{

namespace {
	constexpr uint32_t invalid_id = (uint32_t)-1;

	enum class ManeuverType{
		forbidden,
		mandatory
	};

	struct Maneuver{
		uint64_t osm_relation_id;
		uint32_t from_way;
		uint32_t via_node;
		std::vector<uint32_t>via_way_list;
		uint32_t to_way;
		TurnDir dir;
		ManeuverType type;
	};

	template<class F>
	void forall_nodes_of_way(ConstRefOSMCarRoads map, uint32_t way_id, const F&f){
		uint32_t begin = map.first_link_of_way[way_id];
		uint32_t end = map.first_link_of_way[way_id+1];
		if(begin != end){
			if(!f(map.link_tail[begin]))
				return;
			for(uint32_t l=begin; l<end; ++l)
				if(!f(map.link_head[l]))
					break;
		}
	}

	struct ManeuverMatching{
		std::vector<uint32_t>first_dlink_of_maneuver;
		std::vector<uint32_t>maneuver_dlink;
		std::vector<ManeuverType>maneuver_type;
		std::vector<uint64_t>maneuver_osm_id;
	};

	struct ManeuverMatcher{
		ConstRefOSMCarRoads map;
		std::vector<bool>node_flag1, node_flag2;
		const std::function<void(std::string)>&log_message;

		ManeuverMatching result;

		explicit ManeuverMatcher(ConstRefOSMCarRoads map, const std::function<void(std::string)>&log_message):
			map(map),
			node_flag1(map.node_count, false),
			node_flag2(map.node_count, false),
			log_message(log_message){}

		template<class F>
		void handle_from_to(
			uint32_t from_way, uint32_t from_via_node,
			uint32_t to_way, uint32_t to_via_node,
			uint64_t osm_relation_id,
			TurnDir dir,
			const F&add_maneuver
		){
			std::vector<uint32_t>from_dlink_candidate;

			for(uint32_t l=map.first_link_of_way[from_way]; l!=map.first_link_of_way[from_way+1]; ++l){
				if(map.link_tail[l] == from_via_node){
					uint32_t dlink = link_to_backward_dlink(l);
					if(map.dlink_traversal_time_in_ms[dlink] != std::numeric_limits<uint32_t>::max())
						from_dlink_candidate.push_back(dlink);
				}
				if(map.link_head[l] == from_via_node){
					uint32_t dlink = link_to_forward_dlink(l);
					if(map.dlink_traversal_time_in_ms[dlink] != std::numeric_limits<uint32_t>::max())
						from_dlink_candidate.push_back(dlink);
				}
			}
			std::vector<uint32_t>to_dlink_candidate;

			for(uint32_t l=map.first_link_of_way[to_way]; l!=map.first_link_of_way[to_way+1]; ++l){
				if(map.link_tail[l] == to_via_node){
					uint32_t dlink = link_to_forward_dlink(l);
					if(map.dlink_traversal_time_in_ms[dlink] != std::numeric_limits<uint32_t>::max())
						to_dlink_candidate.push_back(dlink);
				}
				if(map.link_head[l] == to_via_node){
					uint32_t dlink = link_to_backward_dlink(l);
					if(map.dlink_traversal_time_in_ms[dlink] != std::numeric_limits<uint32_t>::max())
						to_dlink_candidate.push_back(dlink);
				}
			}

			if(from_dlink_candidate.empty()){
				log_message("OSM relation with ID "+std::to_string(osm_relation_id)+" is a turn restriction where the \"from\"-way does not pass through the \"via\"-node -> ignoring restriction");
				return;
			}

			if(to_dlink_candidate.empty()){
				log_message("OSM relation with ID "+std::to_string(osm_relation_id)+" is a turn restriction where the \"to\"-way does not pass through the \"via\"-node -> ignoring restriction");
				return;
			}

			if(from_dlink_candidate.size() == 1 && to_dlink_candidate.size() == 1){
				uint32_t
					from_dlink = from_dlink_candidate.front(),
					to_dlink = to_dlink_candidate.front();
				add_maneuver(from_dlink, to_dlink);
				if(!has_turn_direction(TurnGeo(map, from_dlink, to_dlink), dir)){
					log_message("OSM relation with ID "+std::to_string(osm_relation_id)+" is a turn restriction where only one potential turn exists because of oneways. However, this turn does not fulfill the given direction -> assuming direction is wrong and importing restriction");
				}
			}else{
				bool created_maneuver = false;
				for(uint32_t from_dlink:from_dlink_candidate){
					for(uint32_t to_dlink:to_dlink_candidate){
						if(has_turn_direction(TurnGeo(map, from_dlink, to_dlink), dir)){
							add_maneuver(from_dlink, to_dlink);
							if(created_maneuver)
								log_message("OSM relation with ID "+std::to_string(osm_relation_id)+" is a turn restriction where two turns fulfill the given direction -> assuming both turns are forbidden");
							created_maneuver = true;
						}
					}
				}
				if(!created_maneuver)
					log_message("OSM relation with ID "+std::to_string(osm_relation_id)+" is a turn restriction where no turn fulfills the given direction and the oneway constelation allows for several interpretations -> it is unclear what is forbidden -> ignoring restriction");
			}
		}

		void match_simple(const Maneuver&m){
			assert(m.via_way_list.empty());

			auto&in_to_way = node_flag1;

			auto set_mark_of_nodes_of_way = [&](uint32_t way_id, bool val){
				forall_nodes_of_way(
					map, way_id,
					[&](uint32_t x){
						in_to_way[x] = val;
						return true; // continue
					}
				);
			};

			auto mark_nodes_of_way = [&](uint32_t way_id){
				set_mark_of_nodes_of_way(way_id, true);
			};

			auto unmark_nodes_of_way = [&](uint32_t way_id){
				set_mark_of_nodes_of_way(way_id, false);
			};

			uint32_t via_node = m.via_node;
			if(via_node == invalid_id){
				mark_nodes_of_way(m.to_way);

				bool ambiguous = false;
				forall_nodes_of_way(
					map, m.from_way,
					[&](uint32_t x){
						if(in_to_way[x]){
							if(via_node == invalid_id){
								via_node = x;
							}else{
								log_message("OSM relation with ID "+std::to_string(m.osm_relation_id)+" is a turn restriction without \"via\"-node and the two referenced ways cross twice -> it is unclear what is forbidden -> ignoring restriction");
								ambiguous = true;
								return false; // break
							}
						}
						return true; // continue
					}
				);
				unmark_nodes_of_way(m.to_way);
				if(ambiguous)
					return;
				if(via_node == invalid_id){
					log_message("OSM relation with ID "+std::to_string(m.osm_relation_id)+" is a turn restriction without \"via\"-node and the two referenced ways do not cross -> it is unclear what is forbidden -> ignoring restriction");
					return;
				}
			}

			auto add_maneuver = [&](uint32_t from_dlink, uint32_t to_dlink){
				result.maneuver_type.push_back(m.type);
				result.first_dlink_of_maneuver.push_back(result.maneuver_dlink.size());
				result.maneuver_dlink.push_back(from_dlink);
				result.maneuver_dlink.push_back(to_dlink);
				result.maneuver_osm_id.push_back(m.osm_relation_id);
				assert(
					is_dlink_path(
						map,
						{
							&result.maneuver_dlink[0]+result.first_dlink_of_maneuver.back(),
							&result.maneuver_dlink[0]+result.maneuver_dlink.size()
						}
					)
				);
			};

			handle_from_to(
				m.from_way, via_node,
				m.to_way, via_node,
				m.osm_relation_id, m.dir,
				add_maneuver
			);
		}

		void match_complex(const Maneuver&m){
			assert(!m.via_way_list.empty());

			std::vector<bool>&in_prev_way = node_flag1;
			std::vector<bool>&in_next_way = node_flag2;

			auto set_mark_of_nodes_of_way = [&](std::vector<bool>&node_flag, uint32_t way_id, bool val){
				forall_nodes_of_way(
					map, way_id,
					[&](uint32_t x){
						node_flag[x] = val;
						return true; // continue
					}
				);
			};

			auto mark_nodes_of_way = [&](std::vector<bool>&node_flag, uint32_t way_id){
				set_mark_of_nodes_of_way(node_flag, way_id, true);
			};

			auto unmark_nodes_of_way = [&](std::vector<bool>&node_flag, uint32_t way_id){
				set_mark_of_nodes_of_way(node_flag, way_id, false);
			};

			std::vector<uint32_t>intermediate_dlink_list;

			bool all_ways_matched = true;
			for(uint32_t i=0; i<m.via_way_list.size(); ++i){
				uint32_t
					prev_way = i==0 ? m.from_way : m.via_way_list[i-1],
					way = m.via_way_list[i],
					next_way = i==m.via_way_list.size() -1 ? m.to_way : m.via_way_list[i+1];

				mark_nodes_of_way(in_prev_way, prev_way);
				mark_nodes_of_way(in_next_way, next_way);

				bool way_matched = false;

				{
					uint32_t
						first_link = invalid_id,
						last_link = invalid_id;

					for(uint32_t link=map.first_link_of_way[way]; link!=map.first_link_of_way[way+1]; ++link){
						if(in_prev_way[map.link_tail[link]])
							first_link = link;
						if(in_next_way[map.link_head[link]])
							last_link = link;
					}

					if(first_link != invalid_id && last_link != invalid_id && first_link <= last_link){
						for(uint32_t link = first_link; link <= last_link; ++link)
							intermediate_dlink_list.push_back(link_to_forward_dlink(link));
						way_matched = true;
					}
				}

				if(!way_matched){
					uint32_t
						first_link = invalid_id,
						last_link = invalid_id;

					for(uint32_t link=map.first_link_of_way[way+1]-1; link!=map.first_link_of_way[way]-1; --link){
						if(in_prev_way[map.link_head[link]])
							first_link = link;
						if(in_next_way[map.link_tail[link]])
							last_link = link;
					}

					if(first_link != invalid_id && last_link != invalid_id && first_link >= last_link){
						for(uint32_t link = first_link; link >= last_link; --link)
							intermediate_dlink_list.push_back(link_to_backward_dlink(link));
						continue;
					}
				}

				unmark_nodes_of_way(in_prev_way, prev_way);
				unmark_nodes_of_way(in_next_way, next_way);

				if(!way_matched){
					log_message("OSM relation with ID "+std::to_string(m.osm_relation_id)+" is a turn restriction with ways as \"via\". However, cannot connect the "+std::to_string(map.way_osm_id[way])+" way to the way before and/or after-> it is unclear what is forbidden -> ignoring restriction");
					all_ways_matched = false;
					break;
				}
			}

			if(!all_ways_matched)
				return;

			if(intermediate_dlink_list.empty()){
				log_message("OSM relation with ID "+std::to_string(m.osm_relation_id)+" is a turn restriction with ways as \"via\". However, the via ways do not result in links -> ignoring restriction");
				return;
			}

			auto add_maneuver = [&](uint32_t from_dlink, uint32_t to_dlink){
				result.maneuver_type.push_back(m.type);
				result.first_dlink_of_maneuver.push_back(result.maneuver_dlink.size());
				result.maneuver_dlink.push_back(from_dlink);
				result.maneuver_dlink.insert(
					result.maneuver_dlink.end(),
					intermediate_dlink_list.begin(),
					intermediate_dlink_list.end()
				);
				result.maneuver_osm_id.push_back(m.osm_relation_id);
				result.maneuver_dlink.push_back(to_dlink);
				assert(
					is_dlink_path(
						map,
						{
							&result.maneuver_dlink[0]+result.first_dlink_of_maneuver.back(),
							&result.maneuver_dlink[0]+result.maneuver_dlink.size()
						}
					)
				);
			};

			handle_from_to(
				m.from_way, dlink_tail(map, intermediate_dlink_list.front()),
				m.to_way, dlink_head(map, intermediate_dlink_list.front()),
				m.osm_relation_id, m.dir,
				add_maneuver
			);
		}

		void match_finish(){
			result.first_dlink_of_maneuver.push_back(result.maneuver_dlink.size());
		}

		void match_all(const std::vector<Maneuver>&maneuver_list){
			log_message("FOO "+std::to_string(maneuver_list.size()));
			for(auto&m:maneuver_list){
				if(m.via_way_list.empty())
					match_simple(m);
				else
					match_complex(m);
			}
			match_finish();
		}

	};

	void build_forbidden_maneuvers_and_convert_mandatory_to_forbidden(VecOSMCarRoads&map, const ManeuverMatching&mm, const std::function<void(std::string)>log_message){
		VecLinkEndsAdjArray adj = build_adj_array(map.as_cref());

		map.first_dlink_of_forbidden_maneuver.clear();
		map.forbidden_maneuver_dlink.clear();
		map.forbidden_maneuver_osm_id.clear();

		uint32_t maneuver_count = mm.first_dlink_of_maneuver.size()-1;
		assert(mm.maneuver_type.size() == maneuver_count);
		assert(mm.maneuver_osm_id.size() == maneuver_count);

		for(uint32_t m=0; m<maneuver_count; ++m){
			if(mm.maneuver_type[m] == ManeuverType::forbidden){
				map.first_dlink_of_forbidden_maneuver.push_back(map.forbidden_maneuver_dlink.size());
				map.forbidden_maneuver_dlink.insert(
					map.forbidden_maneuver_dlink.end(),
					mm.maneuver_dlink.begin() + mm.first_dlink_of_maneuver[m],
					mm.maneuver_dlink.begin() + mm.first_dlink_of_maneuver[m+1]
				);
				map.forbidden_maneuver_osm_id.push_back(mm.maneuver_osm_id[m]);

				assert(
					is_dlink_path(
						map.as_cref(),
						{
							&map.forbidden_maneuver_dlink[0]+map.first_dlink_of_forbidden_maneuver.back(),
							&map.forbidden_maneuver_dlink[0]+map.forbidden_maneuver_dlink.size()
						}
					)
				);
			}else{
				const uint32_t*begin = &mm.maneuver_dlink[0] + mm.first_dlink_of_maneuver[m];
				const uint32_t*end = &mm.maneuver_dlink[0] + mm.first_dlink_of_maneuver[m+1];

				uint32_t maneuver_len = end - begin;

				bool created_maneuver = false;

				for(uint32_t i=1; i<maneuver_len; ++i){
					unsigned t = dlink_tail(map.as_cref(), begin[i]);
					for(uint32_t a=adj.first_outgoing_dlink_index_of_node[t]; a<adj.first_outgoing_dlink_index_of_node[t+1]; ++a){
						unsigned dlink = adj.outgoing_dlink[a];
						if(dlink != begin[i] && dlink != reverse_dlink(begin[i-1])){
							created_maneuver = true;
							map.first_dlink_of_forbidden_maneuver.push_back(map.forbidden_maneuver_dlink.size());
							map.forbidden_maneuver_dlink.insert(
								map.forbidden_maneuver_dlink.end(),
								begin,
								begin+i
							);
							map.forbidden_maneuver_dlink.push_back(dlink);
							map.forbidden_maneuver_osm_id.push_back(mm.maneuver_osm_id[m]);
							assert(
								is_dlink_path(
									map.as_cref(),
									{
										&map.forbidden_maneuver_dlink[0]+map.first_dlink_of_forbidden_maneuver.back(),
										&map.forbidden_maneuver_dlink[0]+map.forbidden_maneuver_dlink.size()
									}
								)
							);
						}
					}
				}
				if(!created_maneuver)
					log_message("OSM relation with ID "+std::to_string(mm.maneuver_osm_id[m])+" is a mandatory turn restriction along a path where a no deviation is possible -> restriction has no effect -> restriction ignored");

			}
		}

		map.forbidden_maneuver_count = map.first_dlink_of_forbidden_maneuver.size();
		map.first_dlink_of_forbidden_maneuver.push_back(map.forbidden_maneuver_dlink.size());
		map.forbidden_maneuver_dlink_count = map.forbidden_maneuver_dlink.size();
	}
}




VecOSMCarRoads import_car_roads_from_osm_pbf_file(const std::string&file_name, std::vector<uint64_t>extra_node_osm_id, std::function<void(std::string)>log_message){
	std::vector<uint64_t>osm_shape_node_list;

	VecOSMCarRoads map;

	unordered_read_osm_pbf(
		file_name,
		nullptr,
		[&](uint64_t way_osm_id, Span<const uint64_t>node_osm_id_list, const TagMap&tags){
			if(is_osm_way_used_by_cars(way_osm_id, tags, log_message)){
				map.way_osm_id.push_back(way_osm_id);
				for(auto node_osm_id:node_osm_id_list){
					osm_shape_node_list.push_back(node_osm_id);
				}
				osm_shape_node_list.push_back(node_osm_id_list.front());
				osm_shape_node_list.push_back(node_osm_id_list.back());
			}
		},
		nullptr,
		log_message
	);

	map.way_count = map.way_osm_id.size();

	std::sort(map.way_osm_id.begin(), map.way_osm_id.end());
	std::sort(osm_shape_node_list.begin(), osm_shape_node_list.end());

        if(!extra_node_osm_id.empty()){
                std::sort(extra_node_osm_id.begin(), extra_node_osm_id.end());
                extra_node_osm_id.erase(
                        set_difference(
                                extra_node_osm_id.begin(), extra_node_osm_id.end(), 
                                osm_shape_node_list.begin(), osm_shape_node_list.end(),
                                extra_node_osm_id.begin()
                        ), 
                        extra_node_osm_id.end()
                );
        }

	{
		auto
			in = osm_shape_node_list.begin(),
			in_end = osm_shape_node_list.end(),
			out = osm_shape_node_list.begin();

		while(in != in_end){
			auto run_begin = in;
			auto run_end = in+1;
			while(run_end != in_end && *run_end == *in)
				++run_end;
			if(run_end - run_begin > 1){
				map.node_osm_id.push_back(*in);
			} else {
				*out = *in;
				++out;
			}
			in = run_end;
		}

		osm_shape_node_list.erase(out, in_end);
		osm_shape_node_list.shrink_to_fit();
	}

        if(!extra_node_osm_id.empty()){
                std::sort(extra_node_osm_id.begin(), extra_node_osm_id.end());
                extra_node_osm_id.erase(
                        set_difference(
                                extra_node_osm_id.begin(), extra_node_osm_id.end(), 
                                map.node_osm_id.begin(), map.node_osm_id.end(),
                                extra_node_osm_id.begin()
                        ),
                        extra_node_osm_id.end()
                );

                std::vector<uint64_t>new_node_osm_id(extra_node_osm_id.size() + map.node_osm_id.size());
                std::merge(
                        map.node_osm_id.begin(), map.node_osm_id.end(),
                        extra_node_osm_id.begin(), extra_node_osm_id.end(),
                        new_node_osm_id.begin()
                );
                new_node_osm_id.swap(map.node_osm_id);
        }

	auto to_index = [](uint64_t id, const std::vector<uint64_t>&list)->uint32_t{
		auto
			begin = list.begin(),
			end = list.end(),
			i = std::partition_point(
				begin, end,
				[=](uint64_t x){return x < id;}
			);
		if(i == end || *i != id)
			return invalid_id;
		else
			return i-begin;
	};

	auto to_shape_index = [&](uint64_t node_osm_id) -> uint64_t{
		return to_index(node_osm_id, osm_shape_node_list);
	};

	auto to_routing_index = [&](uint64_t node_osm_id) -> uint64_t{
		return to_index(node_osm_id, map.node_osm_id);
	};

	auto to_way_index = [&](uint64_t way_osm_id) -> uint64_t{
		return to_index(way_osm_id, map.way_osm_id);
	};

	std::vector<LatLon> shape_pos(osm_shape_node_list.size());

	map.node_count = map.node_osm_id.size();
	map.node_pos.resize(map.node_count);

	map.first_shape_pos_of_link.push_back(0);

	std::vector<uint32_t>link_way;

	std::vector<Maneuver> maneuver_list;

	ordered_read_osm_pbf(
		file_name,
		[&](uint64_t node_osm_id, LatLon p, const TagMap&tags){
			uint64_t shape_index = to_shape_index(node_osm_id);
			if(shape_index != invalid_id)
				shape_pos[shape_index] = p;

			uint64_t node_index = to_routing_index(node_osm_id);
			if(node_index != invalid_id)
				map.node_pos[node_index] = p;
		},
		[&](uint64_t way_osm_id, Span<const uint64_t>node_osm_id_list, const TagMap&tags){
			uint32_t way_id = to_way_index(way_osm_id);
			if(way_id != invalid_id){

				uint32_t link_in_way_count = 0;
				uint32_t link_begin = map.link_tail.size();

				uint64_t prev_node = to_routing_index(node_osm_id_list.front());
				LatLon first_pos = map.node_pos[prev_node];
				LatLon prev_pos = first_pos;
				uint32_t distance_since_prev_pos_in_cm = 0;


				for(size_t i=1; i<node_osm_id_list.size(); ++i){
					uint64_t shape_index = to_shape_index(node_osm_id_list[i]);
					if(shape_index == invalid_id){
						uint64_t now_node = to_routing_index(node_osm_id_list[i]);
						assert(now_node != invalid_id);

						LatLon now_pos = map.node_pos[now_node];
						distance_since_prev_pos_in_cm += compute_distance_in_cm(GeoPos(prev_pos), GeoPos(now_pos));
						prev_pos = now_pos;

						++link_in_way_count;
						map.link_tail.push_back(prev_node);
						map.link_head.push_back(now_node);
						link_way.push_back(way_id);
						map.link_length_in_cm.push_back(distance_since_prev_pos_in_cm);
						map.first_shape_pos_of_link.push_back(map.shape_pos.size());
						prev_node = now_node;
						distance_since_prev_pos_in_cm = 0;
					} else {
						LatLon now_pos = shape_pos[shape_index];
						map.shape_pos.push_back(now_pos);
						distance_since_prev_pos_in_cm += compute_distance_in_cm(GeoPos(prev_pos), GeoPos(now_pos));
						prev_pos = now_pos;
					}
				}

				uint32_t link_end = map.link_tail.size();

				map.dlink_traversal_time_in_ms.resize(map.dlink_traversal_time_in_ms.size() + 2*link_in_way_count);
				SpeedOrDuration speed_or_duration = get_osm_way_speed_or_duration(way_osm_id, first_pos, prev_pos, tags, log_message);

				auto compute_traversal_time = [](uint32_t length_in_cm, uint32_t speed_in_kmh){
					if(speed_in_kmh == 0){
						return std::numeric_limits<uint32_t>::max();
					}else{
						uint32_t time_in_ms = (36*length_in_cm) / speed_in_kmh;
						return time_in_ms;
					}
				};

				if(speed_or_duration.holds_speed()){
					for(uint32_t i=link_begin; i<link_end; ++i){
						map.dlink_traversal_time_in_ms[link_to_forward_dlink(i)] = compute_traversal_time(map.link_length_in_cm[i], speed_or_duration.forward_speed_in_kmh());
						map.dlink_traversal_time_in_ms[link_to_backward_dlink(i)] = compute_traversal_time(map.link_length_in_cm[i], speed_or_duration.backward_speed_in_kmh());
					}
				}else{
					assert(speed_or_duration.holds_duration());
					if(link_end == link_begin+1){
						map.dlink_traversal_time_in_ms[link_to_forward_dlink(link_begin)] = speed_or_duration.forward_duration_in_ms();
						map.dlink_traversal_time_in_ms[link_to_backward_dlink(link_begin)] = speed_or_duration.backward_duration_in_ms();
					}else{
						uint32_t way_length_in_cm = 0;
						for(uint32_t i=link_begin; i<link_end; ++i)
							way_length_in_cm += map.link_length_in_cm[i];

						auto scale=[](uint32_t dur_in_ms, uint32_t len_in_cm, uint32_t total_len_in_cm){
							return static_cast<uint32_t>(static_cast<uint64_t>(dur_in_ms) * static_cast<uint64_t>(len_in_cm) / static_cast<uint64_t>(total_len_in_cm));
						};

						for(uint32_t i=link_begin; i<link_end; ++i){
							map.dlink_traversal_time_in_ms[link_to_forward_dlink(i)] = scale(speed_or_duration.forward_duration_in_ms(), map.link_length_in_cm[i], way_length_in_cm);
							map.dlink_traversal_time_in_ms[link_to_backward_dlink(i)] = scale(speed_or_duration.backward_duration_in_ms(), map.link_length_in_cm[i], way_length_in_cm);
						}
					}
				}
			}
		},
		[&](uint64_t osm_relation_id, Span<const OSMRelationMember>member_list, const TagMap&tags){
			const char*restriction = tags["restriction"];
			if(!restriction)
				return;

			std::vector<uint32_t>from_way_list;
			uint32_t via_node = invalid_id;
			std::vector<uint32_t>via_way_list;
			std::vector<uint32_t>to_way_list;

			uint32_t via_count = 0;
			uint32_t from_count = 0;
			uint32_t to_count = 0;

			for(unsigned i=0; i<member_list.size(); ++i){
				if(str_eq(member_list[i].role, "via")){
					if(member_list[i].type == OSMIDType::node){
						if(via_node != invalid_id || !via_way_list.empty()){
							log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has several conflicting \"via\" roles, ignoring restriction");
							return;
						}
						++via_count;
						via_node = to_routing_index(member_list[i].id);
						if(via_node == invalid_id){
							return;
						}
					}else if(member_list[i].type == OSMIDType::way){
						if(via_node != invalid_id){
							log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has several \"via\" roles, ignoring restriction");
							return;
						}
						++via_count;
						uint32_t via_way = to_way_index(member_list[i].id);
						if(via_way == invalid_id)
							return;
						via_way_list.push_back(via_way);
					}else{
						log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has a \"via\" role that is a restriction. Only via nodes and via ways are supported. Ignoring restriction");
						return;
					}
				}else if(str_eq(member_list[i].role, "from")){
					if(member_list[i].type != OSMIDType::way){
						log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has a \"from\" that is no way -> ignoring restriction");
						return;
					}
					++from_count;
					uint32_t way_id = to_way_index(member_list[i].id);
					if(way_id != invalid_id)
						from_way_list.push_back(way_id);
				}else if(str_eq(member_list[i].role, "to")){
					if(member_list[i].type != OSMIDType::way){
						log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has a \"to\" that is no way -> ignoring restriction");
						return;
					}
					++to_count;
					uint32_t way_id = to_way_index(member_list[i].id);
					if(way_id != invalid_id)
						to_way_list.push_back(way_id);
				}else if(str_eq(member_list[i].role, "location_hint")){
					// ignore
				}else{
					if(log_message)
						log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" and unknown role \""+member_list[i].role+"\" -> ignoring role");
				}
			}
			if(from_count == 0){
				log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has no \"from\" role -> ignoring restriction");
				return;
			}
			if(to_count == 0){
				log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has no \"to\" role -> ignoring restriction");
				return;
			}

			// This happens when a turn restriction involves a road closed for cars.
			if(from_way_list.empty() || to_way_list.empty())
				return;

			if(starts_with("only_", restriction)){
				if(from_way_list.size() != 1){
					log_message("OSM mandatory turn restriction with ID "+std::to_string(osm_relation_id)+" has several \"from\" roles -> ignoring restriction");
					return;
				}
				if(to_way_list.size() != 1){
					log_message("OSM mandatory turn restriction with ID "+std::to_string(osm_relation_id)+" has several \"to\" roles -> ignoring restriction");
					return;
				}

				struct MandatoryTurnType{
					const char*name;
					TurnDir dir;
				};
				MandatoryTurnType type_list [] = {
					{"only_right_turn", TurnDir::right},
					{"only_left_turn", TurnDir::left},
					{"only_straight_on", TurnDir::straight},
					{"only_uturn", TurnDir::uturn}
				};
				for(MandatoryTurnType&t:type_list){
					if(str_eq(restriction, t.name)){
						maneuver_list.push_back({
							osm_relation_id,
							from_way_list.front(),
							via_node,
							via_way_list,
							to_way_list.back(),
							t.dir,
							ManeuverType::mandatory
						});
						return;
					}
				}
				log_message("OSM relation with ID "+std::to_string(osm_relation_id)+" is an unknown mandatory turn restriction with the value \""+restriction+"\" -> ignoring restriction");
			}else{
				struct ForbiddenTurnType{
					const char*name;
					TurnDir dir;
				};
				ForbiddenTurnType type_list [] = {
					{"no_left_turn", TurnDir::left},
					{"no_right_turn", TurnDir::right},
					{"no_straight_on", TurnDir::straight},
					{"no_exit", TurnDir::straight},
					{"no_entry", TurnDir::straight},
					{"no_u_turn", TurnDir::uturn}
				};
				for(ForbiddenTurnType&t:type_list){
					if(str_eq(restriction, t.name)){
						for(uint32_t from_way:from_way_list){
							for(uint32_t to_way:to_way_list){
								maneuver_list.push_back({
									osm_relation_id,
									from_way,
									via_node,
									via_way_list,
									to_way,
									t.dir,
									ManeuverType::forbidden
								});
							}
						}
						return;
					}
				}
				log_message("OSM relation with ID "+std::to_string(osm_relation_id)+" is an unknown forbidden turn restriction with the value \""+restriction+"\" -> ignoring restriction");
			}
		},
		log_message
	);

	map.link_count = map.link_tail.size();
	map.shape_pos_count = map.shape_pos.size();

	if(!is_sorted_using_less(link_way)){
		auto p = compute_stable_sort_permutation_using_key(link_way, map.link_count, [](uint32_t x){return x;});
		link_way = apply_permutation(p, link_way);
		map.link_head = apply_permutation(p, map.link_head);
		map.link_tail = apply_permutation(p, map.link_tail);
		map.link_length_in_cm = apply_permutation(p, map.link_length_in_cm);

		std::vector<uint32_t>dlink_traversal_time_in_ms(map.dlink_traversal_time_in_ms.size());
		for(uint32_t l=0; l<map.link_count; ++l){
			dlink_traversal_time_in_ms[link_to_forward_dlink(l)] = map.dlink_traversal_time_in_ms[link_to_forward_dlink(p[l])];
			dlink_traversal_time_in_ms[link_to_backward_dlink(l)] = map.dlink_traversal_time_in_ms[link_to_backward_dlink(p[l])];
		}
		map.dlink_traversal_time_in_ms = std::move(dlink_traversal_time_in_ms);

		std::vector<uint32_t>first_shape_pos_of_link(map.link_count+1, 0);
		for(uint32_t l=0; l<map.link_count; ++l)
			first_shape_pos_of_link[l] = map.first_shape_pos_of_link[p[l]+1]-map.first_shape_pos_of_link[p[l]];
		compute_prefix_sum(first_shape_pos_of_link);

		std::vector<LatLon>shape_pos(map.shape_pos_count);
		for(uint32_t l=0; l<map.link_count; ++l){
			std::copy(
				&map.shape_pos[0] + map.first_shape_pos_of_link[p[l]],
				&map.shape_pos[0] + map.first_shape_pos_of_link[p[l]+1],
				&shape_pos[0] + first_shape_pos_of_link[l]
			);
		}

		map.first_shape_pos_of_link = std::move(first_shape_pos_of_link);
		map.shape_pos = std::move(shape_pos);
	}

	map.first_link_of_way = invert_func(link_way, map.way_count);

	map.forbidden_maneuver_count = 0;
	map.forbidden_maneuver_dlink_count = 0;
	map.first_dlink_of_forbidden_maneuver = {0};

	std::sort(
		maneuver_list.begin(), maneuver_list.end(),
		[](const Maneuver&l, const Maneuver&r){
			return l.osm_relation_id < r.osm_relation_id;
		}
	);

	ManeuverMatching maneuver_matching;
	{
		ManeuverMatcher matcher(map.as_ref(), log_message);
		matcher.match_all(maneuver_list);
		maneuver_matching = std::move(matcher.result);
	}

	build_forbidden_maneuvers_and_convert_mandatory_to_forbidden(map, maneuver_matching, log_message);

	map.assert_correct_size();
	assert_osm_car_roads_valid(map.as_cref());
	return map;
}

}
