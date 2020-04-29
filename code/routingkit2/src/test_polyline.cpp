#include "polyline.h"

#include "catch.hpp"

using namespace RoutingKit2;

TEST_CASE("NoShapePointEnumeration", "[polyline]"){

	VecCarRoads map;
	map.node_count = 2;
	map.link_count = 1;

	map.link_tail = {0};
	map.link_head = {1};

	map.node_pos = {
		LatLon::from_lat_lon_in_decamicrodeg(0,0),
		LatLon::from_lat_lon_in_decamicrodeg(100,100)
	};

	map.shape_pos = {};

	map.shape_pos_count = map.shape_pos.size();
	map.first_shape_pos_of_link = {0, static_cast<uint32_t>(map.shape_pos.size())};

	map.link_length_in_cm = {compute_distance_in_cm(GeoPos(map.node_pos[0]), GeoPos(map.node_pos[1]))};

	map.dlink_traversal_time_in_ms = {1,1};

	map.forbidden_maneuver_count = 0;
	map.forbidden_maneuver_dlink_count = 0;
	map.first_dlink_of_forbidden_maneuver = {0};

	map.assert_correct_size();

	REQUIRE(dlink_head(map.as_ref(), link_to_forward_dlink(0)) == 1);
	REQUIRE(dlink_tail(map.as_ref(), link_to_forward_dlink(0)) == 0);
	REQUIRE(dlink_head(map.as_ref(), link_to_backward_dlink(0)) == 0);
	REQUIRE(dlink_tail(map.as_ref(), link_to_backward_dlink(0)) == 1);


	{
		DLinkPointEnumeratorWithoutLast e(map.as_ref(), link_to_forward_dlink(0));

		REQUIRE(*e.next() == map.node_pos[0]);
		REQUIRE(!e.next().has_value());
	}

	{
		DLinkPointEnumeratorWithoutLast e(map.as_ref(), link_to_backward_dlink(0));
		REQUIRE(*e.next() == map.node_pos[1]);
		REQUIRE(!e.next().has_value());
	}

	{
		DLinkPointEnumerator e(map.as_ref(), link_to_forward_dlink(0));
		REQUIRE(*e.next() == map.node_pos[0]);
		REQUIRE(*e.next() == map.node_pos[1]);
		REQUIRE(!e.next().has_value());
	}

	{
		DLinkPointEnumerator e(map.as_ref(), link_to_backward_dlink(0));
		REQUIRE(*e.next() == map.node_pos[1]);
		REQUIRE(*e.next() == map.node_pos[0]);
		REQUIRE(!e.next().has_value());
	}
}



TEST_CASE("WithShapePointEnumeration", "[Polyline]"){

	VecCarRoads map;
	map.node_count = 2;
	map.link_count = 1;

	map.link_tail = {0};
	map.link_head = {1};

	map.node_pos = {
		LatLon::from_lat_lon_in_decamicrodeg(0,0),
		LatLon::from_lat_lon_in_decamicrodeg(100,100)
	};

	map.shape_pos = {
		LatLon::from_lat_lon_in_decamicrodeg(50,0),
		LatLon::from_lat_lon_in_decamicrodeg(50,50),
		LatLon::from_lat_lon_in_decamicrodeg(100,0)
	};

	map.shape_pos_count = map.shape_pos.size();
	map.first_shape_pos_of_link = {0, static_cast<uint32_t>(map.shape_pos.size())};

	map.link_length_in_cm = {
		compute_distance_in_cm(GeoPos(map.node_pos[0]), GeoPos(map.shape_pos[0])) +
		compute_distance_in_cm(GeoPos(map.shape_pos[0]), GeoPos(map.shape_pos[1])) +
		compute_distance_in_cm(GeoPos(map.shape_pos[1]), GeoPos(map.shape_pos[2])) +
		compute_distance_in_cm(GeoPos(map.shape_pos[2]), GeoPos(map.node_pos[1]))
	};

	map.dlink_traversal_time_in_ms = {1,1};

	map.forbidden_maneuver_count = 0;
	map.forbidden_maneuver_dlink_count = 0;
	map.first_dlink_of_forbidden_maneuver = {0};

	map.assert_correct_size();

	REQUIRE(dlink_head(map.as_ref(), link_to_forward_dlink(0)) == 1);
	REQUIRE(dlink_tail(map.as_ref(), link_to_forward_dlink(0)) == 0);
	REQUIRE(dlink_head(map.as_ref(), link_to_backward_dlink(0)) == 0);
	REQUIRE(dlink_tail(map.as_ref(), link_to_backward_dlink(0)) == 1);


	{
		DLinkPointEnumeratorWithoutLast e(map.as_ref(), link_to_forward_dlink(0));
		REQUIRE(*e.next() == map.node_pos[0]);
		REQUIRE(*e.next() == map.shape_pos[0]);
		REQUIRE(*e.next() == map.shape_pos[1]);
		REQUIRE(*e.next() == map.shape_pos[2]);
		REQUIRE(!e.next().has_value());
	}

	{
		DLinkPointEnumeratorWithoutLast e(map.as_ref(), link_to_backward_dlink(0));
		REQUIRE(*e.next() == map.node_pos[1]);
		REQUIRE(*e.next() == map.shape_pos[2]);
		REQUIRE(*e.next() == map.shape_pos[1]);
		REQUIRE(*e.next() == map.shape_pos[0]);
		REQUIRE(!e.next().has_value());
	}

	{
		DLinkPointEnumerator e(map.as_ref(), link_to_forward_dlink(0));
		REQUIRE(*e.next() == map.node_pos[0]);
		REQUIRE(*e.next() == map.shape_pos[0]);
		REQUIRE(*e.next() == map.shape_pos[1]);
		REQUIRE(*e.next() == map.shape_pos[2]);
		REQUIRE(*e.next() == map.node_pos[1]);
		REQUIRE(!e.next().has_value());
	}

	{
		DLinkPointEnumerator e(map.as_ref(), link_to_backward_dlink(0));
		REQUIRE(*e.next() == map.node_pos[1]);
		REQUIRE(*e.next() == map.shape_pos[2]);
		REQUIRE(*e.next() == map.shape_pos[1]);
		REQUIRE(*e.next() == map.shape_pos[0]);
		REQUIRE(*e.next() == map.node_pos[0]);
		REQUIRE(!e.next().has_value());
	}
}
