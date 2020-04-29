#include "bit_select.h"
#include "catch.hpp"

using namespace RoutingKit2;

TEST_CASE("uint64_bit_select", "[BitSelect]")
{
	// 232226 == 00111000101100100010 binary
	REQUIRE(uint64_bit_select(232226, 0) == 1);
	REQUIRE(uint64_bit_select(232226, 1) == 5);
	REQUIRE(uint64_bit_select(232226, 2) == 8);
	REQUIRE(uint64_bit_select(232226, 3) == 9);
	REQUIRE(uint64_bit_select(232226, 4) == 11);
	REQUIRE(uint64_bit_select(232226, 5) == 15);
	REQUIRE(uint64_bit_select(232226, 6) == 16);
	REQUIRE(uint64_bit_select(232226, 7) == 17);

	REQUIRE(uint64_bit_select(0xF, 0) == 0);
	REQUIRE(uint64_bit_select(0xF, 1) == 1);
	REQUIRE(uint64_bit_select(0xF, 2) == 2);
	REQUIRE(uint64_bit_select(0xF, 3) == 3);

	REQUIRE(uint64_bit_select(0xFF, 4) == 4);
	REQUIRE(uint64_bit_select(0xFF, 5) == 5);
	REQUIRE(uint64_bit_select(0xFF, 6) == 6);
	REQUIRE(uint64_bit_select(0xFF, 7) == 7);

	REQUIRE(uint64_bit_select(0xFF02, 0) == 1);
	REQUIRE(uint64_bit_select(0xFF02, 1) == 8);
	REQUIRE(uint64_bit_select(0xFF02, 2) == 9);
	REQUIRE(uint64_bit_select(0xFF02, 3) == 10);
	REQUIRE(uint64_bit_select(0xFF02, 4) == 11);
	REQUIRE(uint64_bit_select(0xFF02, 5) == 12);
	REQUIRE(uint64_bit_select(0xFF02, 6) == 13);
	REQUIRE(uint64_bit_select(0xFF02, 7) == 14);
	REQUIRE(uint64_bit_select(0xFF02, 8) == 15);

	REQUIRE(uint64_bit_select(0xFFFF02, 15) == 22);
	REQUIRE(uint64_bit_select(0xFFFF02, 16) == 23);
}

TEST_CASE("uint512_bit_select1", "[BitSelect]"){
	uint64_t block[] = {0xF, 232226, 232226, 0, 0, 0, 0, 0};
	REQUIRE(uint512_bit_select(block, 0) == 0);
	REQUIRE(uint512_bit_select(block, 1) == 1);
	REQUIRE(uint512_bit_select(block, 2) == 2);
	REQUIRE(uint512_bit_select(block, 3) == 3);

	REQUIRE(uint512_bit_select(block, 4+0) == 64+1);
	REQUIRE(uint512_bit_select(block, 4+1) == 64+5);
	REQUIRE(uint512_bit_select(block, 4+2) == 64+8);
	REQUIRE(uint512_bit_select(block, 4+3) == 64+9);
	REQUIRE(uint512_bit_select(block, 4+4) == 64+11);
	REQUIRE(uint512_bit_select(block, 4+5) == 64+15);
	REQUIRE(uint512_bit_select(block, 4+6) == 64+16);
	REQUIRE(uint512_bit_select(block, 4+7) == 64+17);

	REQUIRE(uint512_bit_select(block, 12+0) == 128+1);
	REQUIRE(uint512_bit_select(block, 12+1) == 128+5);
	REQUIRE(uint512_bit_select(block, 12+2) == 128+8);
	REQUIRE(uint512_bit_select(block, 12+3) == 128+9);
	REQUIRE(uint512_bit_select(block, 12+4) == 128+11);
	REQUIRE(uint512_bit_select(block, 12+5) == 128+15);
	REQUIRE(uint512_bit_select(block, 12+6) == 128+16);
	REQUIRE(uint512_bit_select(block, 12+7) == 128+17);
}

TEST_CASE("uint512_bit_select2", "[BitSelect]"){
	uint64_t block[] = {0, 0, 0, 0, 0, 1, 0, 0};
	REQUIRE(uint512_bit_select(block, 0) == 5*64);
}

TEST_CASE("bit_select", "[BitSelect]"){

	uint64_t uint512_count = 14;

	uint64_t data[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 0, 0, 0
	};

	uint64_t rank[] = {
		0,0,1,1,1,2,2,2,2,3,3,3,3,3,4
	};

	REQUIRE(uint512_count*8 == sizeof(data)/8);
	REQUIRE(uint512_count+1 == sizeof(rank)/8);

	REQUIRE(bit_select(uint512_count, rank, data, 0) == 1*512+0*64);
	REQUIRE(bit_select(uint512_count, rank, data, 1) == 4*512+2*64);
	REQUIRE(bit_select(uint512_count, rank, data, 2) == 8*512+5*64);
	REQUIRE(bit_select(uint512_count, rank, data, 3) == 13*512+2*64);
}
