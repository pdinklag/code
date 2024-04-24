/**
 * code/huffman_code.hpp
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

#ifndef _CODE_HUFFMAN_CODE_HPP
#define _CODE_HUFFMAN_CODE_HPP

#include <cstdint>
#include <cstddef>

namespace code {

/**
 * \brief Represents a Huffman code
 * 
 */
struct HuffmanCode {
    /**
     * \brief Contains the Huffman codeword in LSBF order, occupying the low \ref length bits
     * 
     * The bit order, LSBF, means that the lowest bit determines whether to go left or right from the root node,
     * while the \c length -lowest bit will finally navigate to the leaf level of the Huffman tree.
     */
    uintmax_t word;

    /**
     * \brief The length of the Huffman codeword
     * 
     */
    size_t length;

    bool operator==(HuffmanCode const& other) const = default;
    bool operator!=(HuffmanCode const& other) const = default;
};

}

#endif
