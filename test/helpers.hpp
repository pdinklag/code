#include <cstddef>
#include <cstdint>

namespace code::test {

struct SimpleUint64BitSink {
    uint64_t value;
    size_t p;

    inline SimpleUint64BitSink() : value(0), p(0) {
    }

    inline void write(bool b) { 
        value |= (uint64_t)b << p;
        ++p;
    }

    inline void write(uint64_t bits, size_t num) {
        uint64_t const mask = (num >= 64 ? UINT64_MAX : ~(UINT64_MAX << num));
        value |= (bits & mask) << p;
        p += num;
    }

    inline void flush() {}
    inline size_t num_bits_written() const { return 0; }
};

struct SimpleUint64BitSource {
    uint64_t value;

    inline SimpleUint64BitSource(uint64_t v) : value(v) {
    }

    inline bool read() {
        bool const b = (bool)(value & 1ULL);
        value >>= 1; // FIXME: BITS ARE READ IN WRONG ORDER!?
        return b;
    }

    inline uint64_t read(size_t num) {
        uint64_t const mask = (num >= 64 ? UINT64_MAX : ~(UINT64_MAX << num));
        uint64_t const bits = value & mask;
        value >>= num;
        return bits;
    }
};

}
