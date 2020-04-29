#include "bit_vector.h"
#include "id_mapper.h"

#include "catch.hpp"

using namespace RoutingKit2;

TEST_CASE( "IDMapperTest", "[IDMapperTest]" ) {
        BitVector test_vector_list[5];

	BitVector&single_bit_vector = test_vector_list[0];
	BitVector&sparse_regular_vector = test_vector_list[1];
	BitVector&dense_regular_vector = test_vector_list[2];
	BitVector&sparse_irregular_vector = test_vector_list[3];
	BitVector&dense_irregular_vector = test_vector_list[4];

	single_bit_vector = BitVector(1000000, false);
	single_bit_vector.set(555555);

	sparse_regular_vector = make_bit_vector(9999999, [](uint64_t x){ return x % 7919 == 0;});

	dense_regular_vector = make_bit_vector(8888888, [](uint64_t x){ return x % 7 == 0;});

	sparse_irregular_vector = make_bit_vector(
		18978978,
		[](uint64_t x){
			if(((x/1000000) % 2) != 0)
				return false;
			else
				return x % 6917 == 0;

		}
	);

	dense_irregular_vector = make_bit_vector(
		7777777,
		[](uint64_t x){
			if(((x/1000000) % 2) == 0)
				return false;
			else
				return x % 3 == 0;

		}
	);

        SECTION( "is_global_id_mapped" ) {
	        for(auto&vec:test_vector_list){
		        IDMapper map(vec);
		        for(uint64_t x=0; x<map.global_id_count(); ++x){
			        REQUIRE(vec.is_set(x) == map.is_global_id_mapped(x));
		        }
	        }
        }

        SECTION( "global_to_local" ) {
	        for(auto&vec:test_vector_list){
		        IDMapper map(vec);
		        REQUIRE(map.local_id_count() != 0);

		        uint64_t s = 0;
		        for(uint64_t x=0; x<map.global_id_count(); ++x){
			        if(vec.is_set(x)){
				        REQUIRE(s == map.to_local(x, ~0ull));
				        ++s;
			        } else {
				        REQUIRE(~0ull == map.to_local(x, ~0ull));
			        }
		        }
		        REQUIRE(map.local_id_count() == s);

	        }
        }

        SECTION( "local_to_global" ) {
                for(auto&vec:test_vector_list){
		        IDMapper map(vec);
		        REQUIRE(map.local_id_count() != 0);

		        uint64_t s = 0;
		        for(uint64_t x=0; x<map.global_id_count(); ++x){
			        if(vec.is_set(x)){
				        REQUIRE(map.to_global(s) == x);
				        ++s;
			        }
		        }
		        REQUIRE(map.local_id_count() == s);

	        }
        }

        SECTION( "bijection" ) {
                for(auto&vec:test_vector_list){
		        IDMapper map(vec);
		        REQUIRE(map.local_id_count() != 0);

		        for(uint64_t x=0; x<map.local_id_count(); ++x){
			        uint64_t y = map.to_global(x);
			        REQUIRE(y != ~0ull);
			        uint64_t z = map.to_local(y, ~0ull);
			        REQUIRE(z != ~0ull);
			        REQUIRE(x == z);
		        }

		        for(uint64_t x=0; x<map.global_id_count(); ++x){
			        uint64_t y = map.to_local(x, ~0ull);
			        if(y != ~0ull){
				        uint64_t z = map.to_global(y);
				        REQUIRE(z != ~0ull);
				        REQUIRE(x == z);
			        }
		        }
	        }
        }
}

