/**
 * code/range.hpp
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

#ifndef _CODE_RANGE_HPP
#define _CODE_RANGE_HPP

#include <algorithm>
#include <concepts>

namespace code {

/**
 * \brief Represents a range of unsigned integers
 * 
 * Ranges are typically used to setup a \ref code::Universe by initializing an empty range, and then
 * extending it by contained integers while processing an input. \ref code::Universe offers construction
 * from a range, but not vice versa.
 */
class Range {
private:
    uintmax_t min_, max_;

public:
    /**
     * \brief Constructs an empty range
     * 
     */
    inline Range() {
        min_ = UINTMAX_MAX;
        max_ = 0;
    }

    /**
     * \brief Constructs a pre-initialized range
     * 
     * \param min the range's lower bound
     * \param max the range's upper bound
     */
    inline Range(uintmax_t const min, uintmax_t const max) : min_(min), max_(max) {
    }

    Range(Range const&) = default;
    Range(Range&&) = default;
    Range& operator=(Range const&) = default;
    Range& operator=(Range&&) = default;

    /**
     * \brief Extends the range so that it contains the specified value
     * 
     * \tparam T 
     * \param value 
     */
    template<std::unsigned_integral T>
    void contain(T const value) {
        min_ = std::min(min_, uintmax_t(value));
        max_ = std::max(max_, uintmax_t(value));
    }

    /**
     * \brief Reports the range's current lower bound
     * 
     * \return uintmax_t the range's current lower bound
     */
    uintmax_t min() const { return min_; }

    /**
     * \brief Reports the range's current upper bound
     * 
     * \return uintmax_t the range's current upper bound
     */
    uintmax_t max() const { return max_; }
};

}

#endif
