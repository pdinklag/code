/**
 * test_vbyte.cpp
 * part of pdinklag/code
 * 
 * MIT License
 * 
 * Copyright (c) Patrick Dinklage
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <code/vbyte.hpp>
#include "helpers.hpp"

namespace code::test {

TEST_SUITE("code::Vbyte") {
    TEST_CASE("encode") {
        auto vbyte_of = [](uint64_t v, uint8_t b){
            SimpleUint64BitSink sink;
            Vbyte::encode(sink, v, b);
            return sink.value;
        };

        CHECK(vbyte_of(0, 3) == 0b0001);
        CHECK(vbyte_of(7, 3) == 0b1111);
        CHECK(vbyte_of(8, 3) == 0b0011'0000);
        CHECK(vbyte_of(63, 3) == 0b1111'1110);
        CHECK(vbyte_of(64, 3) == 0b0011'0000'0000);
        CHECK(vbyte_of(511, 3) == 0b1111'1110'1110);
        CHECK(vbyte_of(512, 3) == 0b0011'0000'0000'0000);
        // ...
        CHECK(vbyte_of(0, 5) == 0b000001);
        CHECK(vbyte_of(31, 5) == 0b111111);
        CHECK(vbyte_of(32, 5) == 0b000011'000000);
        CHECK(vbyte_of(63, 5) == 0b000011'111110);
        CHECK(vbyte_of(64, 5) == 0b000101'000000);
        CHECK(vbyte_of(1023, 5) == 0b111111'111110);
        CHECK(vbyte_of(1024, 5) == 0b000011'000000'000000);
        // ...
    }

    TEST_CASE("decode") {
        { SimpleUint64BitSource src(0b0001); CHECK(Vbyte::decode(src, 3) == 0); }
        { SimpleUint64BitSource src(0b1111); CHECK(Vbyte::decode(src, 3) == 7); }
        { SimpleUint64BitSource src(0b0011'0000); CHECK(Vbyte::decode(src, 3) == 8); }
        { SimpleUint64BitSource src(0b1111'1110); CHECK(Vbyte::decode(src, 3) == 63); }
        { SimpleUint64BitSource src(0b0011'0000'0000); CHECK(Vbyte::decode(src, 3) == 64); }
        { SimpleUint64BitSource src(0b1111'1110'1110); CHECK(Vbyte::decode(src, 3) == 511); }
        { SimpleUint64BitSource src(0b0011'0000'0000'0000); CHECK(Vbyte::decode(src, 3) == 512); }
        // ...
        { SimpleUint64BitSource src(0b000001); CHECK(Vbyte::decode(src, 5) == 0); }
        { SimpleUint64BitSource src(0b111111); CHECK(Vbyte::decode(src, 5) == 31); }
        { SimpleUint64BitSource src(0b000011'000000); CHECK(Vbyte::decode(src, 5) == 32); }
        { SimpleUint64BitSource src(0b000011'111110); CHECK(Vbyte::decode(src, 5) == 63); }
        { SimpleUint64BitSource src(0b000101'000000); CHECK(Vbyte::decode(src, 5) == 64); }
        { SimpleUint64BitSource src(0b111111'111110); CHECK(Vbyte::decode(src, 5) == 1023); }
        { SimpleUint64BitSource src(0b000011'000000'000000); CHECK(Vbyte::decode(src, 5) == 1024); }
        // ...
    }
}

}
