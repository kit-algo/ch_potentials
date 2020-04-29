#ifndef ROUTING_KIT2_H
#define ROUTING_KIT2_H

#include "geo_index_schema.h"
#include "optional.h"
#include "map_schema.h"
#include "timestamp_flags.h"
#include <vector>
#include <limits>

namespace RoutingKit2{

class GeoIndexBuilder{
public:
	void add(GeoPos pos, uint32_t id){
		data.push_back({pos, id, 0});
	}

	VecGeoIndex build();

private:

    void compute_distance_from_first(std::size_t begin, std::size_t end)noexcept;
    void recurse_in_construction(std::size_t begin, std::size_t end)noexcept;
    void construct()noexcept;

	struct Data{
		GeoPos pos;
		uint32_t id;
		uint64_t distance_in_sqr_cm;
	};

	std::vector<Data>data;
};

struct GeoIndexItem{
	GeoPos pos;
	uint32_t id;
};

template<>
struct OptionalWithSentinelTraits<GeoIndexItem>{
	constexpr static GeoIndexItem get_invalid()noexcept{
		return {invalid_geo_pos, 0};
	}

	constexpr static bool is_invalid(const GeoIndexItem&x)noexcept{
		return x.pos == invalid_geo_pos;
	}
};

class GeoIndexFindAtLeastWithinRadiusQuery{
public:
	explicit GeoIndexFindAtLeastWithinRadiusQuery(ConstRefGeoIndex geo_index)noexcept:
		geo_index(geo_index){}

	void start(GeoPos center, uint32_t radius_in_cm);
	OptionalWithSentinel<GeoIndexItem> next();

	ConstRefGeoIndex get_geo_index()const noexcept{
		return geo_index;
	}

	uint32_t get_radius_in_cm()const noexcept{
		return radius_in_cm;
	}

	GeoPos get_center()const noexcept{
		return center;
	}

private:
	ConstRefGeoIndex geo_index;


	uint32_t radius_in_cm;
	GeoPos center;

	struct Frame{
		std::size_t begin;
		std::size_t end;
	};
	std::vector<Frame>frame_stack;
};

class GeoIndexFindWithinRadiusQuery{
public:
	explicit GeoIndexFindWithinRadiusQuery(ConstRefGeoIndex geo_index):
		search(geo_index){}

	void start(GeoPos center, uint32_t radius_in_cm);
	OptionalWithSentinel<GeoIndexItem> next();

	ConstRefGeoIndex get_geo_index()const noexcept{
		return search.get_geo_index();
	}

	uint32_t get_radius_in_cm()const noexcept{
		return search.get_radius_in_cm();
	}

	GeoPos get_center()const noexcept{
		return search.get_center();
	}

private:
	uint64_t radius_in_sqr_cm;
	GeoIndexFindAtLeastWithinRadiusQuery search;
};

VecGeoIndex build_link_geo_index(ConstRefLinkShapes);


class LinkGeoIndexFindWithinRadiusQuery{
public:
	LinkGeoIndexFindWithinRadiusQuery(ConstRefLinkShapes map, ConstRefGeoIndex geo_index):
		was_found(map.link_count), map(map), search(geo_index){}

	void start(GeoPos center, uint32_t radius_in_cm);
	Optional<uint32_t> next();

	ConstRefGeoIndex get_geo_index()const noexcept{
		return search.get_geo_index();
	}

	ConstRefLinkShapes get_link_shapes()const noexcept{
		return map;
	}

	uint32_t get_radius_in_cm()const noexcept;

	GeoPos get_center()const noexcept{
		return search.get_center();
	}

private:
	uint64_t radius_in_sqr_cm;
	TimestampFlags was_found;
	ConstRefLinkShapes map;
	GeoIndexFindAtLeastWithinRadiusQuery search;
};

}

#endif
