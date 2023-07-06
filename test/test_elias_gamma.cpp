#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <code/elias_gamma.hpp>
#include "helpers.hpp"

namespace code::test {

TEST_SUITE("code::EliasGamma") {
    TEST_CASE("encode") {
        auto gamma_of = [](uint64_t v){
            SimpleUint64BitSink sink;
            EliasGamma::encode(sink, v);
            return sink.value;
        };

        CHECK(gamma_of(1) == 0);
        CHECK(gamma_of(2) == 0b0'01);
        CHECK(gamma_of(3) == 0b1'01);
        CHECK(gamma_of(4) == 0b00'011);
        CHECK(gamma_of(7) == 0b11'011);
        CHECK(gamma_of(8) == 0b000'0111);
        CHECK(gamma_of(15) == 0b111'0111);
        CHECK(gamma_of(16) == 0b0000'01111);
        CHECK(gamma_of(31) == 0b1111'01111);
        // ...
    }

    TEST_CASE("decode") {
        { SimpleUint64BitSource src(0); CHECK(EliasGamma::decode(src) == 1); }
        { SimpleUint64BitSource src(0b0'01); CHECK(EliasGamma::decode(src) == 2); }
        { SimpleUint64BitSource src(0b1'01); CHECK(EliasGamma::decode(src) == 3); }
        { SimpleUint64BitSource src(0b00'011); CHECK(EliasGamma::decode(src) == 4); }
        { SimpleUint64BitSource src(0b11'011); CHECK(EliasGamma::decode(src) == 7); }
        { SimpleUint64BitSource src(0b000'0111); CHECK(EliasGamma::decode(src) == 8); }
        { SimpleUint64BitSource src(0b111'0111); CHECK(EliasGamma::decode(src) == 15); }
        { SimpleUint64BitSource src(0b0000'01111); CHECK(EliasGamma::decode(src) == 16); }
        { SimpleUint64BitSource src(0b1111'01111); CHECK(EliasGamma::decode(src) == 31); }
        // ...
    }
}

}
