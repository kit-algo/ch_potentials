#include <routingkit/vector_io.h>
#include <vector>
#include <iostream>
using namespace std;
using namespace RoutingKit;

int main(int argc, char*argv[]){

	if(argc != 3){
		cerr << "usage: "<<argv[0] << " node_id box_size" << endl;
		cerr << "Computes bounding box around a nodes position. The graph data is loaded fom the current working directory." << endl;

		return 1;
	}

	unsigned node = atoi(argv[1]);
	float box_size = atof(argv[2]);

	std::vector<float>latitude = load_vector<float>("latitude");
	std::vector<float>longitude = load_vector<float>("longitude");

	cout << latitude[node] - box_size << " " << latitude[node] + box_size << " " << longitude[node] - box_size << " " << longitude[node] + box_size << endl;
}
