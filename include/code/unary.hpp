#ifndef _CODE_UNARY_HPP
#define _CODE_UNARY_HPP

#include <limits>

#include "concepts.hpp"

namespace code {

/**
 * \brief Unary encoding and decoding of integers
 * 
 * Instances of this class satisfy both the \ref code::IntegerEncoder "IntegerEncoder" and the \ref code::IntegerDecoder "IntegerDecoder" concepts.
 */
class Unary {
private:
    static constexpr size_t UINTMAX_BITS = std::numeric_limits<uintmax_t>::digits;

public:
    /**
     * \brief Encodes an integer using unary code
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x) {
        while(x >= UINTMAX_BITS) {
            sink.write(UINTMAX_MAX, UINTMAX_BITS);
            x -= UINTMAX_BITS;
        }

        sink.write(UINTMAX_MAX, x);
        sink.write(0);
    }

    /**
     * \brief Encodes an integer from the given universe using unary code
     * 
     * This function actually encodes the value of the integer relative to the universe's minimum.
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param u the universe of \c x
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x, Universe u) {
        encode(sink, u.rel(x));
    }

    /**
     * \brief Decodes an integer from the given universe using unary code
     * 
     * \tparam Source the bit source type
     * \param src the bit source
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src) {
        uintmax_t x = 0;
        while(src.read()) {
            ++x;
        }
        return x;
    }

    /**
     * \brief Decodes an integer from the given universe using unary code
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
        return u.abs(decode(src));
    }
};

}

#endif
