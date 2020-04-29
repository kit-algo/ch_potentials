#include "map.h"
#include "osm_import.h"
using namespace RoutingKit2;

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

int main(int argc, char*argv[]){
	try{
                if(argc < 2){
                        cout << "usage: "<<argv[0] << " in.pbf output_directory [traffic1.csv [traffic2.csv [...]]]" << endl;
                        return 1;
                }

                vector<uint64_t>osm_node_traffic_end_point;

                for(int i=3; i<argc; ++i){
                        std::ifstream in(argv[i]);
                        if(!in){
                                cout << "Cannot open "<< argv[i]<<" skipping" << endl;
                        }else{
                                string line;
                                while(getline(in, line)){
                                        istringstream line_in(line);                             
                                        string node_id;
                                        getline(line_in, node_id, ',');
                                        osm_node_traffic_end_point.push_back(stol(node_id));
                                        getline(line_in, node_id, ',');
                                        osm_node_traffic_end_point.push_back(stol(node_id));
                                }
                        }
                }

                std::sort(osm_node_traffic_end_point.begin(), osm_node_traffic_end_point.end());
                osm_node_traffic_end_point.erase(std::unique(osm_node_traffic_end_point.begin(), osm_node_traffic_end_point.end()), osm_node_traffic_end_point.end());

                auto map = import_car_roads_from_osm_pbf_file(
                        argv[1],
                        osm_node_traffic_end_point,
                        [&](string msg){
		                cout << msg << endl;
	                }
                );

                dump_into_dir(argv[2], map.as_ref());
	}catch(std::exception&err){
		cerr << "Exception: " << err.what() << endl;
	}

}

