#ifndef ROUTING_KIT2_POLYLINE_H
#define ROUTING_KIT2_POLYLINE_H

#include "geo_pos.h"
#include "protobuf_var_int.h"
#include "map.h"
#include "optional.h"

namespace RoutingKit2{

	//! Iterates over the points of a directed link.
	//! It can be used as follows:
	//!
	//! DLinkPointEnumeratorWithoutLast enumerator(map, dlink_id);
	//! while(!enumerator.is_finished()){
	//!   LatLon pos = enumerator.next();
	//! }
	//!
	//! All shape points are enumerated from tail to head.
	//! The tail position is included in the enumeration.
	//! The position of the head is not included in the enumeration.
	class DLinkPointEnumeratorWithoutLast{
	public:
		DLinkPointEnumeratorWithoutLast():p(invalid_lat_lon){}

		DLinkPointEnumeratorWithoutLast(ConstRefLinkShapes map, uint32_t dlink_id){
			assert(dlink_id < 2*map.link_count);
			uint32_t link_id = dlink_to_link(dlink_id);
			if(is_forward_dlink(dlink_id)){
				p = map.node_pos[map.link_tail[link_id]];
				shape_pos_bound1 = map.shape_pos + map.first_shape_pos_of_link[link_id];
				shape_pos_bound2 = map.shape_pos + map.first_shape_pos_of_link[link_id+1];
			}else{
				p = map.node_pos[map.link_head[link_id]];
				shape_pos_bound1 = map.shape_pos + map.first_shape_pos_of_link[link_id+1];
				shape_pos_bound2 = map.shape_pos + map.first_shape_pos_of_link[link_id];
			}
		}

		OptionalWithSentinel<LatLon> next()noexcept{
			LatLon ret = p;
			if(shape_pos_bound1 < shape_pos_bound2){
				p = *shape_pos_bound1;
				++shape_pos_bound1;
			}else if(shape_pos_bound1 > shape_pos_bound2){
				--shape_pos_bound1;
				p = *shape_pos_bound1;
			}else{
				p = invalid_lat_lon;
			}
			return OptionalWithSentinel<LatLon>(ret);
		}

	private:
		LatLon p;
		const LatLon*shape_pos_bound1;
		const LatLon*shape_pos_bound2;
	};

	class DLinkPointEnumerator{
	public:
		DLinkPointEnumerator():last(invalid_lat_lon){}

		DLinkPointEnumerator(ConstRefLinkShapes map, uint32_t dlink_id):
			enumerator(map, dlink_id),
			last(map.node_pos[dlink_head(map, dlink_id)]){
			assert(dlink_id < 2*map.link_count);
		}

		bool is_finished()const noexcept{
			return last == invalid_lat_lon;
		}

		OptionalWithSentinel<LatLon> next()noexcept{
			auto ret = enumerator.next();
			if(!ret.has_value()){
				ret = OptionalWithSentinel<LatLon>(last);
				last = invalid_lat_lon;
			}
			return ret;
		}

	private:
		DLinkPointEnumeratorWithoutLast enumerator;
		LatLon last;
	};

	class DLinkPathPointEnumeratorWithoutLast{
	public:
		DLinkPathPointEnumeratorWithoutLast():begin_dlink(nullptr), end_dlink(nullptr){}

		DLinkPathPointEnumeratorWithoutLast(ConstRefLinkShapes map, Span<const uint32_t>dlink_list):
			map(map){
			assert(is_dlink_path(map, dlink_list));
			begin_dlink = &dlink_list[0];
			end_dlink = &dlink_list[0]+dlink_list.size();
		}

		OptionalWithSentinel<LatLon> next()noexcept{
			OptionalWithSentinel<LatLon>ret;
			if((ret = enumerator.next()))
				return ret;
			if(begin_dlink != end_dlink){
				enumerator = DLinkPointEnumeratorWithoutLast(map, *begin_dlink);
				++begin_dlink;
				ret = enumerator.next();
			}
			return ret;
		}

	private:
		ConstRefLinkShapes map;
		DLinkPointEnumeratorWithoutLast enumerator;
		const uint32_t*begin_dlink;
		const uint32_t*end_dlink;
	};

	class DLinkPathPointEnumerator{
	public:
		DLinkPathPointEnumerator():last(invalid_lat_lon){}

		DLinkPathPointEnumerator(ConstRefLinkShapes map, Span<const uint32_t> dlink_list):
			enumerator(map, dlink_list){
			assert(is_dlink_path(map, dlink_list));
			if(dlink_list.empty())
				last = invalid_lat_lon;
			else
				last = map.node_pos[dlink_head(map, dlink_list.back())];
		}

		OptionalWithSentinel<LatLon> next()noexcept{
			OptionalWithSentinel<LatLon> ret = enumerator.next();
			if(!ret.has_value()){
				ret = OptionalWithSentinel<LatLon>(last);
				last = invalid_lat_lon;
			}
			return ret;
		}

	private:
		LatLon last;
		DLinkPathPointEnumeratorWithoutLast enumerator;
	};

}


#endif
