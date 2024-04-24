/**
 * test_huffman.cpp
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

#include <code/huffman.hpp>
#include <iopp/util/bit_packer.hpp>
#include <iopp/util/bit_unpacker.hpp>
#include "helpers.hpp"

namespace code::test {

TEST_SUITE("code::Huffman") {
    TEST_CASE("HuffmanTree") {
        // the following string has this histogram:
        // z : 1
        // y : 2
        // x : 3
        // w : 4
        // v : 5
        // u : 6
        // t : 7
        // s : 7
        std::string s = "zyyxxxwwwwvvvvvuuuuuutttttttsssssss";

        HuffmanTree<char> tree(s.begin(), s.end());
        CHECK(tree.size() == 15);
        CHECK(tree.root().freq() == s.length());

        SUBCASE("Codes") {
             // nb: LSBF
            CHECK(tree['z'] == HuffmanCode{0b11100U, 5});
            CHECK(tree['y'] == HuffmanCode{0b01100U, 5});
            CHECK(tree['x'] == HuffmanCode{0b0100U, 4});
            CHECK(tree['w'] == HuffmanCode{0b110U, 3});
            CHECK(tree['v'] == HuffmanCode{0b010U, 3});
            CHECK(tree['u'] == HuffmanCode{0b000U, 3});
            CHECK(tree['t'] == HuffmanCode{0b01U, 2});
            CHECK(tree['s'] == HuffmanCode{0b11U, 2});
        }

        SUBCASE("Encoding") {
            uint64_t out[8];
            {
                auto sink = iopp::BitPacker(out);
                tree.encode(sink);
            }
            {
                auto src = iopp::BitUnpacker(out);

                // the tree has 15 nodes and thus should have been encoded in 15 bits
                auto const bits = src.read(15);
                CHECK(bits == 0b110110110101000U); // nb: LSBF

                // we should now be able to decode the universe of ['s','z']
                auto const umin = EliasDelta::decode(src, Universe::umax());
                auto const umax = EliasDelta::decode(src, Universe::at_least(umin));
                CHECK(umin == 's');
                CHECK(umax == 'z');
                auto const u = Universe(umin, umax);

                // finally, we should be able to decode the characters in left-to-right order
                CHECK(Binary::decode(src, u) == 'u');
                CHECK(Binary::decode(src, u) == 'x');
                CHECK(Binary::decode(src, u) == 'y');
                CHECK(Binary::decode(src, u) == 'z');
                CHECK(Binary::decode(src, u) == 'v');
                CHECK(Binary::decode(src, u) == 'w');
                CHECK(Binary::decode(src, u) == 't');
                CHECK(Binary::decode(src, u) == 's');
            }
            {
                // ... and all of that should work automatically
                auto src = iopp::BitUnpacker(out);
                HuffmanTree<char> decoded_tree(src);

                CHECK(decoded_tree.size() == 15);
                CHECK(decoded_tree['z'] == HuffmanCode{0b11100U, 5});
                CHECK(decoded_tree['y'] == HuffmanCode{0b01100U, 5});
                CHECK(decoded_tree['x'] == HuffmanCode{0b0100U, 4});
                CHECK(decoded_tree['w'] == HuffmanCode{0b110U, 3});
                CHECK(decoded_tree['v'] == HuffmanCode{0b010U, 3});
                CHECK(decoded_tree['u'] == HuffmanCode{0b000U, 3});
                CHECK(decoded_tree['t'] == HuffmanCode{0b01U, 2});
                CHECK(decoded_tree['s'] == HuffmanCode{0b11U, 2});
            }
        }
    }

    std::string lorem_ipsum = 
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus aliquet in turpis vitae mattis. "
        "Etiam nunc nibh, ornare in tincidunt quis, iaculis eget orci. Morbi viverra maximus quam vel feugiat. "
        "Nulla est augue, vehicula eu ante non, dapibus dignissim purus. Donec at viverra est. Sed a rhoncus lectus. "
        "Maecenas a purus nisi. Donec aliquet dignissim tempor. Donec interdum pulvinar massa, sit amet finibus ante volutpat aliquet. "
        "Aliquam eget purus sed ex ornare imperdiet vel in lorem. Cras accumsan egestas malesuada. "
        "Phasellus mauris eros, congue non feugiat porttitor, commodo at quam. Vestibulum cursus enim ullamcorper tristique mattis.";

    TEST_CASE("roundtrip_tree") {
        // roundtrip lorem ipsum using the Huffman tree
        uintmax_t out[800]; // "large enough"
        {
            HuffmanTree<char> tree(lorem_ipsum.begin(), lorem_ipsum.end());
            auto sink = iopp::BitPacker(out);
            tree.encode(sink);
            for(char const c : lorem_ipsum) Huffman::encode(sink, c, tree);
        }
        {
            std::string decoded;
            decoded.reserve(lorem_ipsum.length());

            auto src = iopp::BitUnpacker(out);
            HuffmanTree<char> tree(src);
            for(size_t i = 0; i < lorem_ipsum.length(); i++) {
                decoded.push_back((char)Huffman::decode(src, tree.root()));
            }
            CHECK(decoded == lorem_ipsum);
        }
    }

    TEST_CASE("roundtrip_table") {
        // roundtrip lorem ipsum using the Huffman table
        uintmax_t out[800]; // "large enough"
        {
            auto sink = iopp::BitPacker(out);

            HuffmanTree<char> tree(lorem_ipsum.begin(), lorem_ipsum.end());
            tree.encode(sink);

            auto table = tree.table();
            for(char const c : lorem_ipsum) Huffman::encode(sink, c, table);
        }
        {
            std::string decoded;
            decoded.reserve(lorem_ipsum.length());

            auto src = iopp::BitUnpacker(out);
            HuffmanTree<char> tree(src);
            for(size_t i = 0; i < lorem_ipsum.length(); i++) {
                decoded.push_back((char)Huffman::decode(src, tree.root()));
            }
            CHECK(decoded == lorem_ipsum);
        }
    }
}

}
