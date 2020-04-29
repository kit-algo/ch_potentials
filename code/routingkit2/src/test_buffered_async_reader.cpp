#include "buffered_async_reader.h"
#include "catch.hpp"

#include <iostream>
#include <stdexcept>

using namespace RoutingKit2;
using namespace std;


namespace {

struct test_exception{};

struct WellbehavedMockFile{
	size_t file_length;
	uint8_t file_byte;

	explicit WellbehavedMockFile(size_t file_length):
		file_length(file_length), file_byte(0){}

	size_t operator()(uint8_t*buffer, size_t to_read){
		size_t written = 0;
		while(to_read > 0 && file_length > 0){
			++written;
			*buffer = file_byte;
			++buffer;
			++file_byte;
			--to_read;
			--file_length;
		}
		return written;
	}

	RefDataSource as_ref(){
		return [this](uint8_t*buffer, size_t to_read) -> size_t {
			return (*this)(buffer, to_read);
		};
	}
};

struct HickupMockFile{
	WellbehavedMockFile mock;
	uint8_t hickup;

	explicit HickupMockFile(size_t file_length):
		mock(file_length),  hickup(2){}

	size_t operator()(uint8_t*buffer, size_t to_read){
		hickup *= 7;
		if(to_read > hickup)
			to_read -= hickup;

		return mock(buffer, to_read);
	}

	RefDataSource as_ref(){
		return [this](uint8_t*buffer, size_t to_read) -> size_t {
			return (*this)(buffer, to_read);
		};
	}
};

struct ExceptionMockFile{
	HickupMockFile mock;
	size_t poison_offset;

	explicit ExceptionMockFile(size_t file_length, size_t poison_offset):
		mock(file_length),  poison_offset(poison_offset){}

	size_t operator()(uint8_t*buffer, size_t to_read){
		if(poison_offset < to_read)
			throw test_exception();
		poison_offset -= to_read;

		return mock(buffer, to_read);
	}

	RefDataSource as_ref(){
		return [this](uint8_t*buffer, size_t to_read) -> size_t {
			return (*this)(buffer, to_read);
		};
	}
};

}

TEST_CASE("WellbehavedMockFile", "[BufferedAsyncReader]"){
	const size_t file_length = (1<<21)-103;
	size_t buf_length = 75321;

	WellbehavedMockFile mock(file_length);
	BufferedAsyncReader reader(mock.as_ref(), buf_length);

	size_t read_count = 0;

	uint8_t byte = 0;
	while(buf_length != 0){
		uint8_t*buf = reader.read(buf_length);
		if(buf == nullptr){
			buf_length /= 2;
		} else {
			for(size_t i=0; i<buf_length; ++i){
				REQUIRE(byte == *buf);
				++byte;
				++buf;
				++read_count;
			}
		}
	}
	REQUIRE(read_count == file_length);
}

TEST_CASE("HickupMockFile", "[BufferedAsyncReader]"){
	const size_t file_length = (1<<21)+103;
	size_t buf_length = 75321;

	HickupMockFile mock(file_length);
	BufferedAsyncReader reader(mock.as_ref(), buf_length);

	size_t read_count = 0;

	uint8_t byte = 0;
	while(buf_length != 0){
		uint8_t*buf = reader.read(buf_length);
		if(buf == nullptr){
			buf_length /= 2;
		} else {
			for(size_t i=0; i<buf_length; ++i){
				REQUIRE(byte == *buf);
				++byte;
				++buf;
				++read_count;
			}
		}
	}
	REQUIRE(read_count == file_length);
}

TEST_CASE("Exception", "[BufferedAsyncReader]"){
	ExceptionMockFile mock(1<<20, 1<<19);
	BufferedAsyncReader reader(mock.as_ref(), (1<<17)+5);

	try{
		reader.read_or_throw(1<<17);
		reader.read_or_throw(1<<17);
		reader.read_or_throw(1<<17);
		reader.read_or_throw((1<<17)+1);
		REQUIRE(false);
	}catch(test_exception){
	}
}
