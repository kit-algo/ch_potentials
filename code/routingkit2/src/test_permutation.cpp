#include "permutation.h"

#include "catch.hpp"

#include <vector>
#include <random>

using namespace RoutingKit2;
using namespace std;

typedef std::vector<unsigned> P;
typedef std::vector<std::string> S;

TEST_CASE("is_permutation", "[Permutation]"){
	REQUIRE(RoutingKit2::is_permutation(P{1,5,2,0,3,6,4}));
	REQUIRE(!is_permutation(P{1,5,2,3,6,4}));
	REQUIRE(!is_permutation(P{1,5,2,3,6,2,4}));
	REQUIRE(!is_permutation(P{1,5,0,2,3,6,2,4}));

	REQUIRE(is_permutation(P{}));
	REQUIRE(is_permutation(P{0}));
}

TEST_CASE("identity_permutation", "[Permutation]"){
	REQUIRE(identity_permutation(0) == P{});
	REQUIRE(identity_permutation(1) == P{0});
	REQUIRE(identity_permutation(2) == (P{0, 1}));
	REQUIRE(identity_permutation(10) == (P{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

}

TEST_CASE("invert_permutation", "[Permutation]"){
	P p = {0, 3, 1, 2};
	P inv_p = {0, 2, 3, 1};

	REQUIRE(invert_permutation(p) == inv_p);
	REQUIRE(invert_permutation(inv_p) == p);
	REQUIRE(invert_permutation(P{}) == P{});
	REQUIRE(invert_permutation(P{0}) == P{0});
}


TEST_CASE("apply_permutation", "[Permutation]"){
	REQUIRE(apply_inverse_permutation(P{}, S{}) == S{});
	REQUIRE(apply_permutation(P{}, S{}) == S{});

	P p = {0, 3, 1, 2};
	P inv_p = {0, 2, 3, 1};

	S o = {"a", "b", "c", "d"};
	S inv_p_o = {"a", "c", "d", "b"};
	S p_o = {"a", "d", "b", "c"};

	REQUIRE(apply_permutation(p, o) == p_o);
	REQUIRE(apply_permutation(inv_p, o) == inv_p_o);

	REQUIRE(apply_inverse_permutation(p, o) == inv_p_o);
	REQUIRE(apply_inverse_permutation(inv_p, o) == p_o);

	REQUIRE(apply_permutation(p, p) == inv_p);
	REQUIRE(apply_permutation(p, apply_permutation(p, p)) == identity_permutation(4));
	REQUIRE(apply_permutation(inv_p, inv_p) == p);
	REQUIRE(apply_permutation(apply_permutation(inv_p, inv_p), inv_p) == identity_permutation(4));
	REQUIRE(apply_permutation(p, inv_p) == identity_permutation(4));

	S c = o;
	S r(4);
	apply_permutation_and_move_into(p, c, r);
	REQUIRE(r == p_o);

	apply_inverse_permutation_and_move_into(p, r, c);
	REQUIRE(c == o);

	apply_inverse_permutation_and_move_into(p, c, r);
	REQUIRE(r == inv_p_o);
}

TEST_CASE("random_permutation", "[Permutation]"){
	auto q = random_permutation(10, std::minstd_rand0(42));
	REQUIRE(is_permutation(q));
}
