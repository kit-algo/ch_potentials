#include "data_sink.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>

namespace RoutingKit2 {

FileDataSink::FileDataSink(std::string file){
	fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC, DEFFILEMODE);

	if(fd < 0){
		int err = errno;
		throw std::system_error(err, std::system_category(), "syscall open of file \""+file+"\" for writing failed with reason");
	}
}

void FileDataSink::RefFileDataSink::operator()(const uint8_t*data, size_t len)const{
	while(len != 0){
		ssize_t written = write(fd, data, len);
		if(written < 0){
			int err = errno;
			throw std::system_error(err, std::system_category(), "syscall write failed with reason");
		}
		len -= written;
	}
}

FileDataSink::~FileDataSink(){
	if(fd != -1)
		close(fd);
}

} // namespace RoutingKit2
