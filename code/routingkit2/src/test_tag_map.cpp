#include "tag_map.h"

#include "catch.hpp"

using namespace RoutingKit2;

TEST_CASE("empty", "[TagMap]"){
	TagMap map;
	REQUIRE(map["test"] == nullptr);
}

TEST_CASE("ThreeItems", "[TagMap]"){
	const char* key [] = {"foo", "bar", "foobar"};
	const char* value [] = {"a", "b", "c"};

	TagMap map;

	map.build(
		3,
		[&](unsigned i){return key[i];},
		[&](unsigned i){return value[i];}
	);

	REQUIRE(map["foo"] == value[0]);
	REQUIRE(map["foo"] == value[0]);

	REQUIRE(map["bar"] == value[1]);
	REQUIRE(map["bar"] == value[1]);

	REQUIRE(map["bar"] != value[0]);

	REQUIRE(map["foobar"] == value[2]);

	REQUIRE(map["barfoo"] == nullptr);
}

TEST_CASE("FiveItems", "[TagMap]"){
	const char* key [] = {"foo", "axbyc", "aybxc", "bar", "azbzc"};
	const char* value [] = {"a", "b", "c", "d", "e"};

	TagMap map;

	map.build(
		5,
		[&](unsigned i){return key[i];},
		[&](unsigned i){return value[i];}
	);

	REQUIRE(map["foo"] == value[0]);
	REQUIRE(map["axbyc"] == value[1]);
	REQUIRE(map["aybxc"] == value[2]);
	REQUIRE(map["bar"] == value[3]);
	REQUIRE(map["azbzc"] == value[4]);
}


TEST_CASE("Rebuild", "[TagMap]"){

	TagMap map;

	REQUIRE(map["test"] == nullptr);

	{
		const char* key [] = {"foo", "bar", "foobar"};
		const char* value [] = {"a", "b", "c"};

		map.build(
			3,
			[&](unsigned i){return key[i];},
			[&](unsigned i){return value[i];}
		);

		REQUIRE(map["foo"] == value[0]);
		REQUIRE(map["foo"] == value[0]);

		REQUIRE(map["bar"] == value[1]);
		REQUIRE(map["bar"] == value[1]);

		REQUIRE(map["bar"] != value[0]);

		REQUIRE(map["foobar"] == value[2]);

		REQUIRE(map["barfoo"] == nullptr);
	}
	{
		const char* key [] = {"foo", "axbyc", "aybxc", "bar", "azbzc"};
		const char* value [] = {"a", "b", "c", "d", "e"};

		map.build(
			5,
			[&](unsigned i){return key[i];},
			[&](unsigned i){return value[i];}
		);

		REQUIRE(map["foo"] == value[0]);
		REQUIRE(map["axbyc"] == value[1]);
		REQUIRE(map["aybxc"] == value[2]);
		REQUIRE(map["bar"] == value[3]);
		REQUIRE(map["azbzc"] == value[4]);
	}
}
