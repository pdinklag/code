#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <code/binary.hpp>
#include "helpers.hpp"

namespace code::test {

TEST_SUITE("code::Binary") {
    TEST_CASE("encode") {
        SimpleUint64BitSink sink;
        Binary::encode(sink, 0x12345678U, 64);
        CHECK(sink.value == 0x12345678U);
    }

    TEST_CASE("decode") {
        SimpleUint64BitSource src(0x12345678U);
        CHECK(Binary::decode(src, 64) == 0x12345678U);
    }
}

}
