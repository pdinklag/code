/**
 * test_examples.cpp
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

#include <iopp/bitwise_io.hpp>
#include <code.hpp>

#define REQUIRE(x) if(!(x)) std::abort();

void example_universal() {
    // define a universe of numbers between 10 and 20 (each inclusive)
    code::Universe u(10, 20);

    // encode
    std::string buffer;
    {
        auto sink = iopp::bitwise_output_to(std::back_inserter(buffer));

        code::Binary::encode(sink, 17, 5);     // encode binary with an explicit number of 5 bits
        code::Binary::encode(sink, 17, u);     // encode binary using our universe (encodes 17-10 = 7 with 4 bits)
        code::Unary::encode(sink, 11);         // encode unary (with 12 bits)
        code::Unary::encode(sink, 11, u);      // encode unary using our universe (encodes 11-10 = 1 with 2 bits)
        code::EliasGamma::encode(sink, 12);    // encode gamma (with 7 bits)
        code::EliasGamma::encode(sink, 12, u); // encode gamma using universe (encodes 12-10 = 2 with 3 bits)
        code::Rice::encode(sink, 13, 3);       // encode Rice using a Golomb exponent of 3
        code::Rice::encode(sink, 13, 3, u);    // encode Rice using a Golomb exponent of 3 and our universe (encodes 13-10 = 3)
        code::Vbyte::encode(sink, 18, 8);      // encode vbyte using a byte size of 8 bits
        code::Vbyte::encode(sink, 18, 8, u);   // encode vbyte using a byte size of 8 bits and our universe (encoded 18-10 = 8)
    }

    // decode
    {
        auto src = iopp::bitwise_input_from(buffer.begin(), buffer.end());

        auto const binary1 = code::Binary::decode(src, 5);    // decode binary with an explicit number of 5 bits
        REQUIRE(binary1 == 17);
        auto const binary2 = code::Binary::decode(src, u);    // decode binary using our universe
        REQUIRE(binary2 == 17);
        auto const unary1 = code::Unary::decode(src);         // decode unary
        REQUIRE(unary1 == 11);
        auto const unary2 = code::Unary::decode(src, u);      // decode unary using our universe
        REQUIRE(unary2 == 11);
        auto const gamma1 = code::EliasGamma::decode(src);    // decode gamma
        REQUIRE(gamma1 == 12);
        auto const gamma2 = code::EliasGamma::decode(src, u); // decode gamma using our universe
        REQUIRE(gamma2 == 12);
        auto const rice1 = code::Rice::decode(src, 3);        // decode Rice using a Golomb exponent of 3
        REQUIRE(rice1 == 13);
        auto const rice2 = code::Rice::decode(src, 3, u);     // decode Rice using a Golomb exponent of 3 and our universe
        REQUIRE(rice2 == 13);
        auto const vbyte1 = code::Vbyte::decode(src, 8);      // decode vbyte using a byte size of 8 bits
        REQUIRE(vbyte1 == 18);
        auto const vbyte2 = code::Vbyte::decode(src, 8, u);   // decode vbyte using a byte size of 8 bits and our universe
        REQUIRE(vbyte2 == 18);
    }
}

void example_huffman() {
    std::string const input_str = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus aliquet in turpis vitae mattis.";

    // encode
    std::string buffer;
    {
        auto sink = iopp::bitwise_output_to(std::back_inserter(buffer));

        // construct the Huffman tree for the input
        code::HuffmanTree<char> huffman_tree(input_str.begin(), input_str.end());

        // encode the Huffman tree
        huffman_tree.encode(sink);

        // compute the Huffman table for faster code access
        auto const huffman_table = huffman_tree.table();

        // encode the input
        for(char const c : input_str) {
            code::Huffman::encode(sink, c, huffman_table);
        }
    }

    // decode
    {
        auto src = iopp::bitwise_input_from(buffer.begin(), buffer.end());

        // decode the Huffman tree and get the root node
        code::HuffmanTree<char> huffman_tree(src);
        auto const& huffman_tree_root = huffman_tree.root();

        // decode the input
        std::string decoded_str;
        while(src) {
            decoded_str.push_back(char(code::Huffman::decode(src, huffman_tree_root)));
        }

        REQUIRE(decoded_str == input_str);
    }
}

int main() {
    example_universal();
    example_huffman();
    return 0;
}
