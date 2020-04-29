#ifndef ROUTING_KIT2_GPOLY_H
#define ROUTING_KIT2_GPOLY_H

#include "geo_pos.h"
#include "enumerator.h"
#include "map.h"
#include "polyline.h"
#include "optional.h"

#include <string>
#include <vector>

namespace RoutingKit2{

	struct GPolyVarIntDecodeResult{
		int64_t val;
		size_t offset;
	};

	GPolyVarIntDecodeResult decode_gpoly_var_int_from_front_without_advance(const char*begin, const char*end);

	uint64_t decode_gpoly_var_int_from_front_and_advance(const char*&begin, const char*end);
	uint64_t decode_gpoly_var_int_from_front_and_advance(char*&begin, const char*end);

	constexpr size_t max_gpoly_var_int_size = 13;
	size_t compute_gpoly_var_int_size(int64_t)noexcept;
	char*encode_gpoly_var_int(char*buf, int64_t val)noexcept;

	std::string encode_gpoly_from_span(Span<const LatLon>polyline);
	std::string encode_gpoly_from_dlink(ConstRefLinkShapes map, uint32_t dlink);
	std::string encode_gpoly_from_dlink_path(ConstRefLinkShapes map, Span<const uint32_t> dlink);
	template<class Enumerator>
	std::string encode_gpoly_from_enumerator(Enumerator enumerator);

	void encode_gpoly_from_span(Span<const LatLon>polyline, std::string&out);
	void encode_gpoly_from_dlink(ConstRefLinkShapes map, uint32_t dlink, std::string&out);
	void encode_gpoly_from_dlink_path(ConstRefLinkShapes map, Span<const uint32_t> dlink, std::string&out);
	template<class Enumerator>
	void encode_gpoly_from_enumerator(Enumerator enumerator, std::string&out);

	std::vector<LatLon> decode_gpoly(const std::string&gpoly);
	void decode_gpoly(const std::string&gpoly, std::vector<LatLon>&polyline);

	class GPolyLatLonEnumerator{
	public:
		GPolyLatLonEnumerator()noexcept{}
		GPolyLatLonEnumerator(const char*begin, const char*end)noexcept:
			begin(begin), end(end),
			prev(LatLon::from_lat_lon_in_decamicrodeg(0,0)){}

		bool is_finished()const noexcept{
			return begin == end;
		}

		Optional<LatLon> next(){
			if(begin == end)
				return Optional<LatLon>();

			LatLon r;
			r.lat_in_decamicrodeg = decode_gpoly_var_int_from_front_and_advance(begin, end);
			r.lon_in_decamicrodeg = decode_gpoly_var_int_from_front_and_advance(begin, end);
			r.lat_in_decamicrodeg += prev.lat_in_decamicrodeg;
			r.lon_in_decamicrodeg += prev.lon_in_decamicrodeg;
			prev = r;
			return Optional<LatLon>(r);
		}

	private:
		const char*begin, *end;
		LatLon prev;
	};

	inline uint64_t decode_gpoly_var_int_from_front_and_advance(const char*&begin, const char*end){
		auto r = decode_gpoly_var_int_from_front_without_advance(begin, end);
		begin += r.offset;
		return r.val;
	}

	inline uint64_t decode_gpoly_var_int_from_front_and_advance(char*&begin, const char*end){
		auto r = decode_gpoly_var_int_from_front_without_advance(begin, end);
		begin += r.offset;
		return r.val;
	}

	inline std::string encode_gpoly_from_span(Span<const LatLon>polyline){
		std::string out;
		encode_gpoly_from_span(polyline, out);
		return out;
	}

	inline std::string encode_gpoly_from_dlink(ConstRefLinkShapes map, uint32_t dlink){
		std::string out;
		encode_gpoly_from_dlink(map, dlink, out);
		return out;
	}

	inline std::string encode_gpoly_from_dlink_path(ConstRefLinkShapes map, Span<const uint32_t> dlink){
		std::string out;
		encode_gpoly_from_dlink_path(map, dlink, out);
		return out;
	}

	template<class Enumerator>
	std::string encode_gpoly_from_enumerator(Enumerator enumerator){
		std::string out;
		encode_gpoly_from_enumerator(std::move(enumerator), out);
		return out;
	}

	inline std::vector<LatLon> decode_gpoly(const std::string&gpoly){
		std::vector<LatLon> out;
		decode_gpoly(gpoly, out);
		return out;
	}

	class GPolyStringBuilder{
	public:
		explicit GPolyStringBuilder(std::string&buffer):buffer(buffer){
			if(buffer.size() < 128)
				buffer.resize(128);
			buf_begin = &buffer[0];
			buf_end = buf_begin+buffer.size();
			gpoly_end = buf_begin;
		}

		void append(int64_t val){
			if(gpoly_end + max_gpoly_var_int_size > buf_end){
				uint64_t len = gpoly_end - buf_begin;
				buffer.resize(buffer.size()*3/2);
				buf_begin = &buffer[0];
				buf_end = buf_begin+buffer.size();
				gpoly_end = buf_begin + len;
			}
			gpoly_end = encode_gpoly_var_int(gpoly_end, val);
		}

		void finish(){
			buffer.resize(gpoly_end-buf_begin);
		}
	private:
		std::string&buffer;
		char*gpoly_end;
		char*buf_begin;
		char*buf_end;
	};


	template<class Enumerator>
	void encode_gpoly_from_enumerator(Enumerator enumerator, std::string&out){
		GPolyStringBuilder builder(out);
		LatLon prev = null_lat_lon;
		while(auto now = enumerator.next()){
			builder.append(now->lat_in_decamicrodeg - prev.lat_in_decamicrodeg);
			builder.append(now->lon_in_decamicrodeg - prev.lon_in_decamicrodeg);
			prev = *now;
		}
		builder.finish();
	}

	inline void encode_gpoly_from_span(Span<const LatLon>poly, std::string&out){
		encode_gpoly_from_enumerator(SpanEnumerator<LatLon>(poly), out);
	}

	inline void encode_gpoly_from_dlink(ConstRefLinkShapes map, uint32_t dlink, std::string&out){
		encode_gpoly_from_enumerator(DLinkPointEnumerator(map, dlink), out);
	}

	inline void encode_gpoly_from_dlink(ConstRefLinkShapes map, Span<const uint32_t> dlink, std::string&out){
		encode_gpoly_from_enumerator(DLinkPathPointEnumerator(map, dlink), out);
	}

}

#endif
