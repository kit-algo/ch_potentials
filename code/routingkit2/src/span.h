#ifndef ROUTING_KIT2_SPAN_H
#define ROUTING_KIT2_SPAN_H

#include <stdlib.h>
#include <assert.h>

#include <type_traits>

namespace RoutingKit2{

//! Once std::span is finalized, RoutingKit's Span will likely be replaced by an alias for std::span.
template<class T>
struct Span{

	using value_type = T;

	Span()noexcept{}

	template<class C,
		class = typename std::enable_if<
			std::is_same<T, typename C::value_type>::value ||
			std::is_same<T, typename std::add_const<typename C::value_type>::type>::value
		>::type
	>
	Span(const C&c)noexcept:
		begin_(&*c.begin()), end_(&*c.end()){}

	template<class C,
		class = typename std::enable_if<
			std::is_same<T, typename C::value_type>::value ||
			std::is_same<T, typename std::add_const<typename C::value_type>::type>::value
		>::type
	>
	Span(C&c)noexcept:
		begin_(&*c.begin()), end_(&*c.end()){}

	Span(T*begin_, T*end_)noexcept:
		begin_(begin_), end_(end_){}

	T*begin_, *end_;

	T*begin()const noexcept{
		return begin_;
	}

	T*end()const noexcept{
		return end_;
	}

	T&operator[](size_t i) const noexcept{
		assert(i < size());
		return begin()[i];
	}

	T&front()const noexcept{
		return begin_[0];
	}

	T&back()const noexcept{
		return end_[-1];
	}

	size_t size()const noexcept{
		return end_ - begin_;
	}

	bool empty()const noexcept{
		return begin_ == end_;
	}
};

} // namespace RoutingKit2

#endif
