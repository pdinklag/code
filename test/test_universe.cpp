#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <code/universe.hpp>

namespace code::test {

TEST_SUITE("code::Universe") {
    TEST_CASE("properties") {
        CHECK(Universe::binary().entropy() == 1);
        CHECK(Universe::of<uint8_t>().entropy() == 8);
        CHECK(Universe::of<uint16_t>().entropy() == 16);
        CHECK(Universe::of<uint32_t>().entropy() == 32);
        CHECK(Universe::of<uint64_t>().entropy() == 64);
        CHECK(Universe::of<uintmax_t>().entropy() == std::numeric_limits<uintmax_t>::digits);

        CHECK(Universe::with_entropy(5).entropy() == 5);
        CHECK(Universe::with_entropy(5).max() == 0x1F);
        CHECK(Universe::with_entropy(20).entropy() == 20);
        CHECK(Universe::with_entropy(20).max() == 0xF'FF'FF);

        CHECK(Universe(0, 2).entropy() == 2);
        CHECK(Universe(0, 3).entropy() == 2);
        CHECK(Universe(0, 4).entropy() == 3);
        CHECK(Universe(0, 0x0FFFFFF).entropy() == 24);
        CHECK(Universe(0, 0x1000000).entropy() == 25);

        CHECK(Universe(1, 2).entropy() == 1);
        CHECK(Universe(1, 3).entropy() == 2);
        CHECK(Universe(0x0FFFFFF, 0x1000000).entropy() == 1);

        Universe u(53'748, 1'287'536);
        CHECK(u.delta() == 1'233'788);
        CHECK(u.entropy() == 21);
        CHECK(u.rel(u.min()) == 0);
        CHECK(u.rel(u.min() + 1) == 1);
        CHECK(u.rel(u.max()) == u.delta());
        CHECK(u.abs(0) == u.min());
        CHECK(u.abs(1) == u.min() + 1);
        CHECK(u.abs(u.delta()) == u.max());
    }
}

}
