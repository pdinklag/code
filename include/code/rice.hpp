#ifndef _CODE_RICE_HPP
#define _CODE_RICE_HPP

#include "elias_gamma.hpp"

namespace code {

/**
 * \brief Rice encoding and decoding of integers
 * 
 * The rice code is a special variant of the Golomb code.
 * The integer to be encoded is divided by a parameter.
 * The integer quotient is then encoded using gamma code, followed by the binary code of the remainder.
 * The number of bits that the remainder is encoded with is determined by the universe of possible remainders for the divisor.
 * In rice coding, the divisor is a power of two.
 * 
 * Instances of this class satisfy both the \ref tdc::code::IntegerEncoder "IntegerEncoder" and the \ref tdc::code::IntegerDecoder "IntegerDecoder" concepts.
 */
class Rice {
public:
    /**
     * \brief Encodes an integer using rice code with the specified divisor
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param p the exponent of the Golomb divisor \c 2^p
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x, uint8_t p) {
        uintmax_t const q = x >> p;
        EliasGamma::encode(sink, q + 1); // must not pass zero
        Binary::encode(sink, x, p); // Golomb remainder equals the lowest p bits of v
    }

    /**
     * \brief Encodes an integer from the given universe using rice code with the specified divisor
     * 
     * This function actually encodes the value of the integer relative to the universe's minimum.
     * 
     * \tparam Sink the bit sink type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param p the exponent of the Golomb divisor \c 2^p
     * \param u the universe of \c x
     */
    template<BitSink Sink>
    inline static void encode(Sink& sink, uintmax_t x, uint8_t p, Universe u) {
        encode(sink, u.rel(x), p);
    }

    /**
     * \brief Decodes an integer using rice code with the specified divisor
     * 
     * \tparam Source the bit source type
     * \param src the bit sink
     * \param p the exponent of the Golomb divisor \c 2^p
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src, uint8_t p) {
        auto const q = EliasGamma::decode(src) - 1;
        return (q << p) | Binary::decode(src, p);
    }

    /**
     * \brief Decodes an integer from the given universe using rice code with the specified divisor
     * 
     * This function actually decodes the value of the integer relative to the universe's minimum and adds it afterwards.
     * 
     * \tparam Source the bit source type
     * \param src the bit source
     * \param p the exponent of the Golomb divisor \c 2^p
     * \param u the universe of the integer to decode
     * \return the decoded integer
     */
    template<BitSource Source>
    inline static uintmax_t decode(Source& src, uint8_t p, Universe u) {
        return u.abs(decode(src, p));
    }

private:
    uint8_t exponent_;

public:
    /**
     * \brief Constructs a rice coder with a fixed Golomb divisor
     * 
     * \param exponent the base-two exponent of the Golomb divisor ( \c 2^p ) used by this coder 
     */
    inline constexpr Rice(uint8_t exponent) : exponent_(exponent) {
    }

    /**
     * \brief Encodes an integer from the given universe using rice code
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
        encode(sink, x, exponent_, u);
    }

    /**
     * \brief Decodes an integer from the given universe using rice code
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
        return decode(src, exponent_, u);
    }

    /**
     * \brief Reports the base-two exponent of the Golomb divisor ( \c 2^p ) used by this coder
     * 
     * \return the base-two exponent of the Golomb divisor used by this coder 
     */
    uint8_t exponent() const { return exponent_; }
};

}

#endif
