#include "osm_profile.h"
#include <random>
#include <iostream>

#include "catch.hpp"

using namespace RoutingKit2;
using namespace std;

TEST_CASE("parse_osm_duration_value", "[OSMProfile]"){
	auto log_message = [](string msg){
		cerr << msg << endl;
	};
	REQUIRE(parse_osm_duration_value(42, "5", log_message) == 300);
	REQUIRE(parse_osm_duration_value(42, "05", log_message) == 300);
	REQUIRE(parse_osm_duration_value(42, "15", log_message) == 900);
	REQUIRE(parse_osm_duration_value(42, "1:8", log_message) == 4080);
	REQUIRE(parse_osm_duration_value(42, "01:8", log_message) == 4080);
	REQUIRE(parse_osm_duration_value(42, "11:8", log_message) == 40080);
	REQUIRE(parse_osm_duration_value(42, "1:8:2", log_message) == 4082);
	REQUIRE(parse_osm_duration_value(42, "01:8:2", log_message) == 4082);
	REQUIRE(parse_osm_duration_value(42, "11:8:2", log_message) == 40082);
	REQUIRE(parse_osm_duration_value(42, "1:8:12", log_message) == 4092);
	REQUIRE(parse_osm_duration_value(42, "01:8:12", log_message) == 4092);
	REQUIRE(parse_osm_duration_value(42, "11:8:12", log_message) == 40092);
}
