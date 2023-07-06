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
}

#endif
