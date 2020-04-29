#ifndef ROUTING_KIT2_OPTIONAL_H
#define ROUTING_KIT2_OPTIONAL_H

#include <utility>

namespace RoutingKit2{

	// This class represents a subset of the C++17 std::optional interface. Once
	// C++17 support can be assumed, the RoutingKit class will be replaced by
	// an alias to std::optional.
	template<class T>
	class Optional{
	public:
		Optional()noexcept:has_value_flag(false){}
		explicit Optional(T value):value(std::move(value)), has_value_flag(true){}

		explicit operator bool()const noexcept{
			return has_value();
		}

		bool has_value()const noexcept{
			return has_value_flag;
		}

		T&operator*()noexcept{
			assert(has_value());
			return value;
		}

		const T&operator*()const noexcept{
			assert(has_value());
			return value;
		}

		T*operator->()noexcept{
			assert(has_value());
			return &value;
		}

		const T*operator->()const noexcept{
			assert(has_value());
			return &value;
		}

		template<class V>
		Optional&operator=(V&&v){
			has_value_flag = true;
			value = std::forward<V>(v);
			return *this;
		}

	private:
		T value;
		bool has_value_flag;
	};


	template<class T>
	struct OptionalWithSentinelTraits;

	// This class behaves as Optional<T> but instead of storing an additional boolean,
	// a value of the type T is used to represent the invalid state. Which state is used
	// is defined using OptionTrait.
	template<class T>
	class OptionalWithSentinel{
	public:
		OptionalWithSentinel()noexcept:value(OptionalWithSentinelTraits<T>::get_invalid()){}
		explicit OptionalWithSentinel(T value):value(std::move(value)){}

		explicit operator bool()const noexcept{
			return has_value();
		}

		bool has_value()const noexcept{
			return !OptionalWithSentinelTraits<T>::is_invalid(value);
		}

		T&operator*()noexcept{
			assert(has_value());
			return value;
		}

		const T&operator*()const noexcept{
			assert(has_value());
			return value;
		}

		T*operator->()noexcept{
			assert(has_value());
			return &value;
		}

		const T*operator->()const noexcept{
			assert(has_value());
			return &value;
		}

		template<class V>
		OptionalWithSentinel&operator=(V&&v){
			value = std::forward<V>(v);
			return *this;
		}

	private:
		T value;
	};
}

#endif
