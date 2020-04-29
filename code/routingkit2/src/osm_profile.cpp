#include "osm_profile.h"
#include "min_max.h"
#include "str.h"

namespace RoutingKit2{

bool is_osm_way_used_by_cars(uint64_t osm_way_id, const TagMap&tags, std::function<void(std::string)>log_message){
	if(tags["area"] != nullptr)
		return false;

	const char* junction = tags["junction"];
	if(junction != nullptr)
		return true;

	const char* route = tags["route"];
	if(route && str_eq(route, "ferry"))
		return true;

	const char* ferry = tags["ferry"];
	if(ferry && str_eq(ferry, "yes"))
		return true;

	const char* highway = tags["highway"];
	if(highway == nullptr)
		return false;

	const char*motorcar = tags["motorcar"];
	if(motorcar && str_eq(motorcar, "no"))
		return false;

	const char*motorroad = tags["motorroad"];
	if(motorroad && str_eq(motorroad, "no"))
		return false;

	const char*motor_vehicle = tags["motor_vehicle"];
	if(motor_vehicle && str_eq(motor_vehicle, "no"))
		return false;

	const char*access = tags["access"];
	if(access){
		if(!(str_eq(access, "yes") || str_eq(access, "permissive") || str_eq(access, "delivery")|| str_eq(access, "designated") || str_eq(access, "destination")))
			return false;
	}

	if(
		str_eq(highway, "motorway") ||
		str_eq(highway, "trunk") ||
		str_eq(highway, "primary") ||
		str_eq(highway, "secondary") ||
		str_eq(highway, "tertiary") ||
		str_eq(highway, "unclassified") ||
		str_eq(highway, "residential") ||
		str_eq(highway, "service") ||
		str_eq(highway, "motorway_link") ||
		str_eq(highway, "trunk_link") ||
		str_eq(highway, "primary_link") ||
		str_eq(highway, "secondary_link") ||
		str_eq(highway, "tertiary_link") ||
		str_eq(highway, "motorway_junction") ||
		str_eq(highway, "living_street") ||
		str_eq(highway, "residential") ||
		//str_eq(highway, "track") ||
		str_eq(highway, "ferry")
	)
		return true;

	if(str_eq(highway, "bicycle_road")){
		auto motorcar = tags["motorcar"];
		if(motorcar != nullptr)
			if(str_eq(motorcar, "yes"))
				return true;
		return false;
	}

	if(
		str_eq(highway, "construction") ||
		str_eq(highway, "path") ||
		str_eq(highway, "footway") ||
		str_eq(highway, "cycleway") ||
		str_eq(highway, "bridleway") ||
		str_eq(highway, "pedestrian") ||
		str_eq(highway, "bus_guideway") ||
		str_eq(highway, "raceway") ||
		str_eq(highway, "escape") ||
		str_eq(highway, "steps") ||
		str_eq(highway, "proposed") ||
		str_eq(highway, "conveying")
	)
		return false;

	const char* oneway = tags["oneway"];
	if(oneway != nullptr){
		if(str_eq(oneway, "reversible") || str_eq(oneway, "alternating")) {
			return false;
		}
	}

	const char* maxspeed = tags["maxspeed"];
	if(maxspeed != nullptr)
		return true;

	return false;
}

uint32_t parse_osm_duration_value(uint64_t osm_way_id, const char*duration, std::function<void(std::string)>log_message){

	auto is_digit = [](char c){
		return '0' <= c && c <= '9';
	};

	auto chop = [&] () -> uint32_t {
		uint32_t ret = 0;
		for(;;){
			if(!is_digit(*duration) && *duration != ':' && *duration != '\0'){
				if(log_message)
					log_message("Warning: duration tag of ferry OSM way "+std::to_string(osm_way_id)+" contains an unknown character '"+std::string(1, *duration)+"'; ignoring duration tag");
				return std::numeric_limits<uint32_t>::max();
			}
			if(*duration == ':' || *duration == '\0'){
				return ret;
			}
			ret *= 10;
			ret += *duration - '0';
			++duration;
		}
	};

	if(*duration == '\0'){
		if(log_message)
			log_message("Warning: duration tag of OSM ferry way "+std::to_string(osm_way_id)+" is empty; ignoring duration tag");
		return std::numeric_limits<uint32_t>::max();
	}

	uint32_t a = chop();
	if(a == std::numeric_limits<uint32_t>::max())
		return std::numeric_limits<uint32_t>::max();
	if(*duration == '\0')
		return a*60;
	++duration;

	uint32_t b = chop();
	if(b == std::numeric_limits<uint32_t>::max())
		return std::numeric_limits<uint32_t>::max();
	if(*duration == '\0')
		return a*60*60+b*60;
	++duration;

	uint32_t c = chop();
	if(c == std::numeric_limits<uint32_t>::max())
		return std::numeric_limits<uint32_t>::max();
	if(*duration == '\0')
		return a*60*60+b*60+c;

	if(log_message)
		log_message("Warning: duration tag of OSM ferry way "+std::to_string(osm_way_id)+" is has more than two colons; ignoring duration tag");
	return std::numeric_limits<uint32_t>::max();
}

uint32_t parse_osm_maxspeed_value(uint64_t osm_way_id, const char*maxspeed, std::function<void(std::string)>log_message){
	if(str_eq(maxspeed, "signals") || str_eq(maxspeed, "variable"))
		return std::numeric_limits<uint16_t>::max();

	if(str_eq(maxspeed, "none") || str_eq(maxspeed, "unlimited"))
		return 130;

	if(str_eq(maxspeed, "walk") || str_eq(maxspeed, "foot") || str_wild_char_eq(maxspeed, "??:walk"))
		return 5;

	if(str_wild_char_eq(maxspeed, "??:urban") || str_eq(maxspeed, "urban"))
		return 40;

	if(str_wild_char_eq(maxspeed, "??:living_street") || str_eq(maxspeed, "living_street"))
		return 10;

	if(str_eq(maxspeed, "de:rural") || str_eq(maxspeed, "at:rural") || str_eq(maxspeed, "ro:rural") || str_eq(maxspeed, "rural"))
		return 100;
	if(str_eq(maxspeed, "ru:rural") || str_eq(maxspeed, "fr:rural") || str_eq(maxspeed, "ua:rural"))
		return 90;

	if(str_eq(maxspeed, "ru:motorway"))
		return 110;
	if(str_eq(maxspeed, "at:motorway") || str_eq(maxspeed, "ro:motorway"))
		return 130;

	if(str_eq(maxspeed, "national"))
		return 100;

	if(str_eq(maxspeed, "ro:trunk"))
		return 100;
	if(str_eq(maxspeed, "dk:rural") || str_eq(maxspeed, "ch:rural"))
		return 80;
	if(str_eq(maxspeed, "it:rural") || str_eq(maxspeed, "hu:rural"))
		return 90;
	if(str_eq(maxspeed, "de:zone:30") || str_eq(maxspeed, "de:zone30"))
		return 30;


	if('0' <= *maxspeed && *maxspeed <= '9'){
		unsigned speed = 0;
		while('0' <= *maxspeed && *maxspeed <= '9'){
			speed *= 10;
			speed += *maxspeed - '0';
			++maxspeed;
		}
		while(*maxspeed == ' ')
			++maxspeed;
		if(*maxspeed == '\0' || str_eq(maxspeed, "km/h") || str_eq(maxspeed, "kmh") || str_eq(maxspeed, "kph")){
			return speed;
		}else if(str_eq(maxspeed, "mph")){
			return speed * 1609 / 1000;
		}else if(str_eq(maxspeed, "knots")){
			return speed * 1852 / 1000;
		}else{
			if(log_message)
				log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unknown unit \""+maxspeed+"\" for its \"maxspeed\" tag -> assuming \"km/h\".");
			return speed;
		}
	}else{
		if(log_message)
			log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unrecognized value of \""+maxspeed+"\" for its \"maxspeed\" tag.");
	}

	return std::numeric_limits<uint16_t>::max();
}

SpeedOrDuration get_osm_way_speed_or_duration_disregarding_oneway_and_maxspeed(uint64_t osm_way_id, const TagMap&tags, std::function<void(std::string)>log_message){
	auto highway = tags["highway"];
	if(highway){
		if(str_eq(highway, "motorway"))
			return SpeedOrDuration::from_speed_in_kmh(90);
		if(str_eq(highway, "motorway_link"))
			return SpeedOrDuration::from_speed_in_kmh(45);
		if(str_eq(highway, "trunk"))
			return SpeedOrDuration::from_speed_in_kmh(85);
		if(str_eq(highway, "trunk_link"))
			return SpeedOrDuration::from_speed_in_kmh(40);
		if(str_eq(highway, "primary"))
			return SpeedOrDuration::from_speed_in_kmh(65);
		if(str_eq(highway, "primary_link"))
			return SpeedOrDuration::from_speed_in_kmh(30);
		if(str_eq(highway, "secondary"))
			return SpeedOrDuration::from_speed_in_kmh(55);
		if(str_eq(highway, "secondary_link"))
			return SpeedOrDuration::from_speed_in_kmh(25);
		if(str_eq(highway, "tertiary"))
			return SpeedOrDuration::from_speed_in_kmh(40);
		if(str_eq(highway, "tertiary_link"))
			return SpeedOrDuration::from_speed_in_kmh(20);
		if(str_eq(highway, "unclassified"))
			return SpeedOrDuration::from_speed_in_kmh(25);
		if(str_eq(highway, "residential"))
			return SpeedOrDuration::from_speed_in_kmh(25);
		if(str_eq(highway, "living_street"))
			return SpeedOrDuration::from_speed_in_kmh(10);
		if(str_eq(highway, "service"))
			return SpeedOrDuration::from_speed_in_kmh(1);
		if(str_eq(highway, "track"))
			return SpeedOrDuration::from_speed_in_kmh(1);
		if(str_eq(highway, "ferry"))
			return SpeedOrDuration::from_speed_in_kmh(5);
	}

	auto junction = tags["junction"];
	if(junction){
		return SpeedOrDuration::from_speed_in_kmh(20);
	}

	auto route = tags["route"];
	if(route && str_eq(route, "ferry")) {
		auto duration_str = tags["duration"];
		if(duration_str){
			uint32_t d = parse_osm_duration_value(osm_way_id, duration_str, log_message);
			if(d != std::numeric_limits<uint32_t>::max())
				return SpeedOrDuration::from_duration_in_ms(1000*d);
		}
		return SpeedOrDuration::from_speed_in_kmh(5);
	}

	auto ferry = tags["ferry"];
	if(ferry) {
		return SpeedOrDuration::from_speed_in_kmh(5);
	}

	return SpeedOrDuration::from_speed_in_kmh(50);
}

SpeedOrDuration get_osm_way_speed_or_duration_disregarding_oneway(uint64_t osm_way_id, const TagMap&tags, std::function<void(std::string)>log_message){
	auto
		maxspeed = tags["maxspeed"],
		forward_maxspeed = tags["maxspeed:forward"],
		backward_maxspeed = tags["maxspeed:backward"]
	;
	if(maxspeed || forward_maxspeed || backward_maxspeed){

		if(maxspeed && str_eq(maxspeed, "unposted")){
			return get_osm_way_speed_or_duration_disregarding_oneway_and_maxspeed(osm_way_id, tags, log_message);
		}

		auto parse = [&](const char*str, const char*tag){
			char lower_case_maxspeed[1024];
			copy_str_and_make_lower_case(str, lower_case_maxspeed, sizeof(lower_case_maxspeed)-1);
			uint32_t value = std::numeric_limits<uint16_t>::max();
			split_str_at_osm_value_separators(
			lower_case_maxspeed,
				[&](const char*s){
					min_to(value, parse_osm_maxspeed_value(osm_way_id, s, log_message));
				}
			);
			if(value == 0){
				log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has speed 0 km/h in tag \""+tag+"\", setting it to 1 km/h");
				value = 1;
			}
			return value;
		};

		uint32_t maxspeed_value = std::numeric_limits<uint32_t>::max();
		uint32_t forward_maxspeed_value = std::numeric_limits<uint32_t>::max();
		uint32_t backward_maxspeed_value = std::numeric_limits<uint32_t>::max();

		if(maxspeed)
			maxspeed_value = parse(maxspeed, "maxspeed");
		if(forward_maxspeed)
			forward_maxspeed_value = parse(forward_maxspeed, "maxspeed:forward");
		if(backward_maxspeed)
			backward_maxspeed_value = parse(backward_maxspeed, "maxspeed:backward");

		if(forward_maxspeed_value == std::numeric_limits<uint32_t>::max())
			forward_maxspeed_value = maxspeed_value;
		if(backward_maxspeed_value == std::numeric_limits<uint32_t>::max())
			backward_maxspeed_value = maxspeed_value;

		if(forward_maxspeed_value != std::numeric_limits<uint32_t>::max() &&
			backward_maxspeed_value == std::numeric_limits<uint32_t>::max()){
			auto estimated = get_osm_way_speed_or_duration_disregarding_oneway_and_maxspeed(osm_way_id, tags, log_message);
			estimated.forward = forward_maxspeed_value;
			return estimated;
		}

		if(forward_maxspeed_value == std::numeric_limits<uint32_t>::max() &&
			backward_maxspeed_value != std::numeric_limits<uint32_t>::max()){
			auto estimated = get_osm_way_speed_or_duration_disregarding_oneway_and_maxspeed(osm_way_id, tags, log_message);
			estimated.backward = backward_maxspeed_value;
			return estimated;
		}

		if(forward_maxspeed_value != std::numeric_limits<uint32_t>::max() &&
			backward_maxspeed_value != std::numeric_limits<uint32_t>::max())
			return SpeedOrDuration::from_forward_backward_speed_in_kmh(forward_maxspeed_value, backward_maxspeed_value);

	}

	if(maxspeed && log_message)
		log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unrecognized \"maxspeed\" tag of \""+maxspeed+"\" -> falling back to speed estimation heuristics.");
	if(forward_maxspeed && log_message)
		log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unrecognized \"maxspeed:forward\" tag of \""+forward_maxspeed+"\" -> falling back to speed estimation heuristics.");
	if(backward_maxspeed && log_message)
		log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unrecognized \"maxspeed:backward\" tag of \""+backward_maxspeed+"\" -> falling back to speed estimation heuristics.");

	return get_osm_way_speed_or_duration_disregarding_oneway_and_maxspeed(osm_way_id, tags, log_message);
}

SpeedOrDuration get_osm_way_speed_or_duration(uint64_t osm_way_id, LatLon pos_of_first_node, LatLon pos_of_last_node, const TagMap&tags, std::function<void(std::string)>log_message){

	SpeedOrDuration ret = get_osm_way_speed_or_duration_disregarding_oneway(osm_way_id, tags, log_message);

	const char
		*oneway = tags["oneway"],
		*junction = tags["junction"],
		*highway = tags["highway"]
	;
	if(oneway != nullptr){
		if(str_eq(oneway, "-1") || str_eq(oneway, "reverse") || str_eq(oneway, "backward")) {
			ret.close_forward();
			return ret;
		} else if(str_eq(oneway, "yes") || str_eq(oneway, "true") || str_eq(oneway, "1")) {
			ret.close_backward();
			return ret;
		} else if(str_eq(oneway, "no") || str_eq(oneway, "false") || str_eq(oneway, "0")) {
			return ret;
		} else if(str_eq(oneway, "reversible") || str_eq(oneway, "alternating")) {
			ret.close_forward();
			ret.close_backward();
			return ret;
		} else {
			if(log_message)
				log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has unknown oneway tag value \""+oneway+"\" for \"oneway\". Way is closed.");
		}
	} else if(junction != nullptr && str_eq(junction, "roundabout")) {
		ret.close_backward();
		return ret;
	} else if(highway != nullptr && (str_eq(highway, "motorway") || str_eq(highway, "motorway_link"))) {
		ret.close_backward();
		return ret;
	}
	return ret;
}

std::string get_osm_way_name(uint64_t osm_way_id, const TagMap&tags, std::function<void(std::string)>log_message){
	auto
		name = tags["name"],
		ref = tags["ref"];

	if(name != nullptr && ref != nullptr)
		return std::string(name) + ";"+ref;
	else if(name != nullptr)
		return std::string(name);
	else if(ref != nullptr)
		return std::string(ref);
	else
		return std::string();
}

} // RoutingKit2
