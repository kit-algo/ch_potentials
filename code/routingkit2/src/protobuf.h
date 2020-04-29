#ifndef ROUTING_KIT2_PROTOBUF_H
#define ROUTING_KIT2_PROTOBUF_H

#include "protobuf_var_int.h"

#include <stdint.h>
#include <stdexcept>
#include <string.h>

namespace RoutingKit2{

namespace detail{
	template<class VarIntCallback, class DoubleCallback, class BlobCallback, class UInt8>
	void decode_protobuf_message_with_callbacks(
		UInt8*begin, UInt8*end,
		const VarIntCallback&varint_callback,
		const DoubleCallback&double_callback,
		const BlobCallback&blob_callback
	){
		while(begin != end){
			uint64_t header = decode_var_uint_from_front_and_advance(begin, end);
			uint64_t field_id = header >> 3;
			uint64_t wire_type = header & ((1<<3)-1);
			switch(wire_type){
				case 0:
				{
					varint_callback(field_id, decode_var_uint_from_front_and_advance(begin, end));
					break;
				}
				case 1:
				{
					if(end - begin < 8)
						throw std::runtime_error("Protobuf message is corrupt, the end of message was reached while parsing a 64-bit floating point.");
					static_assert(sizeof(double) == 8, "double must be 8 bytes long");
					double arg;
					memcpy(&arg, begin, sizeof(arg));
					double_callback(field_id, arg);
					begin += 8;
					break;
				}
				case 5:
				{
					if(end - begin < 4)
						throw std::runtime_error("Protobuf message is corrupt, the end of message was reached while parsing a 32-bit floating point.");
					static_assert(sizeof(float) == 4, "float must be 8 bytes long");
					float arg;
					memcpy(&arg, begin, sizeof(arg));
					double_callback(field_id, *reinterpret_cast<const float*>(begin));
					begin += 4;
					break;
				}
				case 2:
				{
					uint64_t len = decode_var_uint_from_front_and_advance(begin, end);
					if((uint64_t)(end - begin) < len)
						throw std::runtime_error("Protobuf message is corrupt, the end of message was reached while parsing a string or embedded message.");
					blob_callback(field_id, begin, begin+len);
					begin += len;
					break;
				}


				default:
					throw std::runtime_error("Protobuf message contains unknown wire type "+std::to_string(wire_type)+".");
			}
		}
	}
}

//! Iterates over a protobuf message and calls the respective callbacks depending
//! on the interpretation of the wireformat.
//!
//! The signatures of the callbacks are:
//!
//! void varint_callback(uint64_t);
//! void double_callback(double);
//! void blob_callback(const uint8_t*blob_begin, const uint8_t*blob_end);
//!
//! `blob_begin` and `blob_end` are guarenteed to be a subrange of `begin` and `end`.
template<class VarIntCallback, class DoubleCallback, class BlobCallback>
void decode_protobuf_message_with_callbacks(
	const uint8_t*begin, const uint8_t*end,
	const VarIntCallback&varint_callback,
	const DoubleCallback&double_callback,
	const BlobCallback&blob_callback
){
	detail::decode_protobuf_message_with_callbacks(
		begin, end,
		varint_callback, double_callback, blob_callback
	);
}

//! See variant of `decode_protobuf_message_with_callbacks` with const parameters for description.
//!
//! If the buffer is passed as writable, the missing constness is propagated to the blob_callback.
//! Its signature is
//!
//! void blob_callback(uint8_t*blob_begin, uint8_t*blob_end);
//!
//! The `decode_protobuf_message_with_callbacks` function does not modify buffer.
//! However, the `blob_callback` is allowed to do so.
//! It may overwrite all bytes between `begin` and `blob_end` without affecting
//! `decode_protobuf_message_with_callbacks`. Because of the way the protobuf format is specified,
//! it is guarenteed that there is a byte before `blob_begin` and that it is not part
//! of any other blob. This can be useful, if the blob should be transformed into a
//! null-terminated string: Move all bytes by one byte and set the 0-byte.
//!
template<class VarIntCallback, class DoubleCallback, class BlobCallback>
void decode_protobuf_message_with_callbacks(
	uint8_t*begin, uint8_t*end,
	const VarIntCallback&varint_callback,
	const DoubleCallback&double_callback,
	const BlobCallback&blob_callback
){
	detail::decode_protobuf_message_with_callbacks(
		begin, end,
		varint_callback, double_callback, blob_callback
	);
}



}

#endif
