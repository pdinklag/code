/**
 * code/huffman.hpp
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

#ifndef _CODE_HUFFMAN_HPP
#define _CODE_HUFFMAN_HPP

#include "concepts.hpp"
#include "huffman_code.hpp"
#include "huffman_tree.hpp"

namespace code {

/**
 * \brief Concept for types that provide Huffman codes for input characters
 * 
 * Specifically, this concept requires that the type override the \c [] operator for any integral character type,
 * which must return a \ref tdc::code::HuffmanCode "HuffmanCode" .
 * 
 * \tparam T the type
 */
template<typename T>
concept HuffmanCodeProvider =
    requires(T const& subject, uintmax_t c) {
        { subject[c] } -> std::convertible_to<HuffmanCode>;
    };

/**
 * \brief Concept for type that simulate top-down navigation in a Huffman tree
 * 
 * Types that satisfy this concept can be used to decode a \ref tdc::code::HuffmanCode "HuffmanCode" by simulating a top-down navigation.
 * 
 * \tparam T the type
 */
template<typename T>
concept HuffmanTreeNavigator =
    requires(T const& subject) {
        { subject.is_leaf() } -> std::convertible_to<bool>;
        { subject.left_child() } -> std::same_as<T const&>;
        { subject.right_child() } -> std::same_as<T const&>;
        { *subject } -> std::integral;
    };

/**
 * \brief Huffman encoding and decoding of integers
 * 
 */
class Huffman {
public:
    /**
     * \brief Encodes an integer using the Huffman code given by the specified Huffman code provider
     * 
     * The behaviour of this function is undefined if the integer is not known by the code provider.
     * 
     * \tparam Sink the bit sink type
     * \tparam Table the Huffman code provider type
     * \param sink the bit sink
     * \param x the integer to encode
     * \param table the Huffman code provider
     */
    template<BitSink Sink, HuffmanCodeProvider Table>
    static void encode(Sink& sink, uintmax_t x, Table const& table) {
        auto code = table[x];
        while(code.length--) {
            sink.write(code.word & 1);
            code.word >>= 1;
        }
    }

    /**
     * \brief Decodes a Huffman code and reports the corresponding integer
     * 
     * While reading bits from the source, the tree is navigated accordingly.
     * Once a leaf has been reached, the corresponding integer is reported.
     * 
     * \tparam Source the bit source type
     * \tparam TreeNavigator the Huffman tree navigator type
     * \param src the bit source
     * \param root the root of a Huffman tree
     * \return the decoded integer
     */
    template<BitSource Source, HuffmanTreeNavigator TreeNavigator>
    static uintmax_t decode(Source& src, TreeNavigator const& root) {
        auto const* v = &root;
        while(!v->is_leaf()) {
            v = src.read() ? &v->right_child() : &v->left_child();
        }
        return (uintmax_t)**v;
    }

    /**
     * \brief Encodes integers using Huffman codes
     * 
     * This class satisfies the \ref tdc::code::IntegerEncoder "IntegerEncoder" concept.
     * 
     * \tparam Table the Huffman code provider type
     */
    template<HuffmanCodeProvider Table>
    class Encoder {
    private:
        Table const* table_;

    public:
        /**
         * \brief Constructs a Huffman encoder
         * 
         * \param table the Huffman code provider
         */
        Encoder(Table const& table) : table_(&table) { }

        /**
         * \brief Encodes an integer using the Huffman code given by code provider
         * 
         * The behaviour of this function is undefined if the integer is not known by the code provider.
         * 
         * \tparam Sink the bit sink type
         * \tparam Table the Huffman code provider type
         * \param sink the bit sink
         * \param x the integer to encode
         * \param u the universe of \c x (ignored)
         */
        template<BitSink Sink> void encode(Sink& sink, uintmax_t x, Universe u = Universe::umax()) { Huffman::encode(sink, x, *table_); }
    };

    /**
     * \brief Decodes integers using Huffman codes
     * 
     * This class satisfies the \ref tdc::code::IntegerDecoder "IntegerDecoder" concept.
     * 
     * \tparam TreeNavigator the Huffman tree navigator type
     */
    template<HuffmanTreeNavigator TreeNavigator>
    class Decoder {
    private:
        TreeNavigator const* nav_;

    public:
        /**
         * \brief Constructs a Huffman decoder
         * 
         * \param nav the root of the Huffman tree used for decoding
         */
        Decoder(TreeNavigator const& nav) : nav_(&nav) { }

        /**
         * \brief Decodes a Huffman code and reports the corresponding integer
         * 
         * While reading bits from the source, the tree is navigated accordingly.
         * Once a leaf has been reached, the corresponding integer is reported.
         * 
         * \tparam Source the bit source type
         * \param src the bit source
         * \param u the universe of the integer to decode (ignored)
         * \return the decoded integer
         */
        template<BitSource Source> uintmax_t decode(Source& src, Universe u = Universe::umax()) { return Huffman::decode(src, *nav_); }
    };
};

}

#endif
