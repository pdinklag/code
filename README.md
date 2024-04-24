# Integer Codes for C++

This header-only C++20 library provides utilities for the bitwise encoding and decoding of (unsigned) integers in various codes. Universal codes include binary, unary, Elias-&gamma;, Elias-&delta;, Rice and variable byte (vbyte). Additionally, the library implements Huffman coding.

All encoders and decoders are targeted at bitwise I/O provided by the [iopp](https://github.com/pdinklag/iopp) library. However, iopp is not a hard dependency &ndash; rather, this library defines C++ concepts that may be satisfied by any bitwise I/O library that you may wish to adapt.

### Requirements

This library is written in C++20, a corresponding compiler is required that fully supports concepts. Tests have been done only with GCC 11. Apart from that, the library as no external dependencies. For running [unit tests](#unit-tests), CMake is required.

### License

```
MIT License

Copyright (c) Patrick Dinklage

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### Unit Tests

The library comes with unit tests powered by [doctest](https://github.com/doctest/doctest).

Using CMake, you can build and run the benchmark using the following chain of commands in the repository root:

```sh
mkdir build; cd build
cmake ..
make
make test
```

The unit test use the [iopp](https://github.com/pdinklag/iopp) library provided as a git submodule that is initialized automatically as needed.

## Usage

The library is header only, so all you need to do is make sure it's in your include path.

In case you use CMake, you can embed this repository into yours (e.g., as a git submodule) and add it like so:

```cmake
add_subdirectory(path/to/code)
```

You can then link against the `code` interface library, which will automatically add the include directory to your target.

### Universal Codes

Universal codes are used to encode integers without taking into account any context (in the sense of previously encoded integers). The following universal codes are available.

| Code                                                         | Include                           | Type Name          |
| ------------------------------------------------------------ | --------------------------------- | ------------------ |
| Binary                                                       | `#include <code/binary.hpp>`      | `code::Binary`     |
| Unary                                                        | `#include <code/unary.hpp>`       | `code::Unary`      |
| [Elias-&gamma;](https://en.wikipedia.org/wiki/Elias_gamma_coding) | `#include <code/elias_gamma.hpp>` | `code::EliasGamma` |
| [Elias-&delta;](https://en.wikipedia.org/wiki/Elias_delta_coding) | `#include <code/elias_delta.hpp>` | `code::EliasDelta` |
| [Rice](https://en.wikipedia.org/wiki/Golomb_coding#Rice_coding) | `#include <code/rice.hpp>`        | `code::Rice`       |
| [Variable Byte](https://nlp.stanford.edu/IR-book/html/htmledition/variable-byte-codes-1.html) | `#include <code/vbyte.hpp>`       | `code::Vbyte`      |

The intended usage is to call the static `encode` and `decode` functions.

#### Universes

To save bits for encoding an integer, the `encode` and `decode` functions accept an argument that defines the *universe* (or interval) from which the integer is drawn. For example, an integer *x* drawn from the universe *[a, b]* can be binary encoded using *&lceil;log<sub>2</sub> b-a&rceil;* bits by encoding the difference *x-a* rather than *x*. For decoding, the decoder must be aware of the universe.

Universes are defined using the `code::Universe` struct.

#### Example

The following example uses [iopp](https://github.com/pdinklag/iopp) to encode some integers into a string and decode them again using a string stream.

```cpp
#include <iopp/bitwise_io.hpp>
#include <code.hpp>

// ...

// define a universe of numbers between 10 and 20 (each inclusive)
code::Universe u(10, 20);

// encode
std::string buffer;
{
    std::ostringstream out;
    auto sink = iopp::bitwise_output_to(std::back_inserter(buffer));

    code::Binary::encode(sink, 17, 5);     // encode binary with an explicit number of 5 bits
    code::Binary::encode(sink, 17, u);     // encode binary using our universe (encodes 17-10 = 7 with 4 bits)
    code::Unary::encode(sink, 11);         // encode unary (with 12 bits)
    code::Unary::encode(sink, 11, u);      // encode unary using our universe (encodes 11-10 = 1 with 2 bits)
    code::EliasGamma::encode(sink, 12);    // encode gamma (with 7 bits)
    code::EliasGamma::encode(sink, 12, u); // encode gamma using universe (encodes 12-10 = 2 with 3 bits)
    code::Rice::encode(sink, 13, 3);       // encode Rice using a Golomb exponent of 3
    code::Rice::encode(sink, 13, 3, u);    // encode Rice using a Golomb exponent of 3 and our universe (encodes 13-10 = 3)
    code::Vbyte::encode(sink, 18, 8);      // encode vbyte using a byte size of 8 bits
    code::Vbyte::encode(sink, 18, 8, u);   // encode vbyte using a byte size of 8 bits and our universe (encoded 18-10 = 8)
}

// decode
{
    auto in = std::istringstream(buffer);
    auto src = iopp::bitwise_input_from(in);

    auto const binary1 = code::Binary::decode(src, 5);    // decode binary with an explicit number of 5 bits
    auto const binary2 = code::Binary::decode(src, u);    // decode binary using our universe
    auto const unary1 = code::Unary::decode(src);         // decode unary
    auto const unary2 = code::Unary::decode(src, u);      // decode unary using our universe
    auto const gamma1 = code::EliasGamma::decode(src);    // decode gamma
    auto const gamma2 = code::EliasGamma::decode(src, u); // decode gamma using our universe
    auto const rice1 = code::Rice::decode(src, 3);        // decode Rice using a Golomb exponent of 3
    auto const rice2 = code::Rice::decode(src, 3, u);     // decode Rice using a Golomb exponent of 3 and our universe
    auto const vbyte1 = code::Vbyte::decode(src, 8);      // decode vbyte using a byte size of 8 bits
    auto const vbyte2 = code::Vbyte::decode(src, 8, u);   // decode vbyte using a byte size of 8 bits and our universe
}
```

### Huffman Codes

Huffman codes exploit the frequency distribution of integers to be encoded. This way, more frequent integers are assigned shorter codes than less frequent integers such that zeroth-entropy compression is achieved. Huffman codes are computed in advance, that is, the integers to be encoded (or at least their frequency histogram) must be known beforehand. In order to decode Huffman codes, the decoder needs to know the code mapping, e.g., in the form of a Huffman tree.

#### Huffman Tree

The class `code::HuffmanTree` represents a Huffman tree for the integers to be encoded or decoded. Every node has either no children or two children and edges are labeled by bits, leaves in the tree represent one of the &sigma; distinct integers that are represented. For each integer, the bits on the edges on the root-to-leaf path to the corresponding leaf spell the Huffman code; the time to compute it is linear in the length of the longest codeword.

Huffman trees are encoded in two sections. First, if there are *m* nodes, the tree topology is encoded depth-first using *m* bits, where each bit indicates whether a node is an inner node (with exactly two children) or a leaf. This is followed by a delta encoding of the characters represented by the leaves in left-to-right order.

#### Huffman Table

For encoding, we can convert the Huffman tree into a Huffman table that maps each represented character to the corresponding Huffman code using a hash table for faster access. This is done using the `table()` function (currently returning a `std::unordered_map`, which isn't particularly great, but sufficient for most cases).

#### Example

The following example shows a roundtrip encoding a decoding a string using Huffman codes and [iopp](https://github.com/pdinklag/iopp).

```cpp
#include <iopp/bitwise_io.hpp>
#include <code.hpp>

// ...

std::string const input_str = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus aliquet in turpis vitae mattis.";

// encode
std::string buffer;
{
    auto sink = iopp::bitwise_output_to(std::back_inserter(buffer));

    // construct the Huffman tree for the input
    code::HuffmanTree<char> huffman_tree(input_str.begin(), input_str.end());

    // encode the Huffman tree
    huffman_tree.encode(sink);

    // compute the Huffman table for faster code access
    auto const huffman_table = huffman_tree.table();

    // encode the input
    for(char const c : input_str) {
        code::Huffman::encode(sink, c, huffman_table);
    }
}

// decode
{
    auto src = iopp::bitwise_input_from(buffer.begin(), buffer.end());

    // decode the Huffman tree and get the root node
    code::HuffmanTree<char> huffman_tree(src);
    auto const& huffman_tree_root = huffman_tree.root();

    // decode the input
    std::string decoded_str;
    while(src) {
        decoded_str.push_back(char(code::Huffman::decode(src, huffman_tree_root)));
    }
}
```