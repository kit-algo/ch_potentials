#include "geo_index.h"
#include <random>

#include "catch.hpp"

using namespace RoutingKit2;
using namespace std;

constexpr uint32_t dim = 1000000;
constexpr uint32_t seed = 42;
constexpr uint32_t item_count = 10000;
constexpr uint32_t test_query_count = 500;

GeoPos rand_geo_pos(std::minstd_rand&gen){
	GeoPos ret;

	ret.x = gen() % (2*dim+1);
	ret.y = gen() % (2*dim+1);
	ret.z = gen() % (2*dim+1);

	ret.x -= dim;
	ret.y -= dim;
	ret.z -= dim;

	return ret;
}

TEST_CASE("RandomPoints", "[GeoPosIndex]"){


	std::minstd_rand gen;
	gen.seed(seed);

	std::vector<GeoPos>pos(item_count);
	for(auto&p:pos)
		p = rand_geo_pos(gen);

	VecGeoIndex vec_geo_index;

	{
		GeoIndexBuilder builder;
		for(uint32_t i=0; i<item_count; ++i)
			builder.add(pos[i], i);
		vec_geo_index = builder.build();
	}

	GeoIndexFindAtLeastWithinRadiusQuery at_least_query(vec_geo_index.as_cref());

	GeoIndexFindWithinRadiusQuery query(vec_geo_index.as_cref());

	for(uint32_t radius:{10000, 500000, 10000000}){

		for(uint32_t i=0; i<test_query_count; ++i){
			std::vector<bool>found(item_count, false);
			std::vector<bool>at_least_found(item_count, false);

			GeoPos center = rand_geo_pos(gen);

			at_least_query.start(center, radius);
			while(auto next=at_least_query.next()){
				REQUIRE(next->pos == pos[next->id]);
				REQUIRE(!at_least_found[next->id]);
				at_least_found[next->id] = true;
			}

			query.start(center, radius);
			while(auto next=query.next()){
				REQUIRE(next->pos == pos[next->id]);
				REQUIRE(!found[next->id]);
				found[next->id] = true;
			}

			for(uint32_t j=0; j<item_count; ++j){
				uint32_t distance = compute_distance_in_cm(center, pos[j]);
				bool in_radius = distance <= radius;
				bool found_by_exact = found[j];
				bool found_by_at_least = at_least_found[j];
				if(found_by_exact)
					REQUIRE(found_by_at_least);
				REQUIRE(in_radius == found_by_exact);
			}
		}
	}

}
