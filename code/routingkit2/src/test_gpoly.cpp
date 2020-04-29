#include "gpoly.h"
#include <vector>
#include <string>
#include <random>

#include "catch.hpp"

using namespace RoutingKit2;
using namespace std;

namespace {
	struct EncodingTest{
		const char*str;
		int64_t val;
	};

	EncodingTest encoding_test_list [] = {
		{"`~oia@", -17998321},
		{"_p~iF", +3850000},
		{"~ps|U", -12020000},
		{"_ulL", +220000},
		{"nnqC", -75000},
		{"_mqN", +255200},
		{"vxq`@", -550300},
		{"?", 0}
	};
}

TEST_CASE("encode_var_int", "[GPoly]"){
	char buffer[1024];

	for(auto&t:encoding_test_list){
		memset(buffer, 0, sizeof(buffer));
		char*end = encode_gpoly_var_int(buffer, t.val);
		REQUIRE(string(buffer) == string(t.str));
		REQUIRE(static_cast<unsigned>(end-buffer) == compute_gpoly_var_int_size(t.val));
	}
}

TEST_CASE("decode_var_int", "[GPoly]"){

	for(auto&t:encoding_test_list){
		auto result = decode_gpoly_var_int_from_front_without_advance(t.str, t.str+strlen(t.str));
		REQUIRE(result.val == t.val);
		REQUIRE(result.offset == strlen(t.str));
	}
}


TEST_CASE("gpoly", "[GPoly]"){

	std::minstd_rand gen;

	std::vector<LatLon> polyline(1000);
	for(auto&p:polyline){
		p.lat_in_decamicrodeg = gen()%1000000;
		p.lon_in_decamicrodeg = gen()%1000000;
	}

	std::string str = encode_gpoly_from_span(polyline);

	std::vector<LatLon> out_polyline = decode_gpoly(str);

	REQUIRE(polyline == out_polyline);
}
