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
