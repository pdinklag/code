/**
 * test_universal.cpp
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

#include <code.hpp>
#include <iopp/util/bit_packer.hpp>
#include <iopp/util/bit_unpacker.hpp>
#include "helpers.hpp"

namespace code::test {

TEST_SUITE("code::Universal") {
    TEST_CASE("Universal Codes") {
        // 100 numbers in [0, 255] from random.org using seed "tdc"
        std::vector<uintmax_t> input8 = {
            27, 70, 139, 92, 112, 56, 46, 118, 203, 189,
            148, 68, 215, 185, 120, 24, 211, 234, 231, 218,
            227, 99, 109, 152, 7, 136, 1, 166, 173, 74,
            98, 26, 201, 215, 221, 34, 39, 92, 39, 73,
            150, 41, 250, 110, 129, 189, 230, 75, 182, 101,
            213, 83, 244, 60, 90, 250, 186, 8, 16, 188,
            211, 128, 134, 139, 81, 30, 176, 87, 185, 235,
            210, 239, 87, 29, 171, 67, 101, 114, 75, 87,
            183, 94, 166, 43, 144, 40, 139, 219, 1, 135,
            13, 236, 209, 117, 241, 164, 219, 60, 157, 85
        };
        auto u8 = Universe::with_entropy(8);

        // 100 numbers in [0, 8191] from random.org using seed "tdc"
        std::vector<uintmax_t> input13 = {
            872, 6701, 3640, 898, 7405, 4847, 3234, 1239, 5935, 99,
            2549, 3709, 5574, 6363, 3264, 1928, 52, 6837, 1329, 428,
            5039, 5960, 4410, 7207, 2354, 6311, 7479, 2075, 7116, 4845,
            4910, 5459, 7815, 4459, 7517, 129, 377, 5344, 1076, 2897,
            982, 350, 7413, 7470, 7854, 1898, 6683, 1394, 2410, 7901,
            3923, 697, 81, 758, 6156, 1805, 7578, 1495, 6354, 3507,
            6458, 5464, 7326, 3591, 7173, 93, 5547, 2752, 523, 6239,
            693, 373, 2642, 7712, 6464, 5818, 7363, 272, 5468, 1213,
            7065, 2489, 844, 7340, 4399, 7142, 4290, 8040, 3669, 4712,
            4426, 8090, 3074, 3838, 7613, 6423, 4319, 4110, 7625, 4228
        };
        auto u13 = Universe::with_entropy(13);

        // 100 numbers in [2846, 15361] from random.org using seed "tdc"
        std::vector<uintmax_t> inputx = {
            4591, 13267, 10448, 7217, 8919, 10532, 6236, 14013, 9729, 9209,
            9374, 10558, 3268, 13936, 12607, 13893, 8955, 5031, 10308, 8519,
            13113, 5844, 9363, 8285, 7183, 9752, 14752, 3113, 2980, 11762,
            7433, 3196, 3715, 10160, 10392, 12617, 12072, 5083, 14116, 4651,
            8829, 4529, 9818, 12915, 8535, 6742, 4862, 5411, 9678, 3107,
            4232, 4339, 7605, 11562, 13108, 14807, 11426, 7112, 5516, 7364,
            10186, 11645, 15032, 4405, 5584, 9536, 12444, 9598, 4109, 14691,
            15148, 3374, 9873, 6862, 8008, 5708, 14415, 8622, 9142, 14571,
            9460, 10100, 8766, 12221, 7337, 11590, 10813, 5694, 6802, 12213,
            15033, 7181, 6961, 5369, 14342, 12208, 3467, 13391, 5475, 6781
        };
        auto ux = Universe(2846, 15361);

        uintmax_t out[800]; // "large enough"
        auto run_for = [&](auto& coder, auto& decoder){
            auto sink = iopp::BitPacker(out);
            for(auto x : input8) coder.encode(sink, x, u8);
            for(auto x : input13) coder.encode(sink, x, u13);
            for(auto x : inputx) coder.encode(sink, x, ux);
            for(auto x : input8) coder.encode(sink, x, u8);
            sink.flush();

            auto src = iopp::BitUnpacker(out);
            for(auto x : input8) REQUIRE(decoder.decode(src, u8) == x);
            for(auto x : input13) REQUIRE(decoder.decode(src, u13) == x);
            for(auto x : inputx) REQUIRE(decoder.decode(src, ux) == x);
            for(auto x : input8) REQUIRE(decoder.decode(src, u8) == x);
        };

        SUBCASE("Binary") {
            auto coder = Binary();
            run_for(coder, coder);
        }
        SUBCASE("EliasGamma") {
            auto coder = EliasGamma();
            run_for(coder, coder);
        }
        SUBCASE("EliasDelta") {
            auto coder = EliasDelta();
            run_for(coder, coder);
        }
        SUBCASE("Rice(5)") {
            auto coder = Rice(5);
            run_for(coder, coder);
        }
        SUBCASE("Rice(8)") {
            auto coder = Rice(8);
            run_for(coder, coder);
        }
        SUBCASE("Vbyte(3)") {
            auto coder = Vbyte(3);
            run_for(coder, coder);
        }
        SUBCASE("Vbyte(7)") {
            auto coder = Vbyte(7);
            run_for(coder, coder);
        }
    }
}

}
