/**
 * test_elias_delta.cpp
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

#include <code/elias_delta.hpp>
#include "helpers.hpp"

namespace code::test {

TEST_SUITE("code::EliasDelta") {
    TEST_CASE("encode") {
        auto delta_of = [](uint64_t v){
            SimpleUint64BitSink sink;
            EliasDelta::encode(sink, v);
            return sink.value;
        };

        CHECK(delta_of(1) == 0);
        CHECK(delta_of(2) == 0b0'0'01); // gamma_of(2) + 0
        CHECK(delta_of(3) == 0b1'0'01); // gamma_of(2) + 1
        CHECK(delta_of(4) == 0b00'1'01); // gamma_of(3) + 00
        CHECK(delta_of(7) == 0b11'1'01); // gamma_of(3) + 11
        CHECK(delta_of(8) == 0b000'00'011); // gamma_of(4) + 000
        CHECK(delta_of(15) == 0b111'00'011); // gamma_of(4) + 111
        CHECK(delta_of(16) == 0b0000'01'011); // gamma_of(5) + 0000
        CHECK(delta_of(31) == 0b1111'01'011); // gamma_of(5) + 1111
        CHECK(delta_of(32) == 0b00000'10'011); // gamma_of(6) + 00000
        CHECK(delta_of(63) == 0b11111'10'011); // gamma_of(6) + 11111
        // ...
    }

    TEST_CASE("decode") {
        { SimpleUint64BitSource src(0); CHECK(EliasDelta::decode(src) == 1); }
        { SimpleUint64BitSource src(0b0'0'01); CHECK(EliasDelta::decode(src) == 2); }
        { SimpleUint64BitSource src(0b1'0'01); CHECK(EliasDelta::decode(src) == 3); }
        { SimpleUint64BitSource src(0b00'1'01); CHECK(EliasDelta::decode(src) == 4); }
        { SimpleUint64BitSource src(0b11'1'01); CHECK(EliasDelta::decode(src) == 7); }
        { SimpleUint64BitSource src(0b000'00'011); CHECK(EliasDelta::decode(src) == 8); }
        { SimpleUint64BitSource src(0b111'00'011); CHECK(EliasDelta::decode(src) == 15); }
        { SimpleUint64BitSource src(0b0000'01'011); CHECK(EliasDelta::decode(src) == 16); }
        { SimpleUint64BitSource src(0b1111'01'011); CHECK(EliasDelta::decode(src) == 31); }
        { SimpleUint64BitSource src(0b00000'10'011); CHECK(EliasDelta::decode(src) == 32); }
        { SimpleUint64BitSource src(0b11111'10'011); CHECK(EliasDelta::decode(src) == 63); }
        // ...
    }
}

}
