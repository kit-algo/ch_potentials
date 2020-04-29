#include "turn.h"
#include "catch.hpp"
#include <ostream>

using namespace std;
using namespace RoutingKit2;

namespace {
	ostream&operator<<(ostream&o, TurnDir dir){
		switch(dir){
			case TurnDir::sharp_left:return o<<"sharp_left";
			case TurnDir::sharp_right:return o<<"sharp_right";
			case TurnDir::left:return o<<"left";
			case TurnDir::right:return o<<"right";
			case TurnDir::uturn:return o<<"uturn";
			case TurnDir::straight:return o<<"straight";
			default:assert(false); return o;
		}
	}
}

TEST_CASE("SimpleClassifyTurn", "[Turn]"){
	REQUIRE(classify_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, -1), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(-1, -1))) == TurnDir::uturn);
	REQUIRE(classify_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, -1), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(1, 1))) == TurnDir::straight);
	REQUIRE(classify_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, 0), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(0, 1))) == TurnDir::right);
	REQUIRE(classify_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, 0), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(-1, 1))) == TurnDir::sharp_right);
	REQUIRE(classify_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, 0), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(0, -1))) == TurnDir::left);
	REQUIRE(classify_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, 0), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(-1, -1))) == TurnDir::sharp_left);
}

TEST_CASE("SimpleIsTurnOfType", "[Turn]"){
	REQUIRE(has_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, -1), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(-1, -1)), TurnDir::uturn));
	REQUIRE(has_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, -1), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(1, 1)), TurnDir::straight));
	REQUIRE(has_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, 0), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(0, 1)), TurnDir::right));
	REQUIRE(has_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, 0), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(-1, 1)), TurnDir::sharp_right));
	REQUIRE(has_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, 0), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(0, -1)), TurnDir::left));
	REQUIRE(has_turn_direction(TurnGeo(LatLon::from_lat_lon(-1, 0), LatLon::from_lat_lon(0, 0), LatLon::from_lat_lon(-1, -1)), TurnDir::sharp_left));
}


TEST_CASE("RotatingTest", "[Turn]"){

	LatLon from_via = LatLon::from_lat_lon(-10, 0);
	LatLon to_via = LatLon::from_lat_lon(+5, +5);
	double from_radius = 3.0;
	double to_radius = 5.0;

	const double pi_div_180 = 3.14159265359/180.0;

	auto make_from = [&](double angle){
		return LatLon::from_lat_lon(
			from_via.lat() + sin(angle * pi_div_180) * from_radius,
			from_via.lon() + cos(angle * pi_div_180) * from_radius
		);
	};


	auto make_to = [&](double angle){
		return LatLon::from_lat_lon(
			to_via.lat() + sin(angle * pi_div_180) * to_radius,
			to_via.lon() + cos(angle * pi_div_180) * to_radius
		);
	};


	for(double angle = 0.0; angle < 1000.0; angle += 10.0){
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+0))) == TurnDir::uturn);
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+170))) == TurnDir::straight);
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+190))) == TurnDir::straight);
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+35))) == TurnDir::sharp_right);
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+80))) == TurnDir::right);
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+100))) == TurnDir::right);
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle-100))) == TurnDir::left);
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle-35))) == TurnDir::sharp_left);
		REQUIRE(classify_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle-80))) == TurnDir::left);

		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+0)), TurnDir::uturn));
		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+170)), TurnDir::straight));
		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+190)), TurnDir::straight));
		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+35)), TurnDir::sharp_right));
		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+80)), TurnDir::right));
		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle+100)), TurnDir::right));
		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle-100)), TurnDir::left));
		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle-35)), TurnDir::sharp_left));
		REQUIRE(has_turn_direction(TurnGeo(make_from(angle + 0), from_via, to_via, make_to(angle-80)), TurnDir::left));
	}
}


TEST_CASE("RotatingMapTest", "[Turn]"){
	VecCarRoads map;
	map.node_count = 3;
	map.link_count = 2;

	map.link_tail = {0, 1};
	map.link_head = {1, 2};

	map.node_pos.resize(3);

	map.shape_pos = {};

	map.shape_pos_count = 0;
	map.first_shape_pos_of_link = {0, 0, 0};

	map.link_length_in_cm = {0, 0};

	map.dlink_traversal_time_in_ms = {1,1,1,1};

	map.forbidden_maneuver_count = 0;
	map.forbidden_maneuver_dlink_count = 0;
	map.first_dlink_of_forbidden_maneuver = {0};

	map.assert_correct_size();


	LatLon via = LatLon::from_lat_lon(-10, 0);
	double radius = 3.0;

	const double pi_div_180 = 3.14159265359/180.0;

	auto make = [&](double angle){
		return LatLon::from_lat_lon(
			via.lat() + sin(angle * pi_div_180) * radius,
			via.lon() + cos(angle * pi_div_180) * radius
		);
	};

	for(double angle = 0.0; angle < 1000.0; angle += 10.0){

		#define MY_TEST(from_angle, to_angle, result)\
			map.node_pos[0] = make(angle + from_angle);\
			map.node_pos[1] = via;\
			map.node_pos[2] = make(angle + to_angle);\
			REQUIRE(classify_turn_direction(TurnGeo(map.as_ref(), link_to_forward_dlink(0), link_to_forward_dlink(1))) == result);\
			REQUIRE(has_turn_direction(TurnGeo(map.as_ref(), link_to_forward_dlink(0), link_to_forward_dlink(1)), result));

		MY_TEST(0, 0, TurnDir::uturn);
		MY_TEST(0, 170, TurnDir::straight);
		MY_TEST(0, 190, TurnDir::straight);
		MY_TEST(0, 35, TurnDir::sharp_right);
		MY_TEST(0, 80, TurnDir::right);
		MY_TEST(0, 100, TurnDir::right);
		MY_TEST(0, -100, TurnDir::left);
		MY_TEST(0, -35, TurnDir::sharp_left);
		MY_TEST(0, -80, TurnDir::left);
		#undef MY_TEST
	}
}
