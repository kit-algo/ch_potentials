#ifndef ROUTING_KIT2_PERMUTATION_H
#define ROUTING_KIT2_PERMUTATION_H

#include "span.h"

#include <vector>
#include <assert.h>
#include <algorithm>
#include <utility>

namespace RoutingKit2{

//
//  Applying a permutation p to a vector v consists of computing the vector {v[p[0]], v[p[1]], v[p[2]], ... , v[p[n]]}
//
//  Applying a permutation p to the elements of a vector v consists of computing the vector {p[v[0]], p[v[1]], p[v[2]], ... , p[v[n]]}
//

inline
bool is_permutation(Span<const unsigned>p){
	std::vector<bool>found(p.size(), false);
	for(unsigned x:p){
		if(x >= p.size())
			return false;
		if(found[x])
			return false;
		found[x] = true;
	}
	return true;
}

template<class In, class Out>
void apply_permutation_and_copy_into(Span<const unsigned>p, const In&in, Out&out){
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == in.size() && "permutation and input must have the same size");
	assert(p.size() == out.size() && "permutation and output must have the same size");

	for(unsigned i=0; i<p.size(); ++i)
		out[i] = in[p[i]];
}

template<class In, class Out>
void apply_permutation_and_move_into(Span<const unsigned>p, In&&in, Out&out){
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == in.size() && "permutation and input span must have the same size");
	assert(p.size() == out.size() && "permutation and output span must have the same size");

	for(unsigned i=0; i<p.size(); ++i)
		out[i] = std::move(in[p[i]]);
}

template<class In, class Out>
void apply_inverse_permutation_and_copy_into(Span<const unsigned>p, const In&in, Out&out){
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == in.size() && "permutation and input span must have the same size");
	assert(p.size() == out.size() && "permutation and output span must have the same size");

	for(unsigned i=0; i<p.size(); ++i)
		out[p[i]] = in[i];
}

template<class In, class Out>
void apply_inverse_permutation_and_move_into(Span<const unsigned>p, const In&in, Out&out){
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == in.size() && "permutation and input span must have the same size");
	assert(p.size() == out.size() && "permutation and output span must have the same size");

	for(unsigned i=0; i<p.size(); ++i)
		out[p[i]] = std::move(in[i]);
}

template<class C>
auto apply_permutation(Span<const unsigned>p, const C&v) -> std::vector<typename C::value_type>{
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == v.size() && "permutation and span must have the same size");

	std::vector<typename C::value_type>r(v.size());
	apply_permutation_and_copy_into(p, v, r);
	return r;
}


template<class C>
auto apply_inverse_permutation(Span<const unsigned>p, const C&v)-> std::vector<typename C::value_type>{
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == v.size() && "permutation and span must have the same size");

	std::vector<typename C::value_type>r(v.size());
	apply_inverse_permutation_and_copy_into(p, v, r);
	return r;
}

inline
void inplace_apply_permutation_to_elements_of(Span<const unsigned>p, Span<unsigned>v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(std::all_of(v.begin(), v.end(), [&](unsigned x){return x < p.size();}) && "v has an out of bounds element");

	for(unsigned i=0; i<v.size(); ++i)
		v[i] = p[v[i]];
}

inline
std::vector<unsigned> apply_permutation_to_elements_of(Span<const unsigned>p, Span<const unsigned>v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(std::all_of(v.begin(), v.end(), [&](unsigned x){return x < p.size();}) && "v has an out of bounds element");

	std::vector<unsigned> r(v.begin(), v.end());
	inplace_apply_permutation_to_elements_of(p, r);
	return r;
}

inline
void invert_permutation_and_copy_into(Span<const unsigned>in, Span<unsigned>out){
	assert(is_permutation(in) && "in must be a permutation");
	assert(in.size() == out.size() && "in and out must have the same size");

	for(unsigned i=0; i<in.size(); ++i)
		out[in[i]] = i;
}

inline
std::vector<unsigned> invert_permutation(Span<const unsigned>p){
	assert(is_permutation(p) && "p must be a permutation");

	std::vector<unsigned> inv_p(p.size());
	invert_permutation_and_copy_into(p, inv_p);
	return inv_p;
}

inline
void copy_identity_permutation_into(Span<unsigned>p){
	for(unsigned i=0; i<p.size(); ++i)
		p[i] = i;
}


inline
std::vector<unsigned> identity_permutation(unsigned n){
	std::vector<unsigned> p(n);
	copy_identity_permutation_into(p);
	return p;
}

template<class RandomGenerator>
void copy_random_permutation_into(RandomGenerator&&gen, Span<unsigned>p){
	copy_identity_permutation_into(p);
	// std::shuffle is allowed to behave differently from compiler to compiler.
	// We do not want that.
	std::shuffle(p.begin(), p.end(), std::forward<RandomGenerator>(gen));
}

template<class RandomGenerator>
std::vector<unsigned> random_permutation(unsigned n, RandomGenerator&&gen){
	std::vector<unsigned>r(n);
	copy_random_permutation_into(std::forward<RandomGenerator>(gen), r);
	return r;
}

} // RoutingKit2

#endif

