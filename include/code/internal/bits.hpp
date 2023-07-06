#ifndef _CODE_INTERNAL_BITS_HPP
#define _CODE_INTERNAL_BITS_HPP

#include <cstddef>
#include <cstdint>

namespace code::internal {

/**
 * \brief Constructs an unsigned integer where the `i`-th bit is set and all other bits are clear
 * 
 * \param i the index of the set bit; for values of 64 or above, the result is undefined
 * \return an unsigned integer where the `i`-th bit is set and all other bits are clear
 */
constexpr inline uintmax_t set_bit(size_t const i) {
    return uintmax_t(1) << i;
}

}

#endif
