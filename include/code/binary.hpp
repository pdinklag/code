/**
 * code/binary.hpp
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

#ifndef _CODE_BINARY_HPP
#define _CODE_BINARY_HPP

#include "concepts.hpp"

namespace code {

/**
 * \brief Binary encoding and decoding of integers
 * 
 * Instances of this class satisfy both the \ref code::IntegerEncoder "IntegerEncoder" and the \ref code::IntegerDecoder "IntegerDecoder" concepts.
 */
class Binary {
public:
    /**
     * \brief Encodes an integer using binary code and the specified number of bits
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param bits the number of bits
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x, size_t bits) {
        sink.write(x, bits);
    }

    /**
     * \brief Encodes an integer from the given universe using binary code
     * 
     * The number of encoded bits equals the universe's worst case entropy,
     * i.e., this function actually encodes the value of the integer relative to the universe's minimum.
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param u the universe of \c x
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x, Universe u) {
        encode(sink, u.rel(x), u.entropy());
    }

    /**
     * \brief Decodes an integer using binary code and the specified number of bits
     * 
     * \tparam Source the bit source type
     * \param src the bit sink
     * \param bits the number of bits
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src, size_t bits) {
        return src.read(bits);
    }

    /**
     * \brief Decodes an integer from the given universe using binary code
     * 
     * The number of decoded bits equals the universe's worst case entropy,
     * i.e., this function actually decodes the value of the integer relative to the universe's minimum and adds it afterwards.
     * 
     * \tparam Source the bit source type
     * \param src the bit source
     * \param u the universe of the integer to decode
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src, Universe u) {
        return u.abs(decode(src, u.entropy()));
    }
};

}

#endif
