#include "map.h"

#include "catch.hpp"

using namespace RoutingKit2;

TEST_CASE("DLink", "[Map]"){
	for(uint32_t l=0; l<1000; ++l){
		uint32_t d = link_to_forward_dlink(l);

		REQUIRE(is_forward_dlink(d));
		REQUIRE(!is_backward_dlink(d));

		REQUIRE(dlink_to_link(d) == l);

		uint32_t r = reverse_dlink(d);

		REQUIRE(!is_forward_dlink(r));
		REQUIRE(is_backward_dlink(r));

		REQUIRE(dlink_to_link(r) == l);
		REQUIRE(reverse_dlink(r) == d);
		REQUIRE(r == link_to_backward_dlink(l));
	}
}
