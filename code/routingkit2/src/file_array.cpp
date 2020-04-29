#include "file_array.h"

#include <system_error>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

namespace RoutingKit2 {

void dump_into_file(std::string file, const char*data, std::size_t len) {
	int fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC, DEFFILEMODE);

	if(fd < 0){
		int err = errno;
		throw std::system_error(err, std::system_category(), "write open of file \""+file+"\" failed with reason");
	}

	do{
		ssize_t written = write(fd, data, len);
		if(written < 0){
			int err = errno;
			close(fd);
			throw std::system_error(err, std::system_category(), "write to file \""+file+"\" failed with reason");
		}
		len -= written;
	}while(len != 0);
}

namespace detail{
	FileView map_readonly_array_into_mem(std::string file, std::size_t type_size) {

		int fd = open(file.c_str(), O_RDONLY);
		if(fd < 0){
			int err = errno;
			throw std::system_error(err, std::system_category(), "read open of file \""+file+"\" failed with reason");
		}

		struct stat file_stat;
		if(fstat(fd, &file_stat)<0){
			int err = errno;
			close(fd);
			throw std::system_error(err, std::system_category(), "fstat of file \""+file+"\" failed with reason");
		}

		size_t file_size = file_stat.st_size;
		size_t mod = file_size % type_size;
		size_t div = file_size / type_size;

		if(mod != 0)
			throw std::runtime_error("the size of file \""+file+"\" must be a multiple of "+std::to_string(type_size));

		void*mem = nullptr;
		if(file_size != 0){
			mem = mmap(0, file_size, PROT_READ, MAP_SHARED|MAP_POPULATE, fd, 0);
			if(mem == MAP_FAILED){
				int err = errno;
				close(fd);
				throw std::system_error(err, std::system_category(), "mmap on file \""+file+"\" failed with reason");
			}
		}

		FileView ret;
		ret.fd = fd;
		ret.mem = (const char*)mem;
		ret.len = div;
		fd = -1;
		return ret;
	}

	void unmap_readonly_array_from_mem(FileView view, std::size_t type_size){
		if(view.len != 0)
			munmap((void*)view.mem, type_size*view.len);
		close(view.fd);
	}
}

} // namespace RoutingKit2
