#include <routingkit/vector_io.h>
#include <vector>
#include <iostream>
using namespace std;
using namespace RoutingKit;

int main(int argc, char*argv[]){

	if(argc != 4){
		cerr << "usage: "<<argv[0] << " source_node_id number_of_nodes_pushed node_sequence_output_file" << endl;
		cerr << "Computes a node sequence by running a BFS. The graph data is loaded fom the current working directory." << endl;

		return 1;
	}

	unsigned source = atoi(argv[1]);
	unsigned radius = atoi(argv[2]);

	std::vector<unsigned>first_out = load_vector<unsigned>("first_out");
	std::vector<unsigned>head = load_vector<unsigned>("head");

	std::vector<unsigned> node_seq;

	unsigned queue_start = 0;

	std::vector<bool>was_pushed(first_out.size()-1, false);

	auto push = [&](unsigned x){
		if(!was_pushed[x]){
			node_seq.push_back(x);
			was_pushed[x] = true;
		}
	};

	auto pop = [&]{
		return node_seq[queue_start++];
	};

	auto empty = [&]{
		return queue_start == node_seq.size();
	};


	push(source);

	while(!empty() && node_seq.size() < radius){
		unsigned x = pop();
		for(unsigned xy=first_out[x]; xy<first_out[x+1]; ++xy){
			unsigned y = head[xy];
			push(y);
		}
	}

	save_vector(argv[3], node_seq);
}
