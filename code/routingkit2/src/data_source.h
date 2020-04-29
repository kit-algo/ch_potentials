#ifndef ROUTING_KIT2_DATA_SOURCE_H
#define ROUTING_KIT2_DATA_SOURCE_H

#include <string>
#include <functional>
#include <vector>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>

namespace RoutingKit2{

using RefDataSource = std::function<size_t(uint8_t*data, size_t len)>;

// An owning DataSink has the following syntax:

#if 0
class MyDataSource{
	// Reads bytes and fills the given buffer.
	// The buffer is not necessarily fully filled.
	// The function returns the number of bytes read.
	// The function return 0, if and only if, the end of file was reached.
	size_t operator()(uint8_t*data, size_t len);

	// Returns a function object that behaves just as operator()
	RefDataSource as_ref();
};
#endif


class FileDataSource{
public:
	FileDataSource():fd(-1){}
	explicit FileDataSource(std::string filename);

	size_t operator()(uint8_t*buf, size_t len);

	struct RefFileDataSource{
		size_t operator()(uint8_t*buf, size_t len)const{
			return (*src)(buf, len);
		}
		FileDataSource*src;
	};

	RefFileDataSource as_ref(){
		return {this};
	}

	void rewind();

	~FileDataSource();

private:
	int fd;
	std::string filename;
};

class UInt8DataSource{
public:
	UInt8DataSource(){}
	UInt8DataSource(const uint8_t*begin, const uint8_t*end):
		src_buf(begin), src_len(end-begin){}
	UInt8DataSource(const uint8_t*buf, size_t len):
		src_buf(buf), src_len(len){}
	explicit UInt8DataSource(const std::vector<uint8_t>&src):
		src_buf(&src[0]), src_len(src.size()){}

	size_t operator()(uint8_t*buf, size_t len){
		if(len > src_len)
			len = src_len;
		memcpy(buf, src_buf, len);
		src_len -= len;
		src_buf += len;
		return len;
	}

	struct RefUInt8DataSource{
		size_t operator()(uint8_t*buf, size_t len)const{
			return (*this)(buf, len);
		}
		UInt8DataSource*parent;
	};

	RefUInt8DataSource as_ref(){
		return {this};
	}

	~UInt8DataSource();

private:
	const uint8_t*src_buf;
	size_t src_len;
};

template<class RefDataSource>
void read_full_buffer_from_data_source(RefDataSource data_source, uint8_t*buf, size_t len){
	while(len != 0){
		size_t read = data_source(buf, len);
		if(read == 0)
			throw std::runtime_error("Cannot fill buffer as end of data source is reached");
		buf += read;
		len -= read;
	}
}

} // namespace RoutingKit2

#endif
