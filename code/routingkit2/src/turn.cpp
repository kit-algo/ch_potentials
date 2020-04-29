#include "turn.h"
#include "polyline.h"
#include <math.h>

#include "gpoly.h"
#include <iostream>
using namespace std;

namespace RoutingKit2{

TurnGeo::TurnGeo(LatLon from, LatLon from_via, LatLon to_via, LatLon to){
	from_vec.lon_in_decamicrodeg = from_via.lon_in_decamicrodeg - from.lon_in_decamicrodeg;
	from_vec.lat_in_decamicrodeg = from_via.lat_in_decamicrodeg - from.lat_in_decamicrodeg;
	to_vec.lon_in_decamicrodeg = to.lon_in_decamicrodeg - to_via.lon_in_decamicrodeg;
	to_vec.lat_in_decamicrodeg = to.lat_in_decamicrodeg - to_via.lat_in_decamicrodeg;
}

TurnGeo::TurnGeo(LatLon from, LatLon via, LatLon to){
	*this = TurnGeo(from, via, via, to);
}

TurnGeo::TurnGeo(ConstRefLinkShapes map, uint32_t from_dlink, uint32_t to_dlink){
	LatLon from, from_via, to_via, to;
	{
		DLinkPointEnumerator enumerator(map, reverse_dlink(from_dlink));
		from_via = *enumerator.next();
		from = *enumerator.next();
	}
	{
		DLinkPointEnumerator enumerator(map, to_dlink);
		to_via = *enumerator.next();
		to = *enumerator.next();
	}

	*this = TurnGeo(from, from_via, to_via, to);
}

TurnDir classify_turn_direction(TurnGeo coords){
	double
		a_x = coords.from_vec.lat(),
		a_y = coords.from_vec.lon(),
		b_x = coords.to_vec.lat(),
		b_y = coords.to_vec.lon(),
		det = a_x*b_y - b_x*a_y,
		dotp = a_x*b_x + a_y*b_y,
		len_a = sqrt(a_x*a_x + a_y*a_y),
		len_b = sqrt(b_x*b_x + b_y*b_y),
		cos_ = dotp/(len_a*len_b);

	const double sqrt3div2 = sqrt(3.0)/2.0;
	const double sqrt2div2 = sqrt(2.0)/2.0;

	bool is_left = det <= 0;

	if(cos_ > sqrt2div2){
		return TurnDir::straight;
	}else if(cos_ > -0.5){
		return is_left ? TurnDir::left : TurnDir::right;
	}else if(cos_ > -sqrt3div2){
		return is_left ? TurnDir::sharp_left : TurnDir::sharp_right;
	}else{
		return TurnDir::uturn;
	}
}

bool has_turn_direction(TurnGeo coords, TurnDir dir){
	double
		a_x = coords.from_vec.lat(),
		a_y = coords.from_vec.lon(),
		b_x = coords.to_vec.lat(),
		b_y = coords.to_vec.lon(),
		det = a_x*b_y - b_x*a_y,
		dotp = a_x*b_x + a_y*b_y,
		len_a = sqrt(a_x*a_x + a_y*a_y),
		len_b = sqrt(b_x*b_x + b_y*b_y),
		cos_ = dotp/(len_a*len_b);

	bool is_left = det <= 0;

	const double
		cos_turn_threshold = 0.99;//sqrt(3.0)/2.0;

	switch(dir){
		case TurnDir::sharp_left: return is_left && -cos_turn_threshold <= cos_ && cos_ <= 0;
		case TurnDir::left: return is_left && -cos_turn_threshold <= cos_ && cos_ <= cos_turn_threshold;
		case TurnDir::straight: return 0.5 <= cos_;
		case TurnDir::right: return !is_left && -cos_turn_threshold <= cos_ && cos_ <= cos_turn_threshold;
		case TurnDir::sharp_right: return !is_left && -cos_turn_threshold <= cos_ && cos_ <= 0;
		case TurnDir::uturn: return cos_ <= -0.5;
		default:assert(false);return false;
	}

}

}
