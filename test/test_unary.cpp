/**
 * test_unary.cpp
 * part of pdinklag/code
 * 
 * MIT License
 * 
 * Copyright (c) 2023 Patrick Dinklage
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
