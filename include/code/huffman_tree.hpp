/**
 * code/huffman_tree.hpp
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

#ifndef _CODE_HUFFMAN_TREE_HPP
#define _CODE_HUFFMAN_TREE_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <queue>
#include <vector>
#include <unordered_map>
#include <utility>

#include "concepts.hpp"
#include "huffman_code.hpp"
#include "elias_delta.hpp"

namespace code {

/**
 * \brief A Huffman tree
 * 
 * This class satisfies the \ref tdc::code::HuffmanCodeProvider "HuffmanCodeProvider" concept
 * 
 * \tparam Char the character type
 */
template<std::integral Char>
class HuffmanTree {
private:
    using UChar = std::make_unsigned_t<Char>;

public:
    /**
     * \brief A node in a Huffman tree
     * 
     * This class satisfies the \ref tdc::code::HuffmanTreeNavigator "HuffmanTreeNavigator" concept
     * 
     * \tparam Char the character type
     */
    class Node {
    private:
        Node* parent_;
        Node* left_;
        Node* right_;

        size_t freq_;
        Char c_;

        bool bit() const { return parent_->right_ == this; }

    public:
        /**
         * \brief Constructs a completely uninitialized node
         * 
         */
        Node() {
        }

        Node(Node const&) = delete;
        Node& operator=(Node const&) = delete;

        Node(Node&&) = default;
        Node& operator=(Node&&) = default;

        /**
         * \brief Constructs a leaf of a Huffman tree
         * 
         * \param c the character represented by the leaf
         * \param freq the frequency of the character
         */
        Node(Char c, size_t freq) : parent_(nullptr) {
            set_leaf(c, freq);
        }

        /**
         * \brief Constructs an inner node of a Huffman tree
         * 
         * The frequency will be set to the sum of the frequencies of the given left and right child.
         * This also updates the parent field of both children.
         * 
         * \param left the left child
         * \param right the right child
         */
        Node(Node& left, Node& right) : parent_(nullptr) {
            set_inner(left, right);
        }

        /**
         * \brief Makes the node a leaf
         * 
         * The left and right child pointers will be reset to null.
         * 
         * \param c the character represented by the leaf
         * \param freq the frequency of the character
         */
        void set_leaf(Char c, size_t freq) {
            c_ = c;
            freq_ = freq;

            left_ = nullptr;
            right_ = nullptr;
        }

        /**
         * \brief Makes this node an inner node
         * 
         * The frequency will be initialized as the sum as the childrens' frequencies.
         * This also updates the parent field of both children.
         * 
         * \param left the left child
         * \param right the right child
         */
        void set_inner(Node& left, Node& right) {
            left_ = &left;
            right_ = &right;
            freq_ = left.freq_ + right.freq_;

            left.parent_ = this;
            right.parent_ = this;
        }

        /**
         * \brief Tests whether this node is a leaf
         * 
         * \return true if this node is a leaf
         * \return false if this is an inner node
         */
        bool is_leaf() const { return left_ == nullptr && right_ == nullptr; }

        /**
         * \brief Gets the node's left child
         * 
         * The result is only valid for nodes where \ref is_leaf reports \c false .
         * 
         * \return the inner node's left child
         */
        Node const& left_child() const { return *left_; }

        /**
         * \brief Gets the node's right child
         * 
         * The result is only valid for nodes where \ref is_leaf reports \c false .
         * 
         * \return the inner node's right child
         */
        Node const& right_child() const { return *right_; }

        /**
         * \brief Gets the character represented by this leaf
         * 
         * The result is only valid for nodes where \ref is_leaf reports \c true .
         * 
         * \return the character represented by this leaf
         */
        Char operator*() const { return c_; }

        /**
         * \brief Provides write access to the represented character
         * 
         * \return a reference to the represented character
         */
        Char& operator*() { return c_; }

        /**
         * \brief Gets the frequency of the represented character
         * 
         * The result is only valid for nodes where \ref is_leaf reports \c true .
         * 
         * \return the frequency of the represented character
         */
        size_t freq() const { return freq_; }

        /**
         * \brief Constructs the Huffman code for this node
         * 
         * This is done by navigating up the tree until the root is reached, and thus takes time proportional to the code length.
         * Note that the codeword will be in LSBF order as documented in \ref HuffmanCode
         * 
         * \return the Huffman code for this node
         */
        HuffmanCode code() const {
            HuffmanCode code { 0, 0 };
            for(auto const* v = this; v->parent_ != nullptr; v = v->parent_) {
                code.word = (code.word << 1) | v->bit();
                ++code.length;
            }
            return code;
        }
    };

private:
    std::vector<Node> nodes_;

    Node const* root_;
    std::unordered_map<Char, Node const*> leaves_;

public:
    HuffmanTree() : root_(nullptr) {
    }

    HuffmanTree(HuffmanTree&&) = default;
    HuffmanTree& operator=(HuffmanTree&&) = default;

    HuffmanTree(HuffmanTree const&) = delete;
    HuffmanTree& operator=(HuffmanTree const&) = delete;

    /**
     * \brief Constructs the Huffman tree for the given input
     * 
     * This is done using a folklore algorithm.
     * The input is scanned once and a histogram is built mapping input characters to their observed frequencies.
     * These form the initial content of a priority queue used to build the tree bottom-up using the two least frequent nodes in each step.
     * 
     * Consider a node with two children.
     * The following (arbitrarily chosen) properties are maintained to ensure deterministic results:
     * * The frequency of the left child is greater than or equal to that of the right child.
     * * If both children have the same frequency and one child is a leaf and the other an inner node, the leaf will become the left child.
     * * When both children are leaves of the same frequency, they will be ordered such that the right child represents the smaller character.
     * 
     * \tparam It the input iterator
     * \param it the input
     * \param end the end of the input
     */
    template<std::input_iterator It>
    requires std::is_convertible_v<typename std::iterator_traits<It>::value_type, Char>
    HuffmanTree(It it, It const end) {
        if(it == end) {
            // input is empty
            root_ = nullptr;
            return;
        }

        // define max-priority queue of tree nodes
        struct FreqCompare {
            bool operator()(Node const* a, Node const* b) const {
                return (a->freq() > b->freq()) // consider nodes with lower frequency first
                    || (a->freq() == b->freq() && a->is_leaf() && !b->is_leaf()) // if two nodes have the same frequency, consider leaves first
                    || (a->freq() == b->freq() && a->is_leaf() && b->is_leaf() && **a > **b) // if two leaves have the same frequency, order ascending
                    ;
            }
        };
        std::priority_queue<Node*, std::vector<Node*>, FreqCompare> queue;

        {
            // compute histogram
            Char c;
            std::unordered_map<Char, size_t> histogram;
            while(it != end) {
                c = *it++;

                auto e = histogram.find(c);
                if(e != histogram.end()) {
                    ++e->second;
                } else {
                    histogram.emplace(c, 1);
                }
            }

            // if the alphabet has exactly one character, we introduce a new character of zero frequency so we actually get a Huffman tree
            if(histogram.size() == 1) {
                histogram.emplace(~c, 0); // nb: since c is the only character, ~c is a new one
            }

            // construct and enqueue leaves
            nodes_.reserve(2 * histogram.size());
            leaves_.reserve(histogram.size());

            for(auto e : histogram) {
                nodes_.emplace_back(e.first, e.second);
                auto* pnode = &nodes_.back();

                leaves_.emplace(e.first, pnode);
                queue.emplace(pnode);
            }
        }

        // build Huffman tree
        auto alphabet_size = queue.size();
        while(alphabet_size-- > 1) {
            // get the next two nodes from the priority queue
            auto* r = queue.top(); queue.pop();
            auto* l = queue.top(); queue.pop();
            assert(r->freq() <= l->freq());

            // create a new node as parent of l and r
            nodes_.emplace_back(*l, *r);
            queue.push(&nodes_.back());
        }

        root_ = queue.top();
        assert(queue.size() == 1);
    }

private:
    template<BitSource Source>
    void decode_topology(Source& src, std::vector<bool>& topology, size_t& alphabet_size) {
        bool const b = src.read();
        topology.push_back(b);
        if(b) {
            // leaf
            ++alphabet_size;
        } else {
            // inner node
            decode_topology(src, topology, alphabet_size); // left subtree
            decode_topology(src, topology, alphabet_size); // right subtree
        }
    }

    using BitVectorIterator = std::vector<bool>::const_iterator;

    template<BitSource Source>
    Node* decode_node(Source& src, BitVectorIterator& bits, Universe const& u) {
        bool const b = *bits++;
        if(b) {
            // decode character and construct leaf
            auto const c = (Char)Binary::decode(src, u);
            nodes_.emplace_back(c, 0); // no weight
            
            auto* v = &nodes_.back();
            leaves_.emplace(c, v);
            return v;
        } else {
            // decode children and construct inner node
            auto* l = decode_node(src, bits, u);
            auto* r = decode_node(src, bits, u);
            nodes_.emplace_back(*l, *r);
            return &nodes_.back();
        }
    }

public:
    /**
     * \brief Decodes a Huffman tree from the given bit source
     * 
     * The tree must have been encoded using \ref encode in order for this function to be able to decode it.
     * 
     * \tparam Source the bit source type
     * \param src the bit source
     */
    template<BitSource Source>
    HuffmanTree(Source& src) {
        // first, decode the topology so we can properly allocate our nodes array
        std::vector<bool> topology;
        size_t alphabet_size = 0;
        
        decode_topology(src, topology, alphabet_size);
        if(topology.size() > 1) {
            // allocate
            nodes_.reserve(topology.size());
            leaves_.reserve(alphabet_size);

            // second, decode the universe of characters
            auto const min = EliasDelta::decode(src, Universe::umax());
            auto const max = EliasDelta::decode(src, Universe::at_least(min));
            Universe u(min, max);
            
            // build the tree and decode characters
            auto bits = topology.cbegin();
            root_ = decode_node(src, bits, u);
        } else {
            // we have decoded an empty tree
            root_ = nullptr;
        }
    }

    /**
     * \brief Computes the Huffman code for the given character
     * 
     * Note that this is done bottom-up starting from the corresponding leaf, and thus takes time proportional to the code length.
     * 
     * \param c the character
     * \return the Huffman code for the given character
     */
    HuffmanCode operator[](Char c) const {
        auto it = leaves_.find(c);
        if(it != leaves_.end()) {
            return it->second->code();
        } else {
            return { 0, 0 };
        }
    }

private:
    struct HashCodeTable {
        std::unordered_map<uintmax_t, HuffmanCode> data;
        HuffmanCode operator[](const uintmax_t c) const { return data.at(c); }
    };

public:
    /**
     * \brief Computes a Huffman table
     * 
     * This involves precomputing the Huffman codes for all input characters and constructing a mapping from character to code.
     * The returned object satisfies the \ref tdc::code::HuffmanCodeProvider "HuffmanCodeProvider" concept.
     * 
     * \return a mapping from all input characters to their Huffman codes
     */
    auto table() const {
        if constexpr(std::numeric_limits<UChar>::max() <= UINT16_MAX) {
            // for 8-bit and 16-bit characters, we can afford building an array
            std::array<HuffmanCode, std::numeric_limits<UChar>::max() + 1> table;
            for(auto e : leaves_) {
                table[(UChar)e.first] = e.second->code();
            }
            return table;
        } else {
            // for larger alphabets, we use hashing instead
            HashCodeTable table;
            table.data.reserve(leaves_.size());
            for(auto e : leaves_) {
                table.data.emplace(e.first, e.second->code());
            }
            return table;
        }
    }

    /**
     * \brief Retrieves the root node of the Huffman tree
     * 
     * \return the root node of the Huffman tree
     */
    Node const& root() const { return *root_; }

    /**
     * \brief Reports the size of the huffman tree, i.e., the number of nodes
     * 
     * \return the size of the Huffman tree
     */
    size_t size() const { return nodes_.size(); }

private:
    template<BitSink Sink>
    void encode_tree(Node const& v, Sink& sink, std::vector<UChar>& uchars_ltr, Range& range) const {
        // write a bit indicating whether this node is a leaf or an inner node
        // in the latter case, it is guaranteed to have two children, so a single bit suffices
        sink.write(v.is_leaf());
        if(v.is_leaf()) {
            // register represented character
            UChar const c = (UChar)*v;
            uchars_ltr.emplace_back(c);
            range.contain(c);
        } else {
            // traverse children in left-to-right order
            encode_tree(v.left_child(), sink, uchars_ltr, range);
            encode_tree(v.right_child(), sink, uchars_ltr, range);
        }
    }

public:
    /**
     * \brief Encodes the Huffman tree to the given bit sink
     * 
     * The tree topology is represented using \em 2n-1 bits, where \em n is the number of nodes in the tree.
     * The representation is encoded in a pre-order traversal.
     * Inner nodes are encoded as a 0-bit, while leaves are encoded as a 1-bit.
     * 
     * The encoded topology is followed by the following delta-encoded values:
     * 1. The minimum character represented in the tree
     * 2. The characters themselves; the number of values equals the number of 1-bits in the topology, and the encoded values are relative to the minimum character
     * 
     * \tparam Sink 
     * \param sink 
     */
    template<BitSink Sink>
    void encode(Sink& sink) const {
        const auto alphabet_size = leaves_.size();

        std::vector<UChar> uchars_ltr;
        uchars_ltr.reserve(alphabet_size);

        Range range;
        if(root_) {
            // encode tree and gather characters left to right
            encode_tree(root(), sink, uchars_ltr, range);

            // encode universe of characters using delta codes
            Universe u(range);
            EliasDelta::encode(sink, u.min(), Universe::umax());
            EliasDelta::encode(sink, u.max(), Universe::at_least(u.min()));

            // encode characters as they occur in the tree in left-to-right order
            for(auto c : uchars_ltr) {
                Binary::encode(sink, c, u);
            }
        } else {
            // the tree is empty
            // encode a 1-bit that indicates that the root is the only leaf, the decoder will handle this
            sink.write(1);
        }
    }
};

}

#endif
