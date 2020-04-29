#include "geo_pos.h"
#include <cassert>

namespace RoutingKit2{

namespace {
	GeoPos p_add(GeoPos l, GeoPos r){
		GeoPos g;
		g.x = l.x + r.x;
		g.y = l.y + r.y;
		g.z = l.z + r.z;
		return g;
	}

	GeoPos p_diff(GeoPos l, GeoPos r){
		GeoPos g;
		g.x = l.x - r.x;
		g.y = l.y - r.y;
		g.z = l.z - r.z;
		return g;
	}

	int32_t i_scale(int32_t i, int64_t num, int64_t denom){
		return static_cast<int32_t>(static_cast<int64_t>(i) * num / denom);
	};

	GeoPos p_scale (GeoPos p, int64_t num, int64_t denom){
		p.x = i_scale(p.x, num, denom);
		p.y = i_scale(p.y, num, denom);
		p.z = i_scale(p.z, num, denom);
		return p;
	};

	int64_t i_mul(int32_t l, int32_t r){
		return static_cast<int64_t>(l) * static_cast<int64_t>(r);
	};

	int64_t dotp(GeoPos l, GeoPos r){
		return i_mul(l.x, r.x) + i_mul(l.y, r.y) + i_mul(l.z, r.z);
	};
}

GeoPos shift_geo_pos(GeoPos a, GeoPos b, int64_t num, int64_t denom)noexcept{
	return p_add(a, p_scale(p_diff(b, a), num, denom));
}

GeoPos find_closest_point_on_segment(GeoPos point, GeoPos line_a, GeoPos line_b)noexcept{
	assert(point != invalid_geo_pos);
	assert(line_a != invalid_geo_pos);
	assert(line_b != invalid_geo_pos);

	if(line_a == line_b)
		return line_a;


	GeoPos ap = p_diff(point, line_a);
	GeoPos ab = p_diff(line_b, line_a);

	int64_t num = dotp(ap, ab);

	if(num < 0){
		return line_a;
	}

	int64_t denom = dotp(ab, ab);

	if(denom < num){
		return line_b;
	}

	return p_add(line_a, p_scale(ab, num, denom));
}

}
