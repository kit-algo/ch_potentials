#ifndef ROUTING_KIT2_TIMESTAMP_FLAG_H
#define ROUTING_KIT2_TIMESTAMP_FLAG_H

#include <vector>
#include <stdint.h>

namespace RoutingKit2{

class TimestampFlags{
public:
	TimestampFlags(){}

	explicit TimestampFlags(uint32_t id_count):last_seen(id_count, 0), current_timestamp(1){}

	bool is_set(uint32_t id)const{
		return last_seen[id] == current_timestamp;
	}

	void set(uint32_t id){
		last_seen[id] = current_timestamp;
	}

	void reset_one(uint32_t id){
		last_seen[id] = current_timestamp-1;
	}

	void reset_all(){
		++current_timestamp;
		if(current_timestamp == 0){
			std::fill(last_seen.begin(), last_seen.end(), 0);
			current_timestamp = 1;
		}
	}

private:
	std::vector<uint16_t>last_seen;
	uint16_t current_timestamp;
};

} // RoutingKit

#endif

