#ifndef ROUTING_KIT2_INVERSE_FUNC_H
#define ROUTING_KIT2_INVERSE_FUNC_H

#include "sort.h"
#include "min_max.h"
#include "span.h"

#include <assert.h>
#include <algorithm>
#include <vector>

namespace RoutingKit2{

//! invert_func is used to quickly lookup values in a sorted array.
//!
//! We are given a sorted array S of uint32_t values between 0 and element_count-1.
//! The task is to compute an array R with size element_count+1 such that the following
//! property holds:
//!
//! forall e with 0 <= e < element_count{
//!   forall i with R[e] <= i < R[e+1]{
//!     assert(S[i] == e);
//!   }
//! }
//!
//! invert_func computes this R.

inline
void invert_func_and_copy_into(Span<const uint32_t>func, Span<uint32_t>inv_func){
	const uint32_t image_count = inv_func.size()-1;
	assert(is_sorted_using_less(func));
	assert(!inv_func.empty());
	assert(func.empty() || max_element_of(func) < image_count);

	uint32_t dom = 0;
	for(uint32_t img=0; img<image_count; ++img){
		while(dom < func.size() && func[dom] < img)
			++dom;
		inv_func[img] = dom;
	}

	inv_func[image_count] = func.size();
}

inline
std::vector<uint32_t>invert_func(Span<const uint32_t>func, uint32_t element_count){
	std::vector<uint32_t>inv_func(element_count+1);
	invert_func_and_copy_into(func, inv_func);
	return inv_func;
}

inline
void invert_inverse_func_and_copy_into(Span<const uint32_t>inv_func, Span<uint32_t>func){
	assert(!inv_func.empty());
	assert(inv_func.back() == func.size());
	for(uint32_t i=0; i<inv_func.size()-1; ++i)
		for(uint32_t j=inv_func[i]; j<inv_func[i+1]; ++j)
			func[j] = i;
}

inline
std::vector<uint32_t> invert_inverse_func(Span<const uint32_t>inv_func){
	assert(!inv_func.empty());
	std::vector<unsigned>func(inv_func.back());
	invert_inverse_func_and_copy_into(inv_func, func);
	return func;
}

inline
bool is_inverse_func(Span<const uint32_t>inv_func, uint32_t preimage_count, uint32_t image_count){
	if(inv_func.empty())
		return false;
	if(inv_func.size()+1 != image_count)
		return false;
	if(inv_func.front() != 0)
		return false;
	if(inv_func.back() != preimage_count)
		return false;
	if(!is_sorted_using_less(inv_func))
		return false;
	return true;
}

inline
void assert_is_inverse_func(Span<const uint32_t>inv_func, uint32_t preimage_count, uint32_t image_count){
	assert(!inv_func.empty());
	assert(inv_func.size() == image_count+1);
	assert(inv_func.front() == 0);
	assert(inv_func.back() == preimage_count);
	assert(is_sorted_using_less(inv_func));
}

} // RoutingKit2

#endif

