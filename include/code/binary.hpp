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
