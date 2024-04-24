/**
 * code/counter.hpp
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

#ifndef _CODE_COUNTER_HPP
#define _CODE_COUNTER_HPP

#include <unordered_map>

#include "concepts.hpp"

namespace code {

/**
 * \brief Utility for counting items
 * 
 * Maintains an unordered map that assigns items to their count.
 * The count can be incremented using the \ref count function.
 * 
 * Note that a counter distinguishes between considered and unconsidered items.
 * Items that have never been considered implicitly have a count of zero.
 * However, using \ref set, an item can retrieve an explicit count of zero.
 * 
 * This class satisifies the \ref code::Histogram "Histogram" concept.
 * 
 * \tparam Item the counted item type
 */
template<typename Item>
class Counter {
private:
    std::unordered_map<Item, size_t> count_;

public:
    /**
     * \brief Constructs a counter where the count of all items is zero
     */
    Counter() {
    }

    /**
     * \brief Constructs a counter and initializes it with the number of occurrences of items in the given input
     * 
     * \tparam It the input iterator type
     * \param begin the input iterator
     * \param end the end of inpute iterator
     */
    template<std::input_iterator It>
    requires std::is_convertible_v<typename std::iterator_traits<It>::value_type, Item>
    Counter(It begin, It const end) {
        while(begin != end) count(*begin++);
    }

    /**
     * \brief Directly sets the count of the given item
     * 
     * The count may be zero, which can be used to explicitly distinguish items of count zero from items that have never been considered.
     * 
     * \param c the item in question
     * \param count the count to set for the item
     */
    void set(Item const c, size_t count) {
        count_[c] = count;
    }

    /**
     * \brief Increments the count of the given item
     * 
     * \param c the item to count
     * \param times the number of times to count the item
     */
    void count(Item const c, size_t times = 1) {
        auto e = count_.find(c);
        if(e != count_.end()) {
            e->second += times;
        } else {
            count_.emplace(c, times);
        }
    }

    /**
     * \brief Counts the given item once
     * 
     * \param c the item to count
     * \return self reference
     */
    Counter& operator+=(Item const c) {
        count(c);
        return *this;
    }

    /**
     * \brief Tests whether the given item is considered by the counter
     * 
     * Note that the count of an item may be zero in case \ref set has been called.
     * In other words, containment does not imply a non-zero count.
     * 
     * \param c the item in question
     * \return true iff the item has a count
     */
    bool contains(Item const c) const { return count_.contains(c); }

    /**
     * \brief Retrieves the count for the given item
     * 
     * Note that the count of a contained may be zero in case \ref set has been called.
     * Use \ref contains to verify whether the item has ever been considered.
     * 
     * \param c the item in question
     * \return the count for the item, or zero if the item is not contained
     */
    size_t operator[](Item const c) const {
        auto e = count_.find(c);
        return e != count_.end() ? e->second : 0;
    }
    
    /**
     * \brief Provides a read-only iterator over the maintained map of counters
     */
    auto begin() const { return count_.begin(); }

    /**
     * \brief Provides the end iterator of the maintained map of counters
     */
    auto end() const { return count_.end(); }

    /**
     * \brief Reports the number of distinct items that have a acount
     * 
     * \return the number of distinct items that have a count
     */
    size_t size() const { return count_.size(); }

    /**
     * \brief Provides direct access to the maintained map of counters
     */
    auto& map() { return count_; }

    /**
     * \brief Provides direct read-only access to the maintained map of counters
     */
    auto const& map() const { return count_; }
};

}

#endif
