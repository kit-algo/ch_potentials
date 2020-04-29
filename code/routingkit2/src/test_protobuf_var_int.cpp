#include "protobuf_var_int.h"

#include "catch.hpp"
#include <limits>

using namespace std;
using namespace RoutingKit2;

namespace {

struct ZigZagTest{
	int64_t i64;
	uint64_t u64;
};

ZigZagTest zigzag_test_cases[] = {
	{0ll, 0ull},
	{-1ll, 1ull},
	{1ll, 2ull},
	{-2ll, 3ull},
	{2147483647ll, 4294967294ull},
	{-2147483648ll, 4294967295ull},
	{-2147483648ll, 4294967295ull},
	{numeric_limits<int64_t>::max(), numeric_limits<uint64_t>::max()-1},
	{numeric_limits<int64_t>::min(), numeric_limits<uint64_t>::max()}
};

TEST_CASE("ZigZagEncodingInt64ToUInt64", "[ZigZagEncoding]") {
	for(auto t:zigzag_test_cases)
		REQUIRE(t.u64 == zigzag_convert_int64_to_uint64(t.i64));
}

TEST_CASE("ZigZagEncodingUInt64ToInt64", "[ZigZagEncoding]") {
	for(auto t:zigzag_test_cases)
		REQUIRE(t.i64 == zigzag_convert_uint64_to_int64(t.u64));
}

TEST_CASE("ZigZagEncodingRoundTrip", "[ZigZagEncoding]") {
	REQUIRE(0 == zigzag_convert_int64_to_uint64(zigzag_convert_uint64_to_int64(0)));
	for(uint64_t x = 3, i=0; i<10000; x *= 7, ++i){
		REQUIRE(x == zigzag_convert_int64_to_uint64(zigzag_convert_uint64_to_int64(x)));
	}
}

TEST_CASE("VarIntEncodingEncodeSize", "[VarIntEncoding]") {
	uint8_t buffer[100];

	#define MY_TEST(x)\
		REQUIRE(compute_var_uint_size(x) == static_cast<unsigned>(encode_var_uint(buffer, x)-buffer));

	for(uint64_t x=0; x<1000; ++x){
		MY_TEST(x);
	}

	for(uint64_t x = 3, i=0; i<10000; x *= 7, ++i){
		MY_TEST(x);
	}

	#undef MY_TEST
}

TEST_CASE("VarIntEncodingEncodeSingleByte", "[VarIntEncoding]") {
	uint8_t buffer[1];

	for(uint64_t x = 0; x<128; ++x){
		uint8_t*end = encode_var_uint(buffer, x);

		REQUIRE(end == buffer+1);
		REQUIRE(buffer[0] == x);
	}
}

TEST_CASE("VarIntEncodingFrontDecodeSingleByte", "[VarIntEncoding]") {
	uint8_t buffer[1];

	for(uint64_t x = 0; x<128; ++x){
		buffer[0] = x;
		const uint8_t* begin = buffer;
		const uint8_t* end = buffer+1;
		REQUIRE(x == decode_var_uint_from_front_and_advance(begin, end));
		REQUIRE(begin == buffer+1);
	}
}

TEST_CASE("VarIntEncodingBackDecodeSingleByte", "[VarIntEncoding]") {
	uint8_t buffer[1];

	for(uint64_t x = 0; x<128; ++x){
		buffer[0] = x;
		const uint8_t* begin = buffer;
		const uint8_t* end = buffer+1;
		REQUIRE(x == decode_var_uint_from_back_and_advance(begin, end));
		REQUIRE(end == buffer);
	}

}


TEST_CASE("VarIntEncodingFrontRoundTrip", "[VarIntEncoding]") {
	uint8_t buffer[100];

	#define MY_TEST(x)\
		const uint8_t* begin = buffer;\
		const uint8_t* end = encode_var_uint(buffer, x);\
		uint64_t y = decode_var_uint_from_front_and_advance(begin, end);\
		REQUIRE(x == y);\
		REQUIRE(begin == end);

	for(uint64_t x=0; x<1000; ++x){
		MY_TEST(x);
	}

	for(uint64_t x = 3, i=0; i<10000; x *= 7, ++i){
		MY_TEST(x);
	}

	#undef MY_TEST
}


TEST_CASE("VarIntEncodingBackRoundTrip", "[VarIntEncoding]") {
	uint8_t buffer[100];

	#define MY_TEST(x)\
		const uint8_t* begin = buffer;\
		const uint8_t* end = encode_var_uint(buffer, x);\
		uint64_t y = decode_var_uint_from_back_and_advance(begin, end);\
		REQUIRE(begin == end);\
		REQUIRE(x == y);

	for(uint64_t x=0; x<1000; ++x){
		MY_TEST(x);
	}

	for(uint64_t x = 3, i=0; i<10000; x *= 7, ++i){
		MY_TEST(x);
	}

	#undef MY_TEST
}

uint64_t decoded [] = {
	6548486ull,
	3274243ull,
	9822730ull,
	4911365ull,
	14734096ull,
	7367048ull,
	3683524ull,
	1841762ull,
	920881ull,
	2762644ull,
	1381322ull,
	690661ull,
	2071984ull,
	1035992ull,
	517996ull,
	258998ull,
	129499ull,
	388498ull,
	194249ull,
	582748ull,
	291374ull,
	145687ull,
	437062ull,
	218531ull,
	655594ull,
	327797ull,
	983392ull,
	491696ull,
	245848ull,
	122924ull,
	61462ull,
	30731ull,
	92194ull,
	46097ull,
	138292ull,
	69146ull,
	34573ull,
	103720ull,
	51860ull,
	25930ull,
	12965ull,
	38896ull,
	19448ull,
	9724ull,
	4862ull,
	2431ull,
	7294ull,
	3647ull,
	10942ull,
	5471ull,
	16414ull,
	8207ull,
	24622ull,
	12311ull,
	36934ull,
	18467ull,
	55402ull,
	27701ull,
	83104ull,
	41552ull,
	20776ull,
	10388ull,
	5194ull,
	2597ull,
	7792ull,
	3896ull,
	1948ull,
	974ull,
	487ull,
	1462ull,
	731ull,
	2194ull,
	1097ull,
	3292ull,
	1646ull,
	823ull,
	2470ull,
	1235ull,
	3706ull,
	1853ull,
	5560ull,
	2780ull,
	1390ull,
	695ull,
	2086ull,
	1043ull,
	3130ull,
	1565ull,
	4696ull,
	2348ull,
	1174ull,
	587ull,
	1762ull,
	881ull,
	2644ull,
	1322ull,
	661ull,
	1984ull,
	992ull,
	496ull,
	248ull,
	124ull,
	62ull,
	31ull,
	94ull,
	47ull,
	142ull,
	71ull,
	214ull,
	107ull,
	322ull,
	161ull,
	484ull,
	242ull,
	121ull,
	364ull,
	182ull,
	91ull,
	274ull,
	137ull,
	412ull,
	206ull,
	103ull,
	310ull,
	155ull,
	466ull,
	233ull,
	700ull,
	350ull,
	175ull,
	526ull,
	263ull,
	790ull,
	395ull,
	1186ull,
	593ull,
	1780ull,
	890ull,
	445ull,
	1336ull,
	668ull,
	334ull,
	167ull,
	502ull,
	251ull,
	754ull,
	377ull,
	1132ull,
	566ull,
	283ull,
	850ull,
	425ull,
	1276ull,
	638ull,
	319ull,
	958ull,
	479ull,
	1438ull,
	719ull,
	2158ull,
	1079ull,
	3238ull,
	1619ull,
	4858ull,
	2429ull,
	7288ull,
	3644ull,
	1822ull,
	911ull,
	2734ull,
	1367ull,
	4102ull,
	2051ull,
	6154ull,
	3077ull,
	9232ull,
	4616ull,
	2308ull,
	1154ull,
	577ull,
	1732ull,
	866ull,
	433ull,
	1300ull,
	650ull,
	325ull,
	976ull,
	488ull,
	244ull,
	122ull,
	61ull,
	184ull,
	92ull,
	46ull,
	23ull,
	70ull,
	35ull,
	106ull,
	53ull,
	160ull
};

uint8_t encoded [] = {
	134u,
	216u,
	143u,
	3u,
	131u,
	236u,
	199u,
	1u,
	138u,
	196u,
	215u,
	4u,
	133u,
	226u,
	171u,
	2u,
	144u,
	166u,
	131u,
	7u,
	136u,
	211u,
	193u,
	3u,
	196u,
	233u,
	224u,
	1u,
	226u,
	180u,
	112u,
	177u,
	154u,
	56u,
	148u,
	207u,
	168u,
	1u,
	202u,
	167u,
	84u,
	229u,
	147u,
	42u,
	176u,
	187u,
	126u,
	216u,
	157u,
	63u,
	236u,
	206u,
	31u,
	182u,
	231u,
	15u,
	219u,
	243u,
	7u,
	146u,
	219u,
	23u,
	201u,
	237u,
	11u,
	220u,
	200u,
	35u,
	174u,
	228u,
	17u,
	151u,
	242u,
	8u,
	198u,
	214u,
	26u,
	163u,
	171u,
	13u,
	234u,
	129u,
	40u,
	245u,
	128u,
	20u,
	224u,
	130u,
	60u,
	176u,
	129u,
	30u,
	216u,
	128u,
	15u,
	172u,
	192u,
	7u,
	150u,
	224u,
	3u,
	139u,
	240u,
	1u,
	162u,
	208u,
	5u,
	145u,
	232u,
	2u,
	180u,
	184u,
	8u,
	154u,
	156u,
	4u,
	141u,
	142u,
	2u,
	168u,
	170u,
	6u,
	148u,
	149u,
	3u,
	202u,
	202u,
	1u,
	165u,
	101u,
	240u,
	175u,
	2u,
	248u,
	151u,
	1u,
	252u,
	75u,
	254u,
	37u,
	255u,
	18u,
	254u,
	56u,
	191u,
	28u,
	190u,
	85u,
	223u,
	42u,
	158u,
	128u,
	1u,
	143u,
	64u,
	174u,
	192u,
	1u,
	151u,
	96u,
	198u,
	160u,
	2u,
	163u,
	144u,
	1u,
	234u,
	176u,
	3u,
	181u,
	216u,
	1u,
	160u,
	137u,
	5u,
	208u,
	196u,
	2u,
	168u,
	162u,
	1u,
	148u,
	81u,
	202u,
	40u,
	165u,
	20u,
	240u,
	60u,
	184u,
	30u,
	156u,
	15u,
	206u,
	7u,
	231u,
	3u,
	182u,
	11u,
	219u,
	5u,
	146u,
	17u,
	201u,
	8u,
	220u,
	25u,
	238u,
	12u,
	183u,
	6u,
	166u,
	19u,
	211u,
	9u,
	250u,
	28u,
	189u,
	14u,
	184u,
	43u,
	220u,
	21u,
	238u,
	10u,
	183u,
	5u,
	166u,
	16u,
	147u,
	8u,
	186u,
	24u,
	157u,
	12u,
	216u,
	36u,
	172u,
	18u,
	150u,
	9u,
	203u,
	4u,
	226u,
	13u,
	241u,
	6u,
	212u,
	20u,
	170u,
	10u,
	149u,
	5u,
	192u,
	15u,
	224u,
	7u,
	240u,
	3u,
	248u,
	1u,
	124u,
	62u,
	31u,
	94u,
	47u,
	142u,
	1u,
	71u,
	214u,
	1u,
	107u,
	194u,
	2u,
	161u,
	1u,
	228u,
	3u,
	242u,
	1u,
	121u,
	236u,
	2u,
	182u,
	1u,
	91u,
	146u,
	2u,
	137u,
	1u,
	156u,
	3u,
	206u,
	1u,
	103u,
	182u,
	2u,
	155u,
	1u,
	210u,
	3u,
	233u,
	1u,
	188u,
	5u,
	222u,
	2u,
	175u,
	1u,
	142u,
	4u,
	135u,
	2u,
	150u,
	6u,
	139u,
	3u,
	162u,
	9u,
	209u,
	4u,
	244u,
	13u,
	250u,
	6u,
	189u,
	3u,
	184u,
	10u,
	156u,
	5u,
	206u,
	2u,
	167u,
	1u,
	246u,
	3u,
	251u,
	1u,
	242u,
	5u,
	249u,
	2u,
	236u,
	8u,
	182u,
	4u,
	155u,
	2u,
	210u,
	6u,
	169u,
	3u,
	252u,
	9u,
	254u,
	4u,
	191u,
	2u,
	190u,
	7u,
	223u,
	3u,
	158u,
	11u,
	207u,
	5u,
	238u,
	16u,
	183u,
	8u,
	166u,
	25u,
	211u,
	12u,
	250u,
	37u,
	253u,
	18u,
	248u,
	56u,
	188u,
	28u,
	158u,
	14u,
	143u,
	7u,
	174u,
	21u,
	215u,
	10u,
	134u,
	32u,
	131u,
	16u,
	138u,
	48u,
	133u,
	24u,
	144u,
	72u,
	136u,
	36u,
	132u,
	18u,
	130u,
	9u,
	193u,
	4u,
	196u,
	13u,
	226u,
	6u,
	177u,
	3u,
	148u,
	10u,
	138u,
	5u,
	197u,
	2u,
	208u,
	7u,
	232u,
	3u,
	244u,
	1u,
	122u,
	61u,
	184u,
	1u,
	92u,
	46u,
	23u,
	70u,
	35u,
	106u,
	53u,
	160u,
	1u
};

TEST_CASE("VarIntEncodingFrontSeqDecode", "[VarIntEncoding]"){
	const uint8_t*begin = encoded;
	const uint8_t*end = encoded + sizeof(encoded);
	for(uint64_t y:decoded){
		REQUIRE(y == decode_var_uint_from_front_and_advance(begin, end));
	}
	REQUIRE(begin == end);
}

TEST_CASE("VarIntEncodingBackSeqDecode", "[VarIntEncoding]"){
	const uint8_t*begin = encoded;
	const uint8_t*end = encoded + sizeof(encoded);

	for(int i=sizeof(decoded)/sizeof(decoded[0])-1; i>=0; --i){
		REQUIRE(decoded[i] == decode_var_uint_from_back_and_advance(begin, end));
	}
	REQUIRE(begin == end);
}

}
