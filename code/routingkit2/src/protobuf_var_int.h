#ifndef ROUTING_KIT2_PROTOBUF_VAR_INT_H
#define ROUTING_KIT2_PROTOBUF_VAR_INT_H

#include <stdint.h>
#include <stdlib.h>

namespace RoutingKit2{
	struct VarIntDecodeResult{
		uint64_t val;
		size_t offset;
	};

	VarIntDecodeResult decode_var_uint_from_front_without_advance(const uint8_t*begin, const uint8_t*end);
	VarIntDecodeResult decode_var_uint_from_back_without_advance(const uint8_t*begin, const uint8_t*end);

	uint64_t decode_var_uint_from_front_and_advance(const uint8_t*&begin, const uint8_t*end);
	uint64_t decode_var_uint_from_front_and_advance(uint8_t*&begin, const uint8_t*end);
	uint64_t decode_var_uint_from_back_and_advance(const uint8_t*begin, const uint8_t*&end);
	uint64_t decode_var_uint_from_back_and_advance(const uint8_t*begin, uint8_t*&end);

	size_t compute_var_uint_size(uint64_t);

	uint8_t* encode_var_uint(uint8_t*dest, uint64_t x);

	int64_t zigzag_convert_uint64_to_int64(uint64_t x);
	uint64_t zigzag_convert_int64_to_uint64(int64_t x);


	inline uint64_t decode_var_uint_from_front_and_advance(const uint8_t*&begin, const uint8_t*end){
		auto r = decode_var_uint_from_front_without_advance(begin, end);
		begin += r.offset;
		return r.val;
	}

	inline uint64_t decode_var_uint_from_front_and_advance(uint8_t*&begin, const uint8_t*end){
		auto r = decode_var_uint_from_front_without_advance(begin, end);
		begin += r.offset;
		return r.val;
	}

	inline uint64_t decode_var_uint_from_back_and_advance(const uint8_t*begin, const uint8_t*&end){
		auto r = decode_var_uint_from_back_without_advance(begin, end);
		end -= r.offset;
		return r.val;
	}

	inline uint64_t decode_var_uint_from_back_and_advance(const uint8_t*begin, uint8_t*&end){
		auto r = decode_var_uint_from_back_without_advance(begin, end);
		end -= r.offset;
		return r.val;
	}

}

#endif
