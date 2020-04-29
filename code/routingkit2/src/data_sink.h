#ifndef ROUTING_KIT2_DATA_SINK_H
#define ROUTING_KIT2_DATA_SINK_H

#include <utility>
#include <string>
#include <functional>
#include <vector>
#include <stdlib.h>

namespace RoutingKit2{

using RefDataSink = std::function<void(const uint8_t*data, size_t len)>;

// An owning DataSink has the following syntax:

#if 0
class MyDataSink{
	// Writes the whole buffer to the file or throws if this is not possible.
	// If an exception is thrown, then some of the data might already have been written.
	void operator()(const uint8_t*data, size_t len);

	// Returns a function object that behaves just as operator()
	RefDataSink as_ref();
};
#endif

class FileDataSink{
public:
	FileDataSink():fd(-1){}
	explicit FileDataSink(std::string file);

	FileDataSink(const FileDataSink&)=delete;
	FileDataSink&operator=(const FileDataSink&)=delete;

	// Writes the whole buffer to the file
	void operator()(const uint8_t*data, size_t len){
		as_ref()(data, len);
	}

	struct RefFileDataSink{
		void operator()(const uint8_t*data, size_t len)const;
		int fd;
	};

	RefFileDataSink as_ref(){
		return RefFileDataSink{fd};
	}

	~FileDataSink();

private:
	int fd;
};

class VecUInt8DataSink{
public:
	VecUInt8DataSink(){}
	explicit VecUInt8DataSink(std::vector<uint8_t>&vec):
		vec(&vec){}

	VecUInt8DataSink(const FileDataSink&)=delete;
	VecUInt8DataSink&operator=(const FileDataSink&)=delete;

	void operator()(const uint8_t*data, size_t len){
		as_ref()(data, len);
	}

	struct RefVecUInt8DataSink{
		void operator()(const uint8_t*data, size_t len)const{
			vec->insert(vec->end(), data, data + len);
		}

		std::vector<std::uint8_t>*vec;
	};


	RefVecUInt8DataSink as_ref(){
		return {vec};
	}

private:
	std::vector<std::uint8_t>*vec;
};

}

#endif
