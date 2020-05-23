#include <routingkit/osm_profile.h>
#include <routingkit/osm_graph_builder.h>
#include <routingkit/osm_decoder.h>
#include <routingkit/vector_io.h>
#include <routingkit/id_mapper.h>
#include <routingkit/strongly_connected_component.h>
#include <routingkit/filter.h>

#include <functional>
#include <iostream>
#include <string>
#include <unordered_set>
#include <fstream>
#include <sstream>

using namespace RoutingKit;
using namespace std;

const uint8_t tunnel_bit = 1;
const uint8_t freeway_bit = 2;

bool is_tunnel(const TagMap&way_tags){
	const char*tunnel_str = way_tags["tunnel"];
	if(tunnel_str && !strcmp(tunnel_str, "yes"))
		return true;
	return false;
}

bool is_freeway(const TagMap&way_tags){
	const char*freeway_str = way_tags["highway"];
	if(freeway_str && !strcmp(freeway_str, "motorway"))
		return true;

	return false;
}

int main(int argc, char*argv[]){
	if(argc < 2){
		cout << argv[0] << " pbf_file [traffic1.csv [traffic2.csv [...]]]\n\nWrites extracted files into current working directory"<<endl;
		return 2;
	}


        unordered_set<uint64_t>is_osm_node_traffic_end_point;

        for(int i=2; i<argc; ++i){
                std::ifstream in(argv[i]);
                if(!in){
                        cout << "Cannot open "<< argv[i]<<" skipping" << endl;
                }else{
                        string line;
                        while(getline(in, line)){
                                istringstream line_in(line);
                                string node_id;
                                getline(line_in, node_id, ',');
                                is_osm_node_traffic_end_point.insert(stol(node_id));
                                getline(line_in, node_id, ',');
                                is_osm_node_traffic_end_point.insert(stol(node_id));
                        }
                }
        }

        cout << "number of osm nodes that are traffic endpoints : "<< is_osm_node_traffic_end_point.size() << endl;

	string pbf_file = argv[1];

	std::function<void(const std::string&)>log_message = [](const string&msg){
		cout << msg << endl;
	};

	auto mapping = load_osm_id_mapping_from_pbf(
		pbf_file,
		[&](uint64_t osm_node_id, const TagMap&){
                        return is_osm_node_traffic_end_point.count(osm_node_id) > 0;
                },
		[&](uint64_t osm_way_id, const TagMap&tags){
			return is_osm_way_used_by_cars(osm_way_id, tags, log_message);
		},
		log_message
	);

	unsigned routing_way_count = mapping.is_routing_way.population_count();
	std::vector<uint32_t>way_speed(routing_way_count);
	std::vector<uint8_t>way_category(routing_way_count, 0);

	auto routing_graph = load_osm_routing_graph_from_pbf(
		pbf_file,
		mapping,
		[&](uint64_t osm_way_id, unsigned routing_way_id, const TagMap&way_tags){
			way_speed[routing_way_id] = get_osm_way_speed(osm_way_id, way_tags, log_message);
			way_category[routing_way_id] = 0;
			if(is_tunnel(way_tags)){
				way_category[routing_way_id] |= tunnel_bit;
			}
			if(is_freeway(way_tags)){
				way_category[routing_way_id] |= freeway_bit;
			}
			return get_osm_car_direction_category(osm_way_id, way_tags, log_message);
		},
		[&](uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags, std::function<void(OSMTurnRestriction)>on_new_restriction){
			return decode_osm_car_turn_restrictions(osm_relation_id, member_list, tags, on_new_restriction, log_message);
		},
		log_message,
                false,
                OSMRoadGeometry::uncompressed
	);

	unsigned arc_count  = routing_graph.arc_count();
	unsigned node_count  = routing_graph.node_count();

	std::vector<uint32_t>travel_time = routing_graph.geo_distance;
	for(unsigned a=0; a<arc_count; ++a){
		travel_time[a] *= 18000;
		travel_time[a] /= way_speed[routing_graph.way[a]];
		travel_time[a] /= 5;
	}

	unsigned tunnel_arc_count = 0;
	unsigned freeway_arc_count = 0;

	std::vector<uint32_t>tail = invert_inverse_vector(routing_graph.first_out);

	std::vector<uint8_t>arc_category(arc_count);
	for(unsigned a=0; a<arc_count; ++a){
		arc_category[a] = way_category[routing_graph.way[a]];
		if(arc_category[a] & tunnel_bit){
			++tunnel_arc_count;
		}
		if(arc_category[a] & freeway_bit){
			++freeway_arc_count;
		}
	}

	cout << "total node count : " << node_count << endl;
	cout << "total arc count : " << arc_count << endl;
	cout << "tunnel arc count : " << tunnel_arc_count << endl;
	cout << "freeway arc count : " << freeway_arc_count << endl;

	IDMapper routing_node_mapper(mapping.is_routing_node);
	auto node_ids = std::vector<uint64_t>(routing_graph.first_out.size() - 1);
	for (unsigned i = 0; i < node_ids.size(); ++i) {
	  node_ids[i] = routing_node_mapper.to_global(i);
	}


        std::vector<unsigned>first_out = routing_graph.first_out;
        std::vector<unsigned>head = routing_graph.head;
        std::vector<unsigned>geo_distance = routing_graph.geo_distance;

        std::vector<float>latitude = routing_graph.latitude;
        std::vector<float>longitude = routing_graph.longitude;

        std::vector<bool>vec_node_in_largest_scc = compute_largest_strongly_connected_component(first_out, head);
        BitVector node_in_largest_scc(node_count);
        for(unsigned i=0; i<node_count; ++i){
                node_in_largest_scc.set(i, vec_node_in_largest_scc[i]);
        }

        BitVector arc_in_largest_scc(arc_count);
        for(unsigned i=0; i<arc_count; ++i){
                arc_in_largest_scc.set(i, node_in_largest_scc.is_set(tail[i]) && node_in_largest_scc.is_set(head[i]));
        }

        std::vector<unsigned>old_to_new_node(node_count);
        unsigned new_node_count = 0;
        for(unsigned x=0; x<node_count; ++x){
                if(node_in_largest_scc.is_set(x)){
                        old_to_new_node[x] = new_node_count++;
                }else{
                        old_to_new_node[x] = invalid_id;
                }
        }

        std::vector<unsigned>old_to_new_arc(arc_count);
        unsigned new_arc_count = 0;
        for(unsigned x=0; x<arc_count; ++x){
                if(arc_in_largest_scc.is_set(x)){
                        old_to_new_arc[x] = new_arc_count++;
                }else{
                        old_to_new_arc[x] = invalid_id;
                }
        }

	cout << "node count in largest scc: " << new_node_count << endl;
	cout << "arc count in largest scc: " << new_arc_count << endl;

        // first we get rid of the arcs outside of the lscc

        tail = keep_element_of_vector_if(arc_in_largest_scc, tail);
        head = keep_element_of_vector_if(arc_in_largest_scc, head);
        arc_category = keep_element_of_vector_if(arc_in_largest_scc, arc_category);
        geo_distance = keep_element_of_vector_if(arc_in_largest_scc, geo_distance);
        travel_time = keep_element_of_vector_if(arc_in_largest_scc, travel_time);

        std::vector<unsigned>forbidden_turn_from_arc, forbidden_turn_to_arc;

        for(unsigned i=0; i<routing_graph.forbidden_turn_from_arc.size(); ++i){
                unsigned from = old_to_new_arc[routing_graph.forbidden_turn_from_arc[i]];
                unsigned to = old_to_new_arc[routing_graph.forbidden_turn_to_arc[i]];
                if(from != invalid_id && to != invalid_id){
                        forbidden_turn_from_arc.push_back(from);
                        forbidden_turn_to_arc.push_back(to);
                }
        }

        // next we get rid of the nodes outside of the lscc

        for(auto&x:tail){
                x = old_to_new_node[x];
                assert(x != invalid_id);
        }

        for(auto&x:head){
                x = old_to_new_node[x];
                assert(x != invalid_id);
        }

        first_out = invert_vector(tail, new_node_count);

        latitude = keep_element_of_vector_if(node_in_largest_scc, latitude);
        longitude = keep_element_of_vector_if(node_in_largest_scc, longitude);
        node_ids = keep_element_of_vector_if(node_in_largest_scc, node_ids);

        // now take care of the modelling nodes

        std::vector<unsigned>first_modelling_node;
        std::vector<float>modelling_node_latitude;
        std::vector<float>modelling_node_longitude;

        {
                for(unsigned a=0; a<arc_count; ++a){
                        unsigned n = old_to_new_arc[a];
                        if(n != invalid_id){
                                first_modelling_node.push_back(modelling_node_latitude.size());
                                modelling_node_latitude.insert(
                                        modelling_node_latitude.end(),
                                        routing_graph.modelling_node_latitude.begin() + routing_graph.first_modelling_node[a],
                                        routing_graph.modelling_node_latitude.begin() + routing_graph.first_modelling_node[a+1]
                                );
                                modelling_node_longitude.insert(
                                        modelling_node_longitude.end(),
                                        routing_graph.modelling_node_longitude.begin() + routing_graph.first_modelling_node[a],
                                        routing_graph.modelling_node_longitude.begin() + routing_graph.first_modelling_node[a+1]
                                );
                        }
                }

                first_modelling_node.push_back(modelling_node_latitude.size());
                assert(first_modelling_node.size() == new_arc_count+1);
        }

	save_vector("first_out", first_out);
	save_vector("head", head);
	save_vector("tail", tail);
	save_vector("arc_category", arc_category);
	save_vector("geo_distance", geo_distance);
	save_vector("travel_time", travel_time);
	save_vector("latitude", latitude);
	save_vector("longitude", longitude);
	save_vector("forbidden_turn_from_arc", forbidden_turn_from_arc);
	save_vector("forbidden_turn_to_arc", forbidden_turn_to_arc);
	save_vector("osm_node_ids", node_ids);
	save_vector("first_modelling_node", first_modelling_node);
	save_vector("modelling_node_latitude", modelling_node_latitude);
	save_vector("modelling_node_longitude", modelling_node_longitude);
}
