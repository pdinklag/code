/**
 * code/universe.hpp
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

#ifndef _CODE_UNIVERSE_HPP
#define _CODE_UNIVERSE_HPP

#include <algorithm>
#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>
#include <utility>

#include "range.hpp"

namespace code {

/**
 * \brief Represents a universe of unsigned integers
 * 
 */
class Universe {
private:
    uintmax_t min_, max_, entropy_;

    inline static constexpr uintmax_t wc_entropy(uintmax_t min, uintmax_t max) {
        return std::max(uintmax_t(1), uintmax_t(std::bit_width(max - min)));
    }

    inline constexpr Universe(uintmax_t min, uintmax_t max, uintmax_t entropy) : min_(min), max_(max), entropy_(entropy) {
    }

public:
    /**
     * \brief Returns the binary universe consisting only of 0 and 1
     * 
     * \return the binary universe
     */
    inline static constexpr Universe binary() { return Universe(0, 1); }

    /**
     * \brief Returns the universe of all unsigned integers that can be represented on the system
     * 
     * \return the universe of all unsigned integers that can be represented on the system
     */
    inline static constexpr Universe umax() { return Universe(0, UINTMAX_MAX); }


    /**
     * \brief Returns the universe of all unsigned integers that are at least as large as the stated minimum
     * 
     * \param min the minimum value of the universe
     * \return the universe of all unsigned integers that are at least as large as min
     */
    inline static constexpr Universe at_least(uintmax_t min) { return Universe(min, UINTMAX_MAX); }

    /**
     * \brief Returns the universe of all integers that can be represented by the given type
     * 
     * Examples:
     * \code{.cpp}
     * Universe::of<uint8_t>(); // universe of all unsigned 8-bit integers
     * Universe::of<uint16_t>(); // universe of all unsigned 16-bit integers
     * Universe::of<uint32_t>(); // universe of all unsigned 32-bit integers
     * Universe::of<uint64_t>(); // universe of all unsigned 64-bit integers
     * \endcode
     * 
     * \tparam Int the integer type
     * \return the universe of all integers that can be represented by the given type
     */
    template<std::unsigned_integral Int>
    inline static constexpr Universe of() { return Universe(Int(0), std::numeric_limits<Int>::max(), std::numeric_limits<Int>::digits); }

    /**
     * \brief Returns the universe of all integers that can be represented using the given number of bits
     * 
     * \param entropy the worst-case entropy of the universe
     * \return the universe of all integers that can be represented using the given number of bits
     */
    inline static constexpr Universe with_entropy(uintmax_t entropy) { return Universe(0, UINTMAX_MAX >> (std::numeric_limits<uintmax_t>::digits - entropy), entropy); }

    /**
     * \brief Returns the universe specified by a minimum value and the delta between maximum and minimum
     * 
     * Specifically, the universe's maximum value will equal \c min+delta .
     * 
     * \param min the minimum integer of the universe
     * \param delta the difference between the maximum and the minimum integers of the universe
     * \return the corresponding universe
     */
    inline static constexpr Universe with_delta(uintmax_t min, uintmax_t delta) { return Universe(min, min + delta); }

    /**
     * \brief Construct an empty universe
     * 
     * The universe's minimum is initialized as \c UINTMAX_MAX and the minimum as zero.
     */
    inline constexpr Universe() : Universe(UINTMAX_MAX, 0, 0) {
    }

    constexpr Universe(Universe const&) = default;
    constexpr Universe(Universe&&) = default;
    constexpr Universe& operator=(Universe const&) = default;
    constexpr Universe& operator=(Universe&&) = default;

    /**
     * \brief Defines a universe for the specified unsigned integer range
     * 
     * Note that it is not verified that `min <= max`.
     * 
     * \param min the minimum integer of the universe
     * \param max the maximum integer of the universe
     */
    inline constexpr Universe(uintmax_t const min, uintmax_t const max) : Universe(min, max, wc_entropy(min, max)) {
    }

    /**
     * \brief Defines a universe for the specified unsigned integer range
     * 
     * Note that it is not verified that `min <= max`.
     * 
     * \param range the range
     */
    inline Universe(Range const& range) : Universe(range.min(), range.max()) {
    }

    /**
     * \brief Defines a universe for the specified unsigned integer range
     * 
     * The range is defined by the given pair, where the first item is interpreted as the minimum and the second as the maximum.
     * Note that it is not verified that `min <= max`.
     * 
     * \tparam Pair the pair type, which must support structured binding to two variables
     * \param minmax the pair defining the integer range
     */
    template<typename Pair>
    inline constexpr Universe(Pair minmax) {
        auto [min, max] = minmax;
        min_ = min;
        max_ = max;
        entropy_ = wc_entropy(min, max);
    }

    /**
     * \brief Defines a universe for the unsigned integer range from zero to the given number
     * 
     * \param max the maximum integer of the universe
     */
    inline constexpr Universe(uintmax_t const max) : Universe(0, max) {
    }

    bool operator==(Universe const& other) const = default;
    bool operator!=(Universe const& other) const = default;

    /**
     * \brief Computes the absolute value of an integer relative to the universe's minimum
     * 
     * This is equivalent to adding the universe's minimum to the input integer.
     * 
     * \param rel the integer relative to the universe's minimum
     * \return the absolute value of the integer
     */
    inline constexpr uintmax_t abs(uintmax_t const rel) const { return min_ + rel; }

    /**
     * \brief Computes the relative value of an integer to the universe's minimum.
     * 
     * This is equivalent to subtracting the universe's minimum from the input integer.
     * There is no verification as to whether the input integer is actually contained in the universe.
     * 
     * \param abs the integer to transform
     * \return the relative value of the integer to the universe's minimum
     */
    inline constexpr uintmax_t rel(uintmax_t const abs) const { return abs - min_; }

    /**
     * \brief Reports the minimum integer contained in the universe
     * 
     * \return the minimum integer contained in the universe
     */
    inline constexpr uintmax_t min() const { return min_; }

    /**
     * \brief Reports the maximum integer contained in the universe
     * 
     * \return the maximum integer contained in the universe
     */
    inline constexpr uintmax_t max() const { return max_; }

    /**
     * \brief Reports the difference between the maximum and minimum integers contained in the universe
     * 
     * This is equal to the number of integers contained in the universe, minus one.
     * 
     * \return the difference between the maximum and minimum integers contained in the universe
     */
    inline constexpr uintmax_t delta() const { return max_ - min_; }

    /**
     * \brief Reports the worst-case entropy for integers from this universe in bits
     * 
     * This is equal to the number of bits requires to store an integer from the universe (relative to its minimum integer).
     * 
     * \return the worst-case entropy for integers from this universe in bits
     */
    inline constexpr uintmax_t entropy() const { return entropy_; }
};

}

#endif
