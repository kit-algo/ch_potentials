#include "geo_pos.h"
#include <random>

#include "catch.hpp"

using namespace RoutingKit2;
using namespace std;

TEST_CASE("RoundtripForNormalLat", "[GeoPos]"){

	std::minstd_rand0 gen;

	// we use none of c++ *_distribution classes as their behavior varies
	// from compiler to compiler. Preductable behavior is more important
	// than uniformness.

	auto rand_lat = [&]{
		// The rounding errors are large for lat < -80 and lat > 80
		// In this test we therefore only generate -80 <= lat <= 80
		return static_cast<int32_t>(gen() % (160*100000)) - 80*100000;
	};

	auto rand_lon = [&]{
		return static_cast<int32_t>(gen() % (360*100000)) - 180*100000;
	};

	auto abs_diff = [](int32_t a, int32_t b) -> uint32_t {
		if(a < b)
			return b-a;
		else
			return a-b;
	};

	for(int i=0; i<100000; ++i){
		LatLon a = LatLon::from_lat_lon_in_decamicrodeg(rand_lat(), rand_lon());
		GeoPos b(a);
		LatLon c(b);
		REQUIRE(abs_diff(a.lat_in_decamicrodeg, c.lat_in_decamicrodeg) <= 1);
		REQUIRE(abs_diff(a.lon_in_decamicrodeg, c.lon_in_decamicrodeg) <= 1);
	}
}
