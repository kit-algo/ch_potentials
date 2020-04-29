#include "buffered_async_reader.h"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string.h>
#include <assert.h>
#include <exception>
#include <string>

namespace RoutingKit2{

// The struct allocates a buffer that is 6 block sizes long.
// The first and the last block contain exactly the same data.
// This is necessary to allow the `read` function to return a
// continous array of bytes even if the data wraps around the buffer end.
// Effectively, there are thus 5 blocks available.
// Of these blocks the background thread fills at most three.
// These three blocks are marked as allocated whereas the other two blocks are unallocated.
// The `read` function removes up to one block from the allocated region and returns a pointer
// into the region it unallocated.
// The background thread notices that less than three blocks are allocated and starts reading bytes.
// However, it will read at most one block, as only one block was removed by the `read` function.
// The bytes returned by the `read` function lie in the other of the two unallocated blocks.
// The buffer returned by `read` will therefore not be overwritten until the next call to `read`.

struct BufferedAsyncReader::Impl{
	uint32_t block_size;
	uint8_t*buffer;

	uint32_t alloc_begin, alloc_end;

	bool was_end_of_file_reached;
	bool was_termination_requested;

	std::thread worker;

	std::exception_ptr read_exception;

	std::mutex lock;
	std::condition_variable main_thread_has_done_something;
	std::condition_variable worker_thread_has_done_something;

	Impl(){}

	~Impl(){
		{
			std::unique_lock<std::mutex>guard(lock);
			was_termination_requested = true;
		}
		main_thread_has_done_something.notify_one();
		worker.join();

		delete[]buffer;
	}

	uint32_t alloc_size() const {
		if(alloc_begin <= alloc_end){
			return alloc_end - alloc_begin;
		}else{
			return alloc_end + 5*block_size - alloc_begin;
		}
	}

	uint32_t unalloc_size() const {
		return 5*block_size - alloc_size();
	}

	Impl(const Impl&)=delete;
	Impl&operator=(const Impl&)=delete;
	Impl(Impl&&)=delete;
	Impl&operator=(Impl&&)=delete;

};

BufferedAsyncReader::BufferedAsyncReader(){}

BufferedAsyncReader::BufferedAsyncReader(BufferedAsyncReader&&o)noexcept{
	impl = std::move(o.impl);
}

BufferedAsyncReader&BufferedAsyncReader::operator=(BufferedAsyncReader&&o)noexcept{
	impl = std::move(o.impl);
	return *this;
}

BufferedAsyncReader::~BufferedAsyncReader(){
}

BufferedAsyncReader::BufferedAsyncReader(RefDataSource byte_source, unsigned block_size):
	impl(std::unique_ptr<Impl>(new Impl))
{
	assert(byte_source && "byte_source must not be 0");

	// reading less than 4 KiB is silly for performance reasons
	if(block_size < (4<<10))
		block_size = 4<<10;

	impl->block_size = block_size;
	impl->buffer = new uint8_t[6*block_size];

	std::unique_lock<std::mutex>guard(impl->lock);
	impl->was_termination_requested = false;
	impl->was_end_of_file_reached = false;

	impl->alloc_begin = 0;
	impl->alloc_end = 0;

	Impl*ptr = impl.get();

	impl->worker = std::thread(
		[this, ptr, byte_source, block_size]{
			std::unique_lock<std::mutex>guard(ptr->lock);
			try{
				for(;;){
					ptr->main_thread_has_done_something.wait(
						guard,
						[&]{
							return ptr->unalloc_size() >= 2*block_size || ptr->was_termination_requested;
						}
					);

					if(ptr->was_termination_requested){
						return;
					}

					guard.unlock();
					size_t bytes_read = byte_source(ptr->buffer + ptr->alloc_end, block_size);
					guard.lock();

					assert(bytes_read <= block_size);

					if(ptr->was_termination_requested){
						return;
					}

					if(bytes_read == 0){
						ptr->was_end_of_file_reached = true;
						ptr->worker_thread_has_done_something.notify_one();
						return;
					}

					uint32_t
						read_begin = ptr->alloc_end,
						read_end = read_begin + bytes_read;

					if(read_begin < block_size){
						// data must be copied from the first block to the sixth
						memcpy(
							ptr->buffer+5*block_size + read_begin,
							ptr->buffer + read_begin,
							std::min(read_end - read_begin, block_size - read_begin)
						);
					} else if(5*block_size < read_end) {
						// data must be copied from the sixth block to the first
						memcpy(ptr->buffer, ptr->buffer+5*block_size, read_end-5*block_size);
					}

					if(5*block_size <= read_end)
						read_end -= 5*block_size;

					ptr->alloc_end = read_end;

					ptr->worker_thread_has_done_something.notify_one();
				}
			}catch(...){
				ptr->read_exception = std::current_exception();
				ptr->worker_thread_has_done_something.notify_one();
			}
		}
	);
}

uint8_t* BufferedAsyncReader::read(unsigned size) {
	if(size > impl->block_size)
		throw std::logic_error("Requested to read " +std::to_string(size)+" bytes, which is more than the maximum read size of "+std::to_string(impl->block_size));

	std::unique_lock<std::mutex>guard(impl->lock);

	impl->worker_thread_has_done_something.wait(
		guard,
		[&]{
			return impl->was_end_of_file_reached || impl->alloc_size() >= size || impl->read_exception;
		}
	);

	if(impl->read_exception){
		impl->was_end_of_file_reached = true;
		std::rethrow_exception(impl->read_exception);
	}

	if(impl->alloc_size() >= size){
		unsigned
			alloc_begin = impl->alloc_begin,
			block_size = impl->block_size;

		uint8_t*ret = impl->buffer + alloc_begin;

		alloc_begin += size;
		if(alloc_begin >= 5*block_size)
			alloc_begin -= 5*block_size;

		impl->alloc_begin = alloc_begin;
		impl->main_thread_has_done_something.notify_one();

		return ret;
	} else {
		assert(impl->was_end_of_file_reached);
		return nullptr;
	}
}

uint8_t* BufferedAsyncReader::read_or_throw(unsigned size){
	uint8_t*x = read(size);
	if(x == 0)
		throw std::runtime_error("Wanted to read "+std::to_string(size)+" bytes but only "+std::to_string(impl->alloc_size())+" are available in the data source.");
	return x;
}

} // namespace RoutingKit2


