#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <code/unary.hpp>
#include "helpers.hpp"

namespace code::test {

TEST_SUITE("code::Unary") {
    TEST_CASE("encode") {
        auto unary_of = [](uint64_t v){
            SimpleUint64BitSink sink;
            Unary::encode(sink, v);
            return sink.value;
        };
        
        CHECK(unary_of(0) == 0);
        CHECK(unary_of(1) == 0b1);
        CHECK(unary_of(2) == 0b11);
        CHECK(unary_of(3) == 0b111);
        CHECK(unary_of(7) == UINT8_MAX >> 1);
        CHECK(unary_of(15) == UINT16_MAX >> 1);
        CHECK(unary_of(31) == UINT32_MAX >> 1);
        CHECK(unary_of(63) == UINT64_MAX >> 1);
    }

    TEST_CASE("decode") {
        { SimpleUint64BitSource src(0); CHECK(Unary::decode(src) == 0); }
        { SimpleUint64BitSource src(0b1); CHECK(Unary::decode(src) == 1); }
        { SimpleUint64BitSource src(0b11); CHECK(Unary::decode(src) == 2); }
        { SimpleUint64BitSource src(0b111); CHECK(Unary::decode(src) == 3); }
        { SimpleUint64BitSource src(UINT8_MAX >> 1); CHECK(Unary::decode(src) == 7); }
        { SimpleUint64BitSource src(UINT16_MAX >> 1); CHECK(Unary::decode(src) == 15); }
        { SimpleUint64BitSource src(UINT32_MAX >> 1); CHECK(Unary::decode(src) == 31); }
        { SimpleUint64BitSource src(UINT64_MAX >> 1); CHECK(Unary::decode(src) == 63); }
    }
}

}
