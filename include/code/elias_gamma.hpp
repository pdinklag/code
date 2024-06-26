/**
 * code/elias_gamma.hpp
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

#ifndef _CODE_ELIAS_GAMMA_HPP
#define _CODE_ELIAS_GAMMA_HPP

#include <cassert>
#include <bit>

#include "unary.hpp"
#include "binary.hpp"

#include "internal/bits.hpp"

namespace code {

/**
 * \brief Elias-Gamma encoding and decoding of integers
 * 
 * The gamma code of an integer is defined as the its unary-encoded worst-case entropy in bits, followed by the integer's binary code using that number of bits minus one (skipping the leading 1).
 * Note that the gamma code is particularly not defined for zero.
 * 
 * Instances of this class satisfy both the \ref code::IntegerEncoder "IntegerEncoder" and the \ref code::IntegerDecoder "IntegerDecoder" concepts.
 */
class EliasGamma {
public:
    /**
     * \brief Encodes an integer using gamma codes
     * 
     * Beware that the gamma code for zero is not defined, and trying to encode zero using this function causes undefined behaviour!
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x) {
        assert(x > 0);
        auto const m = std::bit_width(x) - 1;
        Unary::encode(sink, m);
        if(m) Binary::encode(sink, x, m); // cut off leading 1-bit
    }

    /**
     * \brief Encodes an integer from the given universe using gamma code
     * 
     * This function actually encodes one plus the value of the integer relative to the universe's minimum.
     * Note that therefore, trying to encode \c UINTMAX_MAX is undefined.
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param u the universe of \c x
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x, Universe u) {
        assert(u.rel(x) < UINTMAX_MAX); // nb: we CANNOT encode/decode the value UINTMAX_MAX -- shame on you if you were to do this using Elias codes anyway
        encode(sink, u.rel(x) + 1);
    }

    /**
     * \brief Decodes an integer using gamma code
     * 
     * \tparam Source the bit source type
     * \param src the bit sink
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src) {
        auto const m = Unary::decode(src);
        return m ? (internal::set_bit(m) | Binary::decode(src, m)) : 1;
    }

    /**
     * \brief Decodes an integer from the given universe using gamma code
     * 
     * This function actually decodes the value of the integer relative to the universe's minimum and adds it afterwards.
     * 
     * \tparam Source the bit source type
     * \param src the bit source
     * \param u the universe of the integer to decode
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src, Universe u) {
        return u.abs(decode(src)) - 1;
    }
};

}

#endif
