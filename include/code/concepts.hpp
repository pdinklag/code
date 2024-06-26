/**
 * code/concepts.hpp
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

#ifndef _CODE_CONCEPTS_HPP
#define _CODE_CONCEPTS_HPP

#include <concepts>
#include <cstddef>
#include <cstdint>

#include "universe.hpp"

namespace code {

/**
 * \brief Concept for types that accept bitwise input
 * 
 * In order to satisfy this concept, the type must provide
 * * a function `write` accepting a single bit as a boolean value,
 * * an overload of `write` accepting an unsigned integer containing the bits to be written, as well as the number of bits to write, and
 * * a function `flush` that flushes any current intermediate state to the sink
 * * a function `num_bits_written` that tells the number of bits written since instantiation or last reset
 * 
 * This concept is compatible to `iopp::BitSink`.
 * 
 * \tparam T the type
 */
template<typename T>
concept BitSink =
    requires(T subject) {
        { subject.flush() };
    } &&
    requires(T subject, bool bit) {
        { subject.write(bit) };
    } &&
    requires(T subject, uintmax_t bits, size_t num) {
        { subject.write(bits, num) };
    } &&
    requires(T const subject) {
        { subject.num_bits_written() } -> std::unsigned_integral;
    };

/**
 * \brief Concept for types from which bits can be extracted
 * 
 * In order to satisfy this concept, the type must provide two functions:
 * * `read` to extract a single bit, and
 * * `read` to extract a given number of bits as an unsigned integer
 * 
 * This concept is compatible to `iopp::BitSource`.
 * 
 * \tparam T the type
 */
template<typename T>
concept BitSource =
    requires(T subject) {
        { subject.read() } -> std::same_as<bool>;
    } && requires(T subject, size_t num) {
        { subject.read(num) } -> std::unsigned_integral;
    };

/// \cond INTERNAL
struct SomeBitSink {
    inline void flush() { }
    inline void write(bool) { }
    inline void write(uintmax_t, size_t) { }
    inline size_t num_bits_written() const { return 0; }
};

struct SomeBitSource {
    inline bool read() { return false; }
    inline uintmax_t read(size_t) { return 0; }
};
/// \endcond

/**
 * \brief Concept for types that provide decoding of integers from a \ref tdc::code::BitSource "BitSource"
 * 
 * In order to satisfy this concept, the class must define a function \c decode , which decodes an integer
 * in a given \ref tdc::code::Universe "Universe" from a bit source.
 * 
 * \tparam T the type
 */
template<typename T>
concept IntegerDecoder =
    requires(T subject, SomeBitSource src, Universe u) {
        { subject.decode(src, u) } -> std::unsigned_integral;
    };

/**
 * \brief Concept for types that provide encoding of integers to a \ref tdc::code::BitSink "BitSink"
 * 
 * In order to satisfy this concept, the class must define a function \c encode , which encodes an integer
 * in a given \ref tdc::code::Universe "Universe" to a bit sink.
 * 
 * \tparam T the type
 */
template<typename T>
concept IntegerEncoder =
    requires(T subject, SomeBitSink sink, uintmax_t x, Universe u) {
        { subject.encode(sink, x, u) };
    };

/**
 * \brief Concept for histogram entries
 * 
 * In order to satisfy this concept, the class must be an input iterator over pairs of characters and their assigned frequencies.
 * 
 * \tparam T the type
 * \tparam Char the character type
 */
template<typename T, typename Char>
concept HistogramEntry =
    std::input_iterator<T> &&
    std::convertible_to<std::iter_value_t<T>, std::pair<Char, size_t>>;

/**
 * \brief Concept for histograms that assign frequencies to characters
 * 
 * In order to satisfy this concept, the class be able to report a size (number of entries), as well as begin and end iterators over \ref tdc::code::HistogramEntry "histogram entries"
 * 
 * \tparam T the type
 * \tparam Char the character type
 */
template<typename T, typename Char>
concept Histogram =
    requires(T const& subject) {
        { subject.size() } -> std::unsigned_integral;
        { subject.begin() } -> std::input_iterator;
        { subject.begin() } -> HistogramEntry<Char>;
        { subject.end() } -> std::input_iterator;
    };

}

#endif
