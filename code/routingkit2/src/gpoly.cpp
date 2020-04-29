#include "gpoly.h"
#include "polyline.h"

#include <iostream>
using namespace std;

namespace RoutingKit2{
	namespace {
		uint64_t vali_to_valu(int64_t vali)noexcept{
			uint64_t valu;
			if(vali >= 0){
				valu = vali;
				valu <<= 1;
			}else{
				valu = vali;
				valu <<= 1;
				valu = ~valu;
			}
			return valu;
		}

		int64_t valu_to_vali(uint64_t valu)noexcept{
			int64_t vali;
			if((valu & 1) != 0){
				valu = ~valu;
				vali = valu;
				vali /= 2;
			}else{
				valu >>= 1;
				vali = valu;
			}
			return vali;
		}
	}


	GPolyVarIntDecodeResult decode_gpoly_var_int_from_front_without_advance(const char*begin, const char*end){
		const char*pos = begin;
		uint64_t valu = 0;
		constexpr uint64_t mask = (1u<<5) - 1;
		uint32_t shift = 0;

		for(;;){
			if(pos == end){
				throw std::runtime_error("cannot decode gpoly because string ends before var int");
			}
			uint64_t group = *pos - 63;
			++pos;
			valu |= (group & mask) << shift;
			shift += 5;
			if((group >> 5) == 0)
				break;
		}
		return {valu_to_vali(valu), static_cast<size_t>(pos - begin)};
	}


	size_t compute_gpoly_var_int_size(int64_t vali)noexcept{
		uint64_t valu = vali_to_valu(vali);
		size_t size;
		if(valu == 0)
			size = 1;
		else
			size = (64-__builtin_clzll(valu)+4)/5;
		assert(size <= max_gpoly_var_int_size);
		return size;
	}

	char*encode_gpoly_var_int(char*buf, int64_t vali)noexcept{
		uint64_t valu = vali_to_valu(vali);
		constexpr uint64_t mask = (1u<<5) - 1;
		do{
			uint64_t group = valu & mask;
			valu >>= 5;
			if(valu != 0)
				group |= 0x20;
			group += 63;

			*buf = group;
			++buf;
		}while(valu != 0);

		return buf;

	}

	void decode_gpoly(const std::string&gpoly, std::vector<LatLon>&polyline){
		polyline.clear();
		GPolyLatLonEnumerator enumerator(&gpoly[0], &gpoly[0]+gpoly.size());
		while(auto next = enumerator.next())
			polyline.push_back(*next);
	}


}
