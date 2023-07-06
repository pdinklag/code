/**
 * code/vbyte.hpp
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

#ifndef _CODE_VBYTE_HPP
#define _CODE_VBYTE_HPP

#include <bit>

#include "concepts.hpp"

namespace code {

/**
 * \brief Vbyte encoding and decoding of integers
 * 
 * In vbyte coding, the integer to be encoded is split into blocks of a fixed size.
 * These blocks are encoded separately, each preceded by a bit indicating whether the block contains the integer's highest bit.
 * 
 * Instances of this class satisfy both the \ref tdc::code::IntegerEncoder "IntegerEncoder" and the \ref tdc::code::IntegerDecoder "IntegerDecoder" concepts.
 */
class Vbyte {
public:
    /**
     * \brief Encodes an integer using vbyte code with the specified block size
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param b the vbyte block size
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x, uint8_t b) {
        auto bits = std::bit_width(x);
        while(bits > b) {
            sink.write(0);
            sink.write(x, b);

            x >>= b;
            bits -= b;
        }

        sink.write(1);
        sink.write(x, b);
    }

    /**
     * \brief Encodes an integer from the given universe using rice code with the specified divisor
     * 
     * This function actually encodes the value of the integer relative to the universe's minimum.
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param b the vbyte block size
     * \param u the universe of \c x
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x, uint8_t b, Universe u) {
        encode(sink, u.rel(x), b);
    }

    /**
     * \brief Decodes an integer using rice code with the specified divisor
     * 
     * \tparam Source the bit source type
     * \param src the bit sink
     * \param b the vbyte block size
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src, uint8_t b) {
        size_t bits = 0;
        uintmax_t x = 0;
        while(!src.read()) {
            x |= src.read(b) << bits;
            bits += b;
        }
        x |= src.read(b) << bits;
        return x;
    }

    /**
     * \brief Decodes an integer from the given universe using rice code with the specified divisor
     * 
     * This function actually decodes the value of the integer relative to the universe's minimum and adds it afterwards.
     * 
     * \tparam Source the bit source type
     * \param src the bit source
     * \param b the vbyte block size
     * \param u the universe of the integer to decode
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src, uint8_t b, Universe u) {
        return u.abs(decode(src, b));
    }

private:
    uint8_t block_;

public:
    /**
     * \brief Constructs a vbyte coder with a fixed block size
     * 
     * \param block the block size
     */
    inline constexpr Vbyte(uint8_t block) : block_(block) {
    }

    /**
     * \brief Encodes an integer from the given universe using vbyte code
     * 
     * This function actually encodes the value of the integer relative to the universe's minimum.
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param u the universe of \c x
     */
    template<BitSink Sink>
    inline void encode(Sink& sink, uintmax_t x, Universe u) {
        encode(sink, x, block_, u);
    }

    /**
     * \brief Decodes an integer from the given universe using vbyte code
     * 
     * This function actually decodes the value of the integer relative to the universe's minimum and adds it afterwards.
     * 
     * \tparam Source the bit source type
     * \param src the bit source
     * \param u the universe of the integer to decode
     * \return the decoded integer
     */
    template<BitSource Source>
    inline uintmax_t decode(Source& src, Universe u) {
        return decode(src, block_, u);
    }

    /**
     * \brief Reports the block size used by this coder
     * 
     * \return the vbyte block size
     */
    uint8_t block() const { return block_; }
};

}

#endif
