#ifndef ROUTING_KIT2_OSM_PROFILE_HPP
#define ROUTING_KIT2_OSM_PROFILE_HPP

#include "tag_map.h"
#include "osm_types.h"
#include "geo_pos.h"

#include <functional>
#include <string>
#include <limits>
#include <stdint.h>
#include <assert.h>

namespace RoutingKit2{

bool is_osm_way_used_by_cars(uint64_t osm_way_id, const TagMap&tags, std::function<void(std::string)>log_message);

uint32_t parse_osm_duration_value(uint64_t osm_way_id, const char*duration, std::function<void(std::string)>log_message);

uint32_t parse_osm_maxspeed_value(uint64_t osm_way_id, const char*maxspeed, std::function<void(std::string)>log_message);

struct SpeedOrDuration{
	uint32_t forward;
	uint32_t backward;
	bool is_duration;

	uint32_t forward_speed_in_kmh()const noexcept{
		assert(holds_speed());
		return forward;
	}

	uint32_t backward_speed_in_kmh()const noexcept{
		assert(holds_speed());
		return backward;
	}

	uint32_t forward_duration_in_ms()const noexcept{
		assert(holds_duration());
		return forward;
	}

	uint32_t backward_duration_in_ms()const noexcept{
		assert(holds_duration());
		return backward;
	}


	bool holds_duration()const noexcept{
		return is_duration;
	}

	bool holds_speed()const noexcept{
		return !holds_duration();
	}

	static constexpr SpeedOrDuration from_speed_in_kmh(uint16_t s)noexcept{
		return {
			s,
			s,
			false
		};
	}

	static constexpr SpeedOrDuration from_forward_backward_speed_in_kmh(uint16_t forward, uint16_t backward)noexcept{
		return {
			forward,
			backward,
			false
		};
	}

	static constexpr SpeedOrDuration from_backward_forward_speed_in_kmh(uint16_t backward, uint16_t forward)noexcept{
		return {
			forward,
			backward,
			false
		};
	}

	static constexpr SpeedOrDuration from_duration_in_ms(uint32_t t)noexcept{
		return {
			t,
			t,
			true
		};
	}

	static constexpr SpeedOrDuration from_forward_backward_duration_in_ms(uint16_t forward, uint16_t backward)noexcept{
		return {
			forward,
			backward,
			true
		};
	}

	static constexpr SpeedOrDuration from_backward_forward_duration_in_ms(uint16_t backward, uint16_t forward)noexcept{
		return {
			forward,
			backward,
			true
		};
	}

	void close_forward(){
		if(holds_speed())
			forward = 0;
		else
			forward = std::numeric_limits<uint32_t>::max();
	}

	void close_backward(){
		if(holds_speed())
			backward = 0;
		else
			backward = std::numeric_limits<uint32_t>::max();
	}
};

SpeedOrDuration get_osm_way_speed_or_duration(uint64_t osm_way_id, LatLon pos_of_first_node, LatLon pos_of_last_node, const TagMap&tags, std::function<void(std::string)>log_message = nullptr);

std::string get_osm_way_name(uint64_t osm_way_id, const TagMap&tags, std::function<void(std::string)>log_message = nullptr);


} // RoutingKit2

#endif

