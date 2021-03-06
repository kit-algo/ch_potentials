#ifndef ROUTING_KIT2_MIN_MAX_H
#define ROUTING_KIT2_MIN_MAX_H

#include "span.h"
#include <assert.h>

namespace RoutingKit2{

template<class T>
void min_to(T&x, const T&y){
	if(y < x)
		x = y;
}

template<class T>
void max_to(T&x, const T&y){
	if(y > x)
		x = y;
}

template<class T>
unsigned first_min_element_position_of(const Span<T>&v){
	assert(!v.empty());
	unsigned pos = 0;
	for(unsigned i=1; i<v.size(); ++i)
		if(v[i]< v[pos])
			pos = i;
	return pos;
}

template<class T>
const T&min_element_of(const Span<T>&v){
	return v[first_min_element_position_of(v)];
}

template<class T>
const T&min_element_of(const Span<T>&v, const T&empty_value){
	if(v.empty())
		return empty_value;
	else
		return v[first_min_element_position_of(v)];
}


template<class T>
unsigned first_max_element_position_of(const Span<T>&v){
	assert(!v.empty());
	unsigned pos = 0;
	for(unsigned i=1; i<v.size(); ++i)
		if(v[i] > v[pos])
			pos = i;
	return pos;
}

template<class T>
const T&max_element_of(const Span<T>&v){
	return v[first_max_element_position_of(v)];
}

template<class T>
const T&max_element_of(const Span<T>&v, const T&empty_value){
	if(v.empty())
		return empty_value;
	else
		return v[first_max_element_position_of(v)];
}

} // RoutingKit2

#endif
