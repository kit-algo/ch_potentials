#ifndef ROUTING_KIT2_PREFIX_SUM_H
#define ROUTING_KIT2_PREFIX_SUM_H

#include "span.h"
#include <stdint.h>

namespace RoutingKit2{
	namespace detail {
		template<class T>
		void compute_prefix_sum(Span<T>v){
			T sum = 0;
			for(T*i = v.begin(); i!=v.end(); ++i){
				T tmp = *i;
				*i = sum;
				sum += tmp;
			}
		}
	}

	inline void compute_prefix_sum(Span<uint8_t>v){ detail::compute_prefix_sum(v); }
	inline void compute_prefix_sum(Span<uint16_t>v){ detail::compute_prefix_sum(v); }
	inline void compute_prefix_sum(Span<uint32_t>v){ detail::compute_prefix_sum(v); }
	inline void compute_prefix_sum(Span<uint64_t>v){ detail::compute_prefix_sum(v); }
	inline void compute_prefix_sum(Span<int8_t>v){ detail::compute_prefix_sum(v); }
	inline void compute_prefix_sum(Span<int16_t>v){ detail::compute_prefix_sum(v); }
	inline void compute_prefix_sum(Span<int32_t>v){ detail::compute_prefix_sum(v); }
	inline void compute_prefix_sum(Span<int64_t>v){ detail::compute_prefix_sum(v); }
}

#endif
