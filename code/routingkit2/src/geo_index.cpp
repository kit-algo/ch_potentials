#include "geo_index.h"
#include "geo_pos.h"
#include "polyline.h"
#include <algorithm>
#include <assert.h>

namespace RoutingKit2{

constexpr size_t max_points_per_leaf = 8;

void GeoIndexBuilder::compute_distance_from_first(size_t begin, size_t end)noexcept{
    for(size_t i=begin+1; i<end; ++i)
            data[i].distance_in_sqr_cm = compute_distance_in_sqr_cm(data[begin].pos, data[i].pos);
}

static size_t compute_mid(size_t begin, size_t end)noexcept{
    return (begin + end)/2;
}

// Preconditions:
//  * end - begin > max_points_per_leaf
//  * data[i].distance == compute_distance(data[begin].point, data[i].point for all i with begin < i < end
void GeoIndexBuilder::recurse_in_construction(size_t begin, size_t end)noexcept{
	size_t mid = compute_mid(begin, end);

	std::nth_element(
	    data.begin()+begin+1,
	    data.begin()+mid,
	    data.begin()+end,
	    [](const Data&l, const Data&r){
	            return l.distance_in_sqr_cm < r.distance_in_sqr_cm;
	    }
	);

	if(mid - begin > max_points_per_leaf){
	    recurse_in_construction(begin, mid);
	}

	if(end - mid > max_points_per_leaf){
	    compute_distance_from_first(mid, end);
	    recurse_in_construction(mid, end);
	}
}

void GeoIndexBuilder::construct()noexcept{
	size_t begin = 0, end = data.size();
	if(end-begin > max_points_per_leaf){
	    compute_distance_from_first(begin, end);
	    recurse_in_construction(begin, end);
	}
}

VecGeoIndex GeoIndexBuilder::build(){
	construct();
	VecGeoIndex r(data.size());
	for(size_t i=0; i<r.item_count; ++i){
		r.pos[i] = data[i].pos;
		r.id[i] = data[i].id;
	}
	return r;
}


void GeoIndexFindAtLeastWithinRadiusQuery::start(GeoPos center, uint32_t radius_in_cm){
	this->center = center;
	this->radius_in_cm = radius_in_cm;
	frame_stack.push_back({0, geo_index.item_count});
}

OptionalWithSentinel<GeoIndexItem> GeoIndexFindAtLeastWithinRadiusQuery::next(){
	OptionalWithSentinel<GeoIndexItem> ret;
	while(!frame_stack.empty()){
		size_t begin = frame_stack.back().begin;
		size_t end = frame_stack.back().end;

		size_t frame_size = end - begin;
		if(frame_size > max_points_per_leaf){
			frame_stack.pop_back();
			size_t mid = compute_mid(begin, end);
			uint32_t dist_pivot_to_mid_in_cm = compute_distance_in_cm(geo_index.pos[begin], geo_index.pos[mid]);
			uint32_t dist_pivot_to_center_in_cm = compute_distance_in_cm(geo_index.pos[begin], center);

			if(dist_pivot_to_center_in_cm <= dist_pivot_to_mid_in_cm + radius_in_cm)
				frame_stack.push_back({begin, mid});
			// +1 needed because dist_pivot_to_center_in_cm is the rounded down distance
			if(dist_pivot_to_mid_in_cm <= dist_pivot_to_center_in_cm + radius_in_cm + 1)
				frame_stack.push_back({mid, end});
		}else if(begin == end){
			frame_stack.pop_back();
		}else{
			ret = OptionalWithSentinel<GeoIndexItem>({geo_index.pos[begin], geo_index.id[begin]});
			++frame_stack.back().begin;
			break;
		}
	}

	return ret;
}

void GeoIndexFindWithinRadiusQuery::start(GeoPos center, uint32_t radius_in_cm){
	// compute_distance_in_sqr_cm does not round down in contrary to compute_distance_in_cm
	// +1 is therefore necessary to adjust for this asymmetry
	this->radius_in_sqr_cm = radius_in_cm+1;
	this->radius_in_sqr_cm *= radius_in_cm+1;
	search.start(center, radius_in_cm);
}

OptionalWithSentinel<GeoIndexItem> GeoIndexFindWithinRadiusQuery::next(){
	OptionalWithSentinel<GeoIndexItem> r;
	for(;;){
		r = search.next();
		if(!r.has_value())
			return r;
		if(compute_distance_in_sqr_cm(search.get_center(), r->pos) <= radius_in_sqr_cm)
			return r;
	}
}

namespace {
	constexpr uint32_t link_sampling_distance_in_cm = 50000;
}

VecGeoIndex build_link_geo_index(ConstRefLinkShapes map){
	GeoIndexBuilder builder;


std::vector<LatLon>poly;
	auto add = [&](GeoPos pos, uint32_t link_id){
		builder.add(pos, link_id);
	};

	for(uint32_t l=0; l<map.link_count; ++l){

		add(GeoPos(map.node_pos[map.link_tail[l]]), l);
		if(map.link_length_in_cm[l] >= link_sampling_distance_in_cm){

			uint32_t remaining_distance_in_cm = link_sampling_distance_in_cm;

			DLinkPointEnumerator enumerator(map, link_to_forward_dlink(l));
			GeoPos prev(*enumerator.next());
			while(auto now_ = enumerator.next()){
				GeoPos now(*now_);
				uint32_t dist = compute_distance_in_cm(prev, now);
				while(remaining_distance_in_cm < dist){
					add(shift_geo_pos(prev, now, remaining_distance_in_cm, dist), l);
					remaining_distance_in_cm += link_sampling_distance_in_cm;
				}

				remaining_distance_in_cm -= dist;
				prev = now;
			}
		}
	}

	return builder.build();
}

void LinkGeoIndexFindWithinRadiusQuery::start(GeoPos center, uint32_t radius_in_cm){
	// compute_distance_in_sqr_cm does not round down in contrary to compute_distance_in_cm
	// +1 is therefore necessary to adjust for this asymmetry
	this->radius_in_sqr_cm = radius_in_cm+1;
	this->radius_in_sqr_cm *= radius_in_cm+1;

	search.start(center, radius_in_cm + link_sampling_distance_in_cm);
	was_found.reset_all();
}

uint32_t LinkGeoIndexFindWithinRadiusQuery::get_radius_in_cm()const noexcept{
	return search.get_radius_in_cm() - link_sampling_distance_in_cm;
}

Optional<uint32_t> LinkGeoIndexFindWithinRadiusQuery::next(){
	Optional<uint32_t> ret;
	while(auto cand = search.next()){
		if(!was_found.is_set(cand->id)){
			uint32_t link_id = cand->id;
			was_found.set(link_id);

			uint64_t distance_to_tail_in_sqr_cm = compute_distance_in_sqr_cm(GeoPos(map.node_pos[map.link_tail[link_id]]), search.get_center());

			if(distance_to_tail_in_sqr_cm < radius_in_sqr_cm){
				ret = Optional<uint32_t>(link_id);
				return ret;
			}

			uint64_t d = sqr_cm_to_cm(distance_to_tail_in_sqr_cm);
			d -= map.link_length_in_cm[link_id];
			d *= d;
			if(d > radius_in_sqr_cm)
				continue;

			auto pd = find_closest_point_and_distance_on_link(map, link_id, search.get_center());
			if(pd.distance_in_sqr_cm < radius_in_sqr_cm){
				ret = Optional<uint32_t>(link_id);
				return ret;
			}
		}
	}
	assert(!ret.has_value());
	return ret;
}


}
