#ifndef ROUTING_KIT2_BIT_SELECT_H
#define ROUTING_KIT2_BIT_SELECT_H

#include <stdint.h>

namespace RoutingKit2{

//! returns the 0-based offset of the (n+1)-th bit set in a 64 bit data block
uint32_t uint64_bit_select(uint64_t data, uint32_t n);

//! returns the 0-based offset of the (n+1)-th bit set in a 512 bit data block
uint32_t uint512_bit_select(const uint64_t*data, uint32_t n);

//! returns the 0-based offset of the (n+1)-th bit set in a sequence of 512-bit data block given a rank index.
//! uint512_rank[i] contains the number of bits set in the 0..i-1 512-bit data blocks.
uint64_t bit_select(uint64_t uint512_count, const uint64_t*uint512_rank, const uint64_t*data, uint64_t n);

} // namespace RoutingKit2

#endif
