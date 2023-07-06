#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <code/rice.hpp>
#include "helpers.hpp"

namespace code::test {

TEST_SUITE("code::Rice") {
    TEST_CASE("encode") {
        auto rice_of = [](uint64_t v, uint8_t p){
            SimpleUint64BitSink sink;
            Rice::encode(sink, v, p);
            return sink.value;
        };

        CHECK(rice_of(0, 5) == 0b00000'0); // gamma_of(1) + 00000
        CHECK(rice_of(31, 5) == 0b11111'0); // gamma_of(1) + 11111
        CHECK(rice_of(32, 5) == 0b00000'0'01); // gamma_of(2) + 00000
        CHECK(rice_of(63, 5) == 0b11111'0'01); // gamma_of(2) + 11111
        CHECK(rice_of(64, 5) == 0b00000'1'01); // gamma_of(3) + 00000
        CHECK(rice_of(95, 5) == 0b11111'1'01); // gamma_of(3) + 11111
        CHECK(rice_of(96, 5) == 0b00000'00'011); // gamma_of(4) + 00000
        CHECK(rice_of(127, 5) == 0b11111'00'011); // gamma_of(4) + 11111
        // ...
        CHECK(rice_of(0, 6) == 0b000000'0); // gamma_of(1) + 000000
        CHECK(rice_of(63, 6) == 0b111111'0); // gamma_of(1) + 111111
        CHECK(rice_of(64, 6) == 0b000000'0'01); // gamma_of(2) + 000000
        CHECK(rice_of(127, 6) == 0b111111'0'01); // gamma_of(2) + 111111
        // ...
    }

    TEST_CASE("decode") {
        { SimpleUint64BitSource src(0b00000'0); CHECK(Rice::decode(src, 5) == 0); }
        { SimpleUint64BitSource src(0b11111'0); CHECK(Rice::decode(src, 5) == 31); }
        { SimpleUint64BitSource src(0b00000'0'01); CHECK(Rice::decode(src, 5) == 32); }
        { SimpleUint64BitSource src(0b11111'0'01); CHECK(Rice::decode(src, 5) == 63); }
        { SimpleUint64BitSource src(0b00000'1'01); CHECK(Rice::decode(src, 5) == 64); }
        { SimpleUint64BitSource src(0b11111'1'01); CHECK(Rice::decode(src, 5) == 95); }
        { SimpleUint64BitSource src(0b00000'00'011); CHECK(Rice::decode(src, 5) == 96); }
        { SimpleUint64BitSource src(0b11111'00'011); CHECK(Rice::decode(src, 5) == 127); }
        // ...
        { SimpleUint64BitSource src(0b000000'0); CHECK(Rice::decode(src, 6) == 0); }
        { SimpleUint64BitSource src(0b111111'0); CHECK(Rice::decode(src, 6) == 63); }
        { SimpleUint64BitSource src(0b000000'0'01); CHECK(Rice::decode(src, 6) == 64); }
        { SimpleUint64BitSource src(0b111111'0'01); CHECK(Rice::decode(src, 6) == 127); }
        // ...
    }
}

}
