#include "inverse_func.h"
#include <limits>

#include "catch.hpp"

using namespace RoutingKit2;
using namespace std;

TEST_CASE("InverseFuncEmpty", "[InverseFunc]") {
	REQUIRE(invert_func(vector<uint32_t>{}, 3) == (vector<uint32_t>{0,0,0,0}));
	REQUIRE(invert_func(vector<uint32_t>{}, 0) == (vector<uint32_t>{0}));
}

TEST_CASE("InverseFuncRoundTrip", "[InverseFunc]"){
	const uint32_t element_count = 13;
	vector<uint32_t>func = {0,0,0,2,2,2,3,5,6,6,7,8,8,8,8,9};

	vector<uint32_t>inv_func = invert_func(func, element_count);

	std::vector<bool>tested(func.size(), false);
	for(uint32_t i=0; i<element_count; ++i){
		for(uint32_t j=inv_func[i]; j<inv_func[i+1]; ++j){
			REQUIRE(i == func[j]);
			REQUIRE(!tested[j]);
			tested[j] = true;
		}
	}

	for(bool b:tested)
		REQUIRE(b);

	REQUIRE(func == invert_inverse_func(inv_func));
}
