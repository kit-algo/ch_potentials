#include "protobuf_var_int.h"
#include <stdexcept>

namespace RoutingKit2{

const uint8_t high_bit = 1u<<7;
const uint8_t low_bits = static_cast<uint8_t>(~high_bit);

VarIntDecodeResult decode_var_uint_from_front_without_advance(const uint8_t*begin, const uint8_t*end){
	uint64_t n = 0;
	uint64_t shift = 0;

	const uint8_t*now = begin;

	for(;;){
		if(now == end)
			throw std::runtime_error("cannot decode varint as end of data is reached before end of varint");
		n |= static_cast<uint64_t>(*now & low_bits) << shift;
		if(*now & high_bit){
			++now;
			shift += 7;
		}else{
			++now;
			return VarIntDecodeResult{n, static_cast<size_t>(now - begin)};
		}
	}
}

VarIntDecodeResult decode_var_uint_from_back_without_advance(const uint8_t*begin, const uint8_t*end){
	uint64_t n = 0;

	if(begin == end)
		throw std::runtime_error("cannot decode varint as begin of data is reached before begin of varint");

	const uint8_t*now = end;

	--now;

	if(*now & high_bit)
		throw std::runtime_error("cannot decode varint as end of data is reached before end of varint");

	for(;;){
		n <<= 7;
		n |= static_cast<uint64_t>(*now & low_bits);

		if(begin == now || (*(now-1) & high_bit) == 0)
			return VarIntDecodeResult{n, static_cast<size_t>(end - now)};
		else
			--now;
	}
}

size_t compute_var_uint_size(uint64_t x){
	if(x == 0)
		return 1;
	else
		return (70 - __builtin_clzll(x))/7;
}

uint8_t* encode_var_uint(uint8_t*dest, uint64_t x){
	for(;;){
		*dest = x & low_bits;
		x >>= 7;
		if(x){
			*dest |= high_bit;
			++dest;
		}else{
			return dest+1;
		}
	}
}

int64_t zigzag_convert_uint64_to_int64(uint64_t x) {
	if((x & 1))
		return -static_cast<int64_t>(x>>1)-1;
	else
		return static_cast<int64_t>(x>>1);
}

uint64_t zigzag_convert_int64_to_uint64(int64_t x){
	if(x >= 0){
		return static_cast<uint64_t>(x) << 1;
	}else{
		return (static_cast<uint64_t>(-(x+1)) << 1) | 1;
	}
}



}
