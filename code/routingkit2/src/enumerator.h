#ifndef ROUTING_KIT2_ENUMERATOR_H
#define ROUTING_KIT2_ENUMERATOR_H

#include "span.h"
#include "optional.h"

namespace RoutingKit2{

	//! An enumerator is a class that fulfills the following concept:
	//!
	//! class Enumerator{
	//! public:
	//!     Enumerator()noexcept;
	//!     Optional<T>next();
	//! };
	//!
	//! next may also return a OptionalWithSentinel.
	//!
	//! An enumerates a (possibly endless) sequence of elements of type T.
	//! Once the end was reached, next returns an empty optional.
	//!
	//! A typical loop looks as follows:
	//!
	//! Enumerator my_enumerator = ...;
	//! while(auto next = my_enumerator.next()){
	//!     do stuff with *next
	//! }

	template<class T>
	class SpanEnumerator{
	public:
		SpanEnumerator()noexcept:begin(nullptr), end(nullptr){}

		explicit SpanEnumerator(const T*begin, const T*end):
			begin(begin), end(end){}

		explicit SpanEnumerator(Span<const T>arr):
			begin(&*arr.begin()), end(&*arr.end()){}

		Optional<T> next(){
			if(begin == end)
				return Optional<T>();
			else
				return Optional<T>(*begin++);
		}

	private:
		const T*begin, *end;
	};
}

#endif
