#ifndef BUFFERED_ASYNC_READER_H
#define BUFFERED_ASYNC_READER_H

#include "data_source.h"

#include <functional>
#include <memory>
#include <stdint.h>

namespace RoutingKit2{

//! A class that reads bytes from a data source in a background thread and fills an internal buffer.
//! Using the read or read_or_throw functions it is possible to get access to the bytes in the buffer.
class BufferedAsyncReader{
public:
	BufferedAsyncReader();

	//! Creates a reader by referencing a data source. The data source object must outlive
	//! the reader. The second parameter is the maximum number of bytes that can be read
	//! in one call to `read` or `read_or_throw`.
	//!
	//! The second parameter is also the number of bytes requested from the byte source.
	BufferedAsyncReader(RefDataSource byte_source, uint32_t max_read_size);

	BufferedAsyncReader(const BufferedAsyncReader&)=delete;
	BufferedAsyncReader&operator=(const BufferedAsyncReader&)=delete;

	BufferedAsyncReader(BufferedAsyncReader&&)noexcept;
	BufferedAsyncReader&operator=(BufferedAsyncReader&&)noexcept;

	~BufferedAsyncReader();

	//! Blocks until the buffer contains at least `size` bytes. As soon as at least `size` were read
	//! the function returns with a pointer into buffer pointing at the `size` bytes.
	//!
	//! The bytes in the buffer remain valid until read or read_or_throw are called.
	//! The buffer is also invalidated when the `BufferedAsyncReader` object is destroyed.
	//!
	//! If the end of the data source is reached before `size` bytes were read,
	//! the function returns a nullptr without removing any bytes from the buffer.
	//!
	//! `size` must be less that the `max_read_size` parameter passed to the constructor.
	//!
	//! If the call to the data source threw an exception,
	//! then the next call to `read` or `read_or_throw` will rethrow the exception.
	//! Afterwards, the reader object is in a bad state and can only be destroyed.
	//!
	//! The caller of `read` can modify the bytes pointed to by the returned pointer.
	uint8_t* read(uint32_t size);

	//! Just as `read` but throws an exception if not enough bytes are in the buffer.
	uint8_t* read_or_throw(uint32_t size);

private:
	struct Impl;
	std::unique_ptr<Impl>impl;
};

} // namespace RoutingKit2

#endif
