#include "sort.h"
#include "permutation.h"

#include "catch.hpp"

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <random>

using namespace RoutingKit2;
using namespace std;

unsigned range_list[] = {1, 2, 3, 5, 10, 100, 1000, 10000, 1000000000};


TEST_CASE("StableSort", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<unsigned>v(10000);
		for(auto&x:v)
			x = rng()%range;

		auto q = compute_stable_sort_permutation_using_key(v, range, [](unsigned x){return x;});
		REQUIRE(is_sorted_using_key(apply_permutation(q, v), range, [](unsigned x){return x;}));
		REQUIRE(is_sorted_using_less(apply_permutation(q, v)));
		REQUIRE(q == compute_stable_sort_permutation_using_less(v));
		REQUIRE(q == compute_stable_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l<r;}));
	}
}

TEST_CASE("Sort", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<unsigned>v(10000);
		for(auto&x:v)
			x = rng()%range;

		REQUIRE(is_sorted_using_less(apply_permutation(compute_sort_permutation_using_key(v, range, [](unsigned x){return x;}), v)));
		REQUIRE(is_sorted_using_less(apply_permutation(compute_sort_permutation_using_less(v), v)));
		REQUIRE(is_sorted_using_less(apply_permutation(compute_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l<r;}), v)));
	}
}

TEST_CASE("InverseStableSort", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<unsigned>v(10000);
		for(auto&x:v)
			x = rng()%range;

		auto q = compute_inverse_stable_sort_permutation_using_key(v, range, [](unsigned x){return x;});
		REQUIRE(is_sorted_using_key(apply_inverse_permutation(q, v), range, [](unsigned x){return x;}));
		REQUIRE(is_sorted_using_less(apply_inverse_permutation(q, v)));
		REQUIRE(q == compute_inverse_stable_sort_permutation_using_less(v));
		REQUIRE(q == compute_inverse_stable_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l<r;}));
	}
}

TEST_CASE("InverseSort", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<unsigned>v(10000);
		for(auto&x:v)
			x = rng()%range;

		REQUIRE(is_sorted_using_less(apply_inverse_permutation(compute_inverse_sort_permutation_using_key(v, range, [](unsigned x){return x;}), v)));
		REQUIRE(is_sorted_using_less(apply_inverse_permutation(compute_inverse_sort_permutation_using_less(v), v)));
		REQUIRE(is_sorted_using_less(apply_inverse_permutation(compute_inverse_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l<r;}), v)));
	}
}

TEST_CASE("ReverseStableSort", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<unsigned>v(10000);
		for(auto&x:v)
			x = rng()%range;

		auto q = compute_stable_sort_permutation_using_key(v, range, [range](unsigned x){return range-x-1;});
		REQUIRE(is_sorted_using_key(apply_permutation(q, v), range, [range](unsigned x){return range-x-1;}));
		REQUIRE(is_sorted_using_comparator(apply_permutation(q, v), [](unsigned l, unsigned r){return l>r;}));
		REQUIRE(q == compute_stable_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l>r;}));
	}
}

TEST_CASE("ReverseSort", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<unsigned>v(10000);
		for(auto&x:v)
			x = rng()%range;

		REQUIRE(is_sorted_using_comparator(apply_permutation(compute_sort_permutation_using_key(v, range, [range](unsigned x){return range-x-1;}), v), [](unsigned l, unsigned r){return l>r;}));
		REQUIRE(is_sorted_using_comparator(apply_permutation(compute_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l>r;}), v), [](unsigned l, unsigned r){return l>r;}));
	}
}

TEST_CASE("ReverseInverseStableSort", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<unsigned>v(10000);
		for(auto&x:v)
			x = rng()%range;

		auto q = compute_inverse_stable_sort_permutation_using_key(v, range, [range](unsigned x){return range-x-1;});
		REQUIRE(is_sorted_using_key(apply_inverse_permutation(q, v), range, [range](unsigned x){return range-x-1;}));
		REQUIRE(is_sorted_using_comparator(apply_inverse_permutation(q, v), [](unsigned l, unsigned r){return l>r;}));
		REQUIRE(q == compute_inverse_stable_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l>r;}));
	}
}

TEST_CASE("ReverseInverseSort", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<unsigned>v(10000);
		for(auto&x:v)
			x = rng()%range;

		REQUIRE(is_sorted_using_comparator(apply_inverse_permutation(compute_inverse_sort_permutation_using_key(v, range, [range](unsigned x){return range-x-1;}), v), [](unsigned l, unsigned r){return l>r;}));
		REQUIRE(is_sorted_using_comparator(apply_inverse_permutation(compute_inverse_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l>r;}), v), [](unsigned l, unsigned r){return l>r;}));
	}
}

/*

*/

struct Point{
	unsigned x, y;
};

bool operator==(Point l, Point r){
	return l.x == r.x && l.y == r.y;
}

TEST_CASE("StableSortPoint", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<Point>v(10000);
		for(auto&p:v){
			p.x = rng()%range;
			p.y = rng()%range;
		}

		auto get_key = [&](Point p){
			return std::min(p.x, p.y);
		};

		auto is_less = [&](Point l, Point r){
			return  get_key(l) < get_key(r);
		};


		auto s = v;
		std::stable_sort(s.begin(), s.end(), is_less);

		auto q = compute_stable_sort_permutation_using_key(v, range, get_key);
		REQUIRE(s == apply_permutation(q, v));
		REQUIRE(is_sorted_using_key(apply_permutation(q, v), range, get_key));
		REQUIRE(q == compute_stable_sort_permutation_using_comparator(v, is_less));
	}
}

TEST_CASE("StableInverseSortPoint", "[Sort]"){
	std::minstd_rand rng(42);
	for(unsigned range:range_list){
		vector<Point>v(10000);
		for(auto&p:v){
			p.x = rng()%range;
			p.y = rng()%range;
		}

		auto get_key = [&](Point p){
			return std::min(p.x, p.y);
		};

		auto is_less = [&](Point l, Point r){
			return  get_key(l) < get_key(r);
		};


		auto s = v;
		std::stable_sort(s.begin(), s.end(), is_less);

		auto q = compute_inverse_stable_sort_permutation_using_key(v, range, get_key);
		REQUIRE(s == apply_inverse_permutation(q, v));
		REQUIRE(is_sorted_using_key(apply_inverse_permutation(q, v), range, get_key));
		REQUIRE(q == compute_inverse_stable_sort_permutation_using_comparator(v, is_less));
	}
}
