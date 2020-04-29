#ifndef ROUTING_KIT2_FILE_ARRAY_H
#define ROUTING_KIT2_FILE_ARRAY_H

#include <utility>
#include <string>
#include <functional>
#include <vector>
#include <stdlib.h>

namespace RoutingKit2{

namespace detail{
	struct FileView{
		int fd;
		const char*mem;
		std::size_t len;
	};

	FileView map_readonly_array_into_mem(std::string file, std::size_t type_size);
	void unmap_readonly_array_from_mem(FileView, std::size_t type_size);
}

template<class T>
class FileArray{
public:
	FileArray() noexcept:
		view{-1}{}

	explicit FileArray(std::string file):
		view(detail::map_readonly_array_into_mem(std::move(file), sizeof(T))){
	}

	FileArray(FileArray&&o) noexcept:
		view(o.view){
		o.view.fd = -1;
	}

	FileArray&operator=(FileArray&&o){
		view = o.view;
		o.view.fd = -1;
		return *this;
	}

	FileArray(const FileArray&)=delete;
	FileArray&operator=(const FileArray&)=delete;

	~FileArray() noexcept { close(); }

	void open(std::string file){
		detail::FileView old_view = view;
		view = detail::map_readonly_array_into_mem(std::move(file), sizeof(T));
		if(old_view.fd != -1)
			detail::unmap_readonly_array_from_mem(old_view, sizeof(T));
	}

	void close() noexcept{
		if(view.fd != -1)
			detail::unmap_readonly_array_from_mem(view, sizeof(T));
	}

	std::size_t size() const noexcept { return view.len; }
	const T* data() const noexcept { return (const T*)view.mem; }

	bool empty() const noexcept {return size() == 0; }
	const T&operator[](std::size_t i) const noexcept { return *(data() + i); }

	const char*begin() const noexcept { return data(); }
	const char*end() const noexcept { return data()+size(); }
	const char*cbegin() const noexcept { return begin(); }
	const char*cend() const noexcept { return end(); }

private:
	detail::FileView view;
};

} // namespace RoutingKit2

#endif
