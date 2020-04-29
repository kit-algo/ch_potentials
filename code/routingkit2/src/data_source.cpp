#include "data_source.h"

#include <system_error>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

namespace RoutingKit2 {

FileDataSource::FileDataSource(std::string filename):filename(std::move(filename)){
	fd = open(this->filename.c_str(), O_RDONLY);

	if(fd < 0){
		int err = errno;
		throw std::system_error(err, std::system_category(), "syscall open of file \""+this->filename+"\" for reading failed with reason");
	}
}

size_t FileDataSource::operator()(uint8_t*data, size_t len){
	ssize_t n = read(fd, data, len);
	if(n < 0){
		int err = errno;
		throw std::system_error(err, std::system_category(), "syscall read on file \""+this->filename+"\" failed with reason");
	}
	return n;
}

FileDataSource::~FileDataSource(){
	if(fd != -1)
		close(fd);
}

void FileDataSource::rewind(){
	if(lseek(fd, 0, SEEK_SET) == -1){
		int err = errno;
		throw std::system_error(err, std::system_category(), "syscall lseek(fd, 0, SEEK_SET) on file \""+this->filename+"\" failed with reason");
	}
}

} // namespace RoutingKit2
