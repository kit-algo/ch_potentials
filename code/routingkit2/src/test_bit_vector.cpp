#include "bit_vector.h"

#include "catch.hpp"

using namespace RoutingKit2;
using namespace std;

TEST_CASE("Constructors", "[BitVector]"){
	{
		BitVector v;
	}

	{
		BitVector v(7777, false);
		REQUIRE(v.size() == 7777);
		for(unsigned i=0; i<v.size(); ++i)
			REQUIRE(!v.is_set(i));
	}

	{
		BitVector v(0, false);
	}

	{
		BitVector v(0, true);
	}

	{
		BitVector v(0, BitVector::uninitialized);
	}

	{
		BitVector v(0);
		v.resize(30, false);
	}

	{
		BitVector v(0);
		v.resize(300, true);
	}

	{
		BitVector v(0);
		v.resize(500, BitVector::uninitialized);
	}

	{
		BitVector v(100);
		v.resize(0, false);
	}

	{
		BitVector v(1000);
		v.resize(0, true);
	}

	{
		BitVector v(7777, true);
		REQUIRE(v.size() == 7777);
		for(unsigned i=0; i<v.size(); ++i)
			REQUIRE(v.is_set(i));
	}

	{
		BitVector x(7777, true);
		BitVector y = x;

		REQUIRE(x.size() == 7777);
		for(unsigned i=0; i<x.size(); ++i)
			REQUIRE(x.is_set(i));
		REQUIRE(y.size() == 7777);
		for(unsigned i=0; i<y.size(); ++i)
			REQUIRE(y.is_set(i));
	}

	{
		BitVector x(7777, true);
		BitVector y = std::move(x);

		REQUIRE(x.empty());
		REQUIRE(y.size() == 7777);
		for(unsigned i=0; i<y.size(); ++i)
			REQUIRE(y.is_set(i));
	}
}

TEST_CASE("set", "[BitVector]"){
	BitVector v(1000);
	v.set(501);
	v.set(500);
	v.set(498);
	v.set(2);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.set(500);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.reset(500);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(!v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.reset(500);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(!v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.set(500, true);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.set(500, true);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.set(500, false);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(!v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.set(500, false);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(!v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.toggle(500);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.toggle(500);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(!v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.toggle(500);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.toggle(500);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(!v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));


	v.reset(500);
	v.set_if(500, false);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(!v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.reset(500);
	v.set_if(500, true);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.set(500);
	v.set_if(500, false);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));

	v.set(500);
	v.set_if(500, true);

	REQUIRE(!v.is_set(502));
	REQUIRE(v.is_set(501));
	REQUIRE(v.is_set(500));
	REQUIRE(!v.is_set(499));
	REQUIRE(v.is_set(498));
	REQUIRE(v.is_set(2));
	REQUIRE(!v.is_set(1));
}

TEST_CASE("resize", "[BitVector]"){
	BitVector v(10000);
	v.resize(0, BitVector::uninitialized);
}

TEST_CASE("swap", "[BitVector]"){
	BitVector x(7777, true);
	BitVector y(6666, false);

	REQUIRE(x.size() == 7777);
	for(unsigned i=0; i<x.size(); ++i)
		REQUIRE(x.is_set(i));
	REQUIRE(y.size() == 6666);
	for(unsigned i=0; i<y.size(); ++i)
		REQUIRE(!y.is_set(i));

	x.swap(y);

	REQUIRE(y.size() == 7777);
	for(unsigned i=0; i<y.size(); ++i)
		REQUIRE(y.is_set(i));
	REQUIRE(x.size() == 6666);
	for(unsigned i=0; i<x.size(); ++i)
		REQUIRE(!x.is_set(i));
}

TEST_CASE("any_all", "[BitVector]"){
	{
		BitVector x(7777, true);

		REQUIRE(x.is_set(0));
		REQUIRE(x.is_any_set());
		REQUIRE(x.are_all_set());
	}

	{
		BitVector x(7777, false);

		REQUIRE(!x.is_any_set());
		REQUIRE(!x.are_all_set());
	}

	{
		BitVector x(7777, false);

		x.set(42);

		REQUIRE(x.is_any_set());
		REQUIRE(!x.are_all_set());
	}
}


TEST_CASE("set2", "[BitVector]"){
	BitVector x(7777, false);

	REQUIRE(!x.is_set(420));
	x.set(420);
	REQUIRE(!x.is_set(419));
	REQUIRE(x.is_set(420));
	REQUIRE(!x.is_set(421));
	x.reset(420);
	REQUIRE(!x.is_set(419));
	REQUIRE(!x.is_set(420));
	REQUIRE(!x.is_set(421));
	x.toggle(420);
	REQUIRE(!x.is_set(419));
	REQUIRE(x.is_set(420));
	REQUIRE(!x.is_set(421));
}


TEST_CASE("negation", "[BitVector]"){
	auto v = ~BitVector(7777, true);
	REQUIRE(v.size() == 7777);
	for(unsigned i=0; i<v.size(); ++i)
		REQUIRE(!v.is_set(i));

	v.reset_all();
	for(unsigned i=0; i<v.size(); ++i)
		REQUIRE(!v.is_set(i));

	v.set_all();
	for(unsigned i=0; i<v.size(); ++i)
		REQUIRE(v.is_set(i));

	v.reset_all();
	for(unsigned i=0; i<v.size(); ++i)
		REQUIRE(!v.is_set(i));

}

TEST_CASE("compare_eq", "[BitVector]"){
	{
		BitVector x(7777, true);
		BitVector y(6666, false);

		REQUIRE(x!=y);

		y = BitVector(7777, true);

		REQUIRE(x==y);

		y.inplace_not();

		REQUIRE(x!=y);
	}

	{
		BitVector x(7777, true);
		BitVector y(6666, false);

		REQUIRE(x!=y);

		y = BitVector(7777, true);

		REQUIRE(x==y);

		y.inplace_not();

		REQUIRE(x!=y);
	}
}

TEST_CASE("population_count", "[BitVector]"){
	{
		BitVector x(7777, true);
		BitVector y(6666, false);

		x.reset(42);

		REQUIRE(x.population_count() == 7776);
		REQUIRE(y.population_count() == 0);
	}

	{
		BitVector x(7777, true);

		REQUIRE(x.size() == 7777);
		REQUIRE(x.population_count() == 7777);
		REQUIRE(x.are_all_set());

		x.resize(3000);

		REQUIRE(x.size() == 3000);
		REQUIRE(x.population_count() == 3000);
		REQUIRE(x.are_all_set());

		x.resize(3001, true);

		REQUIRE(x.size() == 3001);
		REQUIRE(x.population_count() == 3001);
		REQUIRE(x.are_all_set());

		x.resize(7777, true);

		REQUIRE(x.size() == 7777);
		REQUIRE(x.population_count() == 7777);
		REQUIRE(x.are_all_set());

	}
}

TEST_CASE("irregular_content", "[BitVector]"){
	{
		BitVector v(12345);
		for(uint64_t i=0; i<12345; ++i)
			v.set(i, i%17 == 0);
		for(uint64_t i=0; i<12345; ++i){
			if(i%17 == 0)
				REQUIRE(v.is_set(i));
			else
				REQUIRE(!v.is_set(i));
		}
	}

	{
		auto x = make_bit_vector(12345, [](uint64_t x){return x%17 == 0;});
		for(uint64_t i=0; i<12345; ++i){
			if(i%17 == 0)
				REQUIRE(x.is_set(i));
			else
				REQUIRE(!x.is_set(i));
		}
	}
}

TEST_CASE("bitwise", "[BitVector]"){
	{
		auto
			x = make_bit_vector(12345, [](uint64_t x){return x%2 == 0;}),
			y = make_bit_vector(12345, [](uint64_t x){return x%2 != 0;});

		REQUIRE((x|y).are_all_set());
		REQUIRE((x^y).are_all_set());
		REQUIRE(!(x&y).is_any_set());
	}
}

TEST_CASE("make_large_enough_for", "[BitVector]"){
	{
		BitVector v;

		v.make_large_enough_for(5);
		REQUIRE(v.size() > 5);

		v.make_large_enough_for(8);
		REQUIRE(v.size() > 8);

		v.make_large_enough_for(15);
		REQUIRE(v.size() > 15);

		v.make_large_enough_for(1024);
		REQUIRE(v.size() > 1024);
	}

	{
		BitVector v;

		v.make_large_enough_for(5, BitVector::uninitialized);
		REQUIRE(v.size() > 5);

		v.make_large_enough_for(8, BitVector::uninitialized);
		REQUIRE(v.size() > 8);

		v.make_large_enough_for(15, BitVector::uninitialized);
		REQUIRE(v.size() > 15);

		v.make_large_enough_for(1024, BitVector::uninitialized);
		REQUIRE(v.size() > 1024);
	}
}
