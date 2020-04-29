#ifndef ROUTING_KIT2_SORT_H
#define ROUTING_KIT2_SORT_H

#include "permutation.h"
#include "span.h"

#include <vector>
#include <assert.h>
#include <algorithm>
#include <functional>

namespace RoutingKit2{

//
// A sort permutation p of an array v is a permutation such that
//
// v[p[0]] <= v[p[1]] <= v[p[2]] <= ... .
//
// Applying a sort permutation p of v to v yields a sorted array.
//
// The functions in this header follow the following naming sheme:
//
//   1) compute_[inverse_][stable_]sort_permutation_using_[(key|less|comparator][_and_copy_into](vector, ...order...[, permutation])
//   2) is_sorted_using_[key|less|comparator](vector, ...order...)
//
// The functionality is explained as following:
//
//   * The functions of category 1 compute sort permutations but do not actually
//     sort the array. The sorting is done by applying the resulting permutation
//     to one or more arrays.
//   * The functions of category 2 do not sort anything but check whether their
//     argument array is sorted.
//   * "inverse" indicates that not a sort permutation but its inverse is
//     computed.
//   * "stable" indicates that the relative order of values that compare equally
//     is not changed.
//   * "key" indicates that elements are sorted using bucket sort. All functions
//     take a (key_count, get_key) pair of parameters to specify the order.
//     get_key is a function  object that must map every element onto a value in
//     [0,key_count).  get_key is assumed to have a quick execution and is
//     called several times per sort operation. However, get_key is never
//     copied.
//   * "comparator" indicates that an STL like comparator is used and needs to
//     be passed to every function.
//   * "less" indicates that < is used for comparision. No order paramters need
//     to be passed to the functions.
//   * The compute_* functions without "_and_copy_into" return the permutation
//     as vector<unsigned>. The _and_copy_into functions take a Span<unsigned> as
//     final parameter and write their output into this parameter. The
//     "_and_copy_into" have no return value.
//

template<class V, class C>
void compute_stable_sort_permutation_using_comparator_and_copy_into(const V&v, const C&is_less, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	copy_identity_permutation_into(p);
	std::stable_sort(
		p.begin(), p.end(),
		[&](unsigned l, unsigned r){
			return is_less(v[l], v[r]);
		}
	);
}

template<class V, class C>
std::vector<unsigned> compute_stable_sort_permutation_using_comparator(const V&v, const C&is_less){
	std::vector<unsigned>p(v.size());
	compute_stable_sort_permutation_using_comparator_and_copy_into(v, is_less, p);
	return p;
}

template<class V, class C>
void compute_sort_permutation_using_comparator_and_copy_into(const V&v, const C&is_less, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	copy_identity_permutation_into(p);
	std::sort(
		p.begin(), p.end(),
		[&](unsigned l, unsigned r){
			return is_less(v[l], v[r]);
		}
	);
}

template<class V, class C>
std::vector<unsigned> compute_sort_permutation_using_comparator(const V&v, const C&is_less){
	std::vector<unsigned>p(v.size());
	compute_sort_permutation_using_comparator_and_copy_into(v, is_less, p);
	return p;
}

template<class V, class C>
void compute_inverse_sort_permutation_using_comparator_and_copy_into(const V&v, const C&is_less, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	std::vector<unsigned>tmp(v.size());
	compute_sort_permutation_using_comparator_and_copy_into(v, is_less, tmp);
	invert_permutation_and_copy_into(tmp, p);
}

template<class V, class C>
std::vector<unsigned> compute_inverse_sort_permutation_using_comparator(const V&v, const C&is_less){
	std::vector<unsigned>r(v.size());
	compute_inverse_sort_permutation_using_comparator_and_copy_into(v, is_less, r);
	return r;
}

template<class V, class C>
void compute_inverse_stable_sort_permutation_using_comparator_and_copy_into(const V&v, const C&is_less, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	std::vector<unsigned>tmp(v.size());
	compute_stable_sort_permutation_using_comparator_and_copy_into(v, is_less, tmp);
	invert_permutation_and_copy_into(tmp, p);
}

template<class V, class C>
std::vector<unsigned> compute_inverse_stable_sort_permutation_using_comparator(const V&v, const C&is_less){
	std::vector<unsigned>r(v.size());
	compute_inverse_stable_sort_permutation_using_comparator_and_copy_into(v, is_less, r);
	return r;
}

template<class V, class C>
bool is_sorted_using_comparator(const V&v, const C&is_less){
	for(unsigned i=1; i<v.size(); ++i)
		if(is_less(v[i], v[i-1]))
			return false;
	return true;
}

namespace detail{
	const unsigned bucket_sort_min_key_to_element_ratio = 16;

	template<class V, class K>
	std::vector<unsigned>compute_key_pos(const V&v, unsigned key_count, const K&get_key){
		std::vector<unsigned>key_pos(key_count, 0);
		for(unsigned i=0; i<v.size(); ++i){
			unsigned k = get_key(v[i]);
			assert(k <= key_count && "key is too large");
			++key_pos[k];
		}
		unsigned sum = 0;
		for(unsigned i=0; i<key_count; ++i){
			unsigned tmp = sum + key_pos[i];
			key_pos[i] = sum;
			sum = tmp;
		}
		return key_pos; // NVRO
	}

	template<class K>
	struct CompareByKey{
		explicit CompareByKey(unsigned n, const K&k):get_key(k){
			#ifndef NDEBUG
			key_count = n;
			#else
			(void)n;
			#endif
		}

		template<class T>
		bool operator()(const T&l, const T&r)const{
			unsigned l_key = get_key(l);
			assert(l_key < key_count);
			unsigned r_key = get_key(r);
			assert(r_key < key_count);
			return l_key < r_key;
		}

		#ifndef NDEBUG
		unsigned key_count;
		#endif
		const K&get_key;
	};

	template<class K>
	CompareByKey<K> make_compare_by_key(unsigned n, const K&k){
		return CompareByKey<K>(n, k);
	}

	// Sorting by key uses bucket sort, if the number of elements is
	// not significantly smaller than the number of keys. Otherwise
	// we use the sort by comparator functions, which (at the time
	// of writing this comment) forward to std::sort and
	// std::stable_sort. Bucket sort is always stable whereas
	// comparator-based sort is not. To avoid code dublication, we
	// therefore have maybe_stable functions that take a compile time
	// boolean which inidcates whether the fallback function should
	// be stable.

	template<bool is_stable, class V, class K>
	void compute_maybe_stable_sort_permutation_using_key_and_copy_into(const V&v, unsigned key_count, const K&get_key, Span<unsigned>p){
		assert(v.size() == p.size() && "array and permutation must have the same size");
		if(v.size() >= key_count / bucket_sort_min_key_to_element_ratio){
			std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
			for(unsigned i=0; i<v.size(); ++i){
				unsigned k = get_key(v[i]);
				assert(k <= key_count && "key is too large");
				p[key_pos[k]] = i;
				++key_pos[k];
			}
		}else if(is_stable){
			compute_stable_sort_permutation_using_comparator_and_copy_into(v, detail::make_compare_by_key(key_count, get_key), p);
		}else{
			compute_sort_permutation_using_comparator_and_copy_into(v, detail::make_compare_by_key(key_count, get_key), p);
		}
	}

	template<bool is_stable, class V, class K>
	void compute_inverse_maybe_stable_sort_permutation_using_key_and_copy_into(const V&v, unsigned key_count, const K&get_key, Span<unsigned>p){
		assert(v.size() == p.size() && "array and permutation must have the same size");
		if(v.size() >= key_count / bucket_sort_min_key_to_element_ratio){
			std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
			for(unsigned i=0; i<v.size(); ++i){
				unsigned k = get_key(v[i]);
				assert(k <= key_count && "key is too large");
				p[i] = key_pos[k];
				++key_pos[k];
			}
		} else if(is_stable){
			compute_inverse_stable_sort_permutation_using_comparator_and_copy_into(v, detail::make_compare_by_key(key_count, get_key), p);
		}else{
			compute_inverse_sort_permutation_using_comparator_and_copy_into(v, detail::make_compare_by_key(key_count, get_key), p);
		}
	}
}

template<class T, class K>
void compute_sort_permutation_using_key_and_copy_into(const std::vector<T>&v, unsigned key_count, const K&get_key, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	return detail::compute_maybe_stable_sort_permutation_using_key_and_copy_into<false>(v, key_count, get_key, p);
}

template<class T, class K>
std::vector<unsigned> compute_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	std::vector<unsigned>p(v.size());
	detail::compute_maybe_stable_sort_permutation_using_key_and_copy_into<false>(v, key_count, get_key, p);
	return p;
}

template<class T, class K>
void compute_stable_sort_permutation_using_key_and_copy_into(const std::vector<T>&v, unsigned key_count, const K&get_key, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	return detail::compute_maybe_stable_sort_permutation_using_key_and_copy_into<true>(v, key_count, get_key, p);
}

template<class T, class K>
std::vector<unsigned> compute_stable_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	std::vector<unsigned>p(v.size());
	detail::compute_maybe_stable_sort_permutation_using_key_and_copy_into<true>(v, key_count, get_key, p);
	return p;
}

template<class T, class K>
void compute_inverse_sort_permutation_using_key_and_copy_into(const std::vector<T>&v, unsigned key_count, const K&get_key, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	return detail::compute_inverse_maybe_stable_sort_permutation_using_key_and_copy_into<false>(v, key_count, get_key, p);
}

template<class T, class K>
std::vector<unsigned> compute_inverse_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	std::vector<unsigned>p(v.size());
	detail::compute_inverse_maybe_stable_sort_permutation_using_key_and_copy_into<false>(v, key_count, get_key, p);
	return p;
}

template<class T, class K>
void compute_inverse_stable_sort_permutation_using_key_and_copy_into(const std::vector<T>&v, unsigned key_count, const K&get_key, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	return detail::compute_inverse_maybe_stable_sort_permutation_using_key_and_copy_into<true>(v, key_count, get_key, p);
}

template<class T, class K>
std::vector<unsigned> compute_inverse_stable_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	std::vector<unsigned>p(v.size());
	detail::compute_inverse_maybe_stable_sort_permutation_using_key_and_copy_into<true>(v, key_count, get_key, p);
	return p;
}


template<class V, class K>
bool is_sorted_using_key(const V&v, unsigned key_count, const K&get_key){
	for(unsigned i=1; i<v.size(); ++i)
		if(get_key(v[i]) < get_key(v[i-1]))
			return false;
	return true;
}

template<class V>
void compute_sort_permutation_using_less(const V&v, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	compute_sort_permutation_using_comparator_and_copy_into(v, std::less<typename V::value_type>());
}

template<class V>
void compute_stable_sort_permutation_using_less_and_copy_into(const V&v, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	compute_stable_sort_permutation_using_comparator_and_copy_into(v, std::less<typename V::value_type>());
}

template<class V>
void compute_inverse_sort_permutation_using_less_and_copy_into(const V&v, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	compute_inverse_sort_permutation_using_comparator_and_copy_into(v, std::less<typename V::value_type>());
}

template<class V>
void compute_inverse_stable_sort_permutation_using_less_and_copy_into(const V&v, Span<unsigned>p){
	assert(v.size() == p.size() && "array and permutation must have the same size");
	compute_inverse_stable_sort_permutation_using_comparator_and_copy_into(v, std::less<typename V::value_type>());
}

template<class V>
std::vector<unsigned> compute_sort_permutation_using_less(const V&v){
	return compute_sort_permutation_using_comparator(v, std::less<typename V::value_type>());
}

template<class V>
std::vector<unsigned> compute_stable_sort_permutation_using_less(const V&v){
	return compute_stable_sort_permutation_using_comparator(v, std::less<typename V::value_type>());
}

template<class V>
std::vector<unsigned> compute_inverse_sort_permutation_using_less(const V&v){
	return compute_inverse_sort_permutation_using_comparator(v, std::less<typename V::value_type>());
}

template<class V>
std::vector<unsigned> compute_inverse_stable_sort_permutation_using_less(const V&v){
	return compute_inverse_stable_sort_permutation_using_comparator(v, std::less<typename V::value_type>());
}

template<class V>
bool is_sorted_using_less(const V&v){
	return is_sorted_using_comparator(v, std::less<typename V::value_type>());
}

} // RoutingKit2

#endif
