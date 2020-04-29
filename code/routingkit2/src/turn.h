#ifndef ROUTING_KIT2_TURN_H
#define ROUTING_KIT2_TURN_H

#include "map.h"

namespace RoutingKit2{

enum class TurnDir{
	sharp_left,
	left,
	straight,
	right,
	sharp_right,
	uturn
};

//! Stores the geometry of a turn. This class is used to classify turns.
struct TurnGeo{
	LatLon from_vec, to_vec;

	TurnGeo(){}

	//! Creates a turn geometry from a position via an intersection to a position.
	TurnGeo(LatLon from, LatLon via, LatLon to);

	//! Creates a turn geometry with a from and a to via position.
	//! The points are virtually shifted such that the via points are superimposed.
	//! This is useful to classify complex turn maneuvers where there is not a single via node.
	TurnGeo(LatLon from, LatLon from_via, LatLon to_via, LatLon to);

	//! Extracts the turn geometry from a map.
	//! `from_dlink` and `to_dlink` do not have to be consecutive.
	TurnGeo(ConstRefLinkShapes, uint32_t from_dlink, uint32_t to_dlink);
};

//! Given a turn geometry, returns the turn direction as TurnDir.
TurnDir classify_turn_direction(TurnGeo);

//! Checks whether a turn geometry could be consistent with a given TurnDir.
//! This is not the same as calling classify_turn_direction and comparing for equality.
//! For example, has_turn_direction return true when asked whether a sharp_left turn
//! is a left turn. However, what holds is the invariant:
//!  assert(has_turn_direction(turn_coords, classify_turn_direction(turn_coords)));
bool has_turn_direction(TurnGeo, TurnDir);

} // RoutingKit2

#endif
