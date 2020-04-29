#ifndef ROUTING_KIT2_GEO_POS_H
#define ROUTING_KIT2_GEO_POS_H

#include <utility>
#include <stdint.h>
#include <math.h>

namespace RoutingKit2{

struct LatLon;
struct GeoPos;

struct LatLon{
	int32_t lat_in_decamicrodeg;
	int32_t lon_in_decamicrodeg;

	double lat() const noexcept {
		return static_cast<double>(lat_in_decamicrodeg) * (1.0/100000.0);
	}

	double lon() const noexcept {
		return static_cast<double>(lon_in_decamicrodeg) * (1.0/100000.0);
	}

private:
	constexpr LatLon(int32_t lat_in_decamicrodeg, int32_t lon_in_decamicrodeg)noexcept:
		lat_in_decamicrodeg(lat_in_decamicrodeg), lon_in_decamicrodeg(lon_in_decamicrodeg){}

public:
	LatLon()noexcept{}
	explicit LatLon(GeoPos o)noexcept;

	// no LatLon(double lat, double lon) as people constantly forget
	// the order of the lat and lon parameter. We therefore have more
	// descriptive static construction functions.

	constexpr static LatLon from_lat_lon_in_decamicrodeg(int32_t lat, int32_t lon)noexcept{
		return {lat, lon};
	}

	constexpr static LatLon from_lon_lat_in_decamicrodeg(int32_t lon, int32_t lat)noexcept{
		return from_lat_lon_in_decamicrodeg(lat, lon);
	}

	static LatLon from_lat_lon(double lat, double lon)noexcept{
		return from_lat_lon_in_decamicrodeg(
			static_cast<int32_t>(round(lat*100000.0)),
			static_cast<int32_t>(round(lon*100000.0))
		);
	}

	static LatLon from_lon_lat(double lon, double lat)noexcept{
		return from_lat_lon(lat, lon);
	}
};

constexpr LatLon invalid_lat_lon = LatLon::from_lat_lon_in_decamicrodeg(
	static_cast<int32_t>(0xFFFFFFFF),
	static_cast<int32_t>(0xFFFFFFFF)
);

constexpr LatLon invalid_lon_lat = invalid_lat_lon;

constexpr LatLon null_lat_lon = LatLon::from_lat_lon_in_decamicrodeg(0,0);

constexpr LatLon null_lon_lat = null_lat_lon;

constexpr bool operator==(LatLon l, LatLon r)noexcept{
	return l.lat_in_decamicrodeg == r.lat_in_decamicrodeg && l.lon_in_decamicrodeg == r.lon_in_decamicrodeg;
}

constexpr bool operator!=(LatLon l, LatLon r)noexcept{
	return !(l == r);
}

struct GeoPos{
	int32_t x,y,z;

	GeoPos()noexcept{}
	explicit GeoPos(LatLon ll)noexcept;
	constexpr GeoPos(int32_t x, int32_t y, int32_t z)noexcept:
		x(x), y(y), z(z){}

};

constexpr GeoPos invalid_geo_pos = {0,0,0};

constexpr bool operator==(GeoPos l, GeoPos r)noexcept{
	return l.x == r.x && l.y == r.y && l.z == r.z;
}

constexpr bool operator!=(GeoPos l, GeoPos r)noexcept{
	return !(l == r);
}

inline GeoPos::GeoPos(LatLon ll)noexcept{
	const double
		earth_radius_in_cm = 637100080,
		decamicrodeg_to_rad = 3.14159265359*(1.0/18000000.0),
		cos_lat = cos(ll.lat_in_decamicrodeg*decamicrodeg_to_rad),
		cos_lon = cos(ll.lon_in_decamicrodeg*decamicrodeg_to_rad),
		sin_lat = sin(ll.lat_in_decamicrodeg*decamicrodeg_to_rad),
		sin_lon = sin(ll.lon_in_decamicrodeg*decamicrodeg_to_rad);

	z = static_cast<int32_t>(round(earth_radius_in_cm*sin_lat));
	x = static_cast<int32_t>(round(earth_radius_in_cm*cos_lon*cos_lat));
	y = static_cast<int32_t>(round(earth_radius_in_cm*sin_lon*cos_lat));
}

inline LatLon::LatLon(GeoPos o)noexcept{
	const double
		earth_radius_in_cm = 637100080,
		rad_to_decamicrodeg = (1.0/3.14159265359)*18000000.0;
	lat_in_decamicrodeg = rad_to_decamicrodeg*asin(static_cast<double>(o.z)/earth_radius_in_cm);
	lon_in_decamicrodeg = rad_to_decamicrodeg*atan2(static_cast<double>(o.y),static_cast<double>(o.x));
}

inline uint64_t compute_distance_in_sqr_cm(GeoPos a, GeoPos b)noexcept{
	auto sqr = [](int32_t x) -> uint64_t {
		return static_cast<uint64_t>(static_cast<int64_t>(x)*static_cast<int64_t>(x));
	};

	return sqr(a.x-b.x) + sqr(a.y-b.y) + sqr(a.z-b.z);
}

inline uint32_t sqr_cm_to_cm(uint64_t sqr_cm)noexcept{
	return static_cast<uint32_t>(floor(sqrt(sqr_cm)));
}

inline uint32_t compute_distance_in_cm(GeoPos a, GeoPos b)noexcept{
	return sqr_cm_to_cm(compute_distance_in_sqr_cm(a, b));
}

//! Shifts a along the vector from a to b by num/denom times the distance from a to b.
GeoPos shift_geo_pos(GeoPos a, GeoPos b, int64_t num, int64_t denom)noexcept;

GeoPos find_closest_point_on_segment(GeoPos point, GeoPos line_a, GeoPos line_b)noexcept;

template<class T>
struct OptionalWithSentinelTraits;

template<>
struct OptionalWithSentinelTraits<LatLon>{
	constexpr static LatLon get_invalid()noexcept{
		return invalid_lat_lon;
	}

	constexpr  static bool is_invalid(const LatLon&x)noexcept{
		return x == invalid_lat_lon;
	}
};

template<>
struct OptionalWithSentinelTraits<GeoPos>{
	constexpr static GeoPos get_invalid()noexcept{
		return invalid_geo_pos;
	}

	constexpr  static bool is_invalid(const GeoPos&x)noexcept{
		return x == invalid_geo_pos;
	}
};


} // namespace RoutingKit2

#endif
