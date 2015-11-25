//
// Created by Tom on 25.11.2015.
//

#ifndef GBHD_UTIL_H
#define GBHD_UTIL_H

/**
 * Common STL includes.
 */

#include <cstdint>
#include <iostream>
#include <string>
#include <functional>
#include <cmath>
#include <chrono>
#include <vector>
#include <cstring> // memset(...)

/**
 * Macro definitions.
 */

#define RAM_SIZE    65536
#define ISET_SIZE   512
#define DIVIDABLE(num, denom) (!(num % denom))

namespace gbhd {
    using namespace std;

    typedef uint8_t byte;  // 8-bit.
    typedef uint16_t word;  // 16-bit

    // Readable 8-bit hex string.
    inline string hex(byte value) {
        char buf[5];
        snprintf(buf, 5, "0x%hhx", value);
        return string(buf);
    }

    // Readable 16-bit hex string.
    inline string hex(word value) {
        char buf[7];
        snprintf(buf, 7, "0x%hx", value);
        return string(buf);
    }

    // Concatenates 2 8-bit wide operands to a 16-bit word.
    inline word concat(byte hi, byte lo) {
        return ((hi << 8) | lo);
    }

    inline chrono::high_resolution_clock::time_point currentTime(void) {
        return chrono::high_resolution_clock::now();
    }

    template<typename fn, typename...args>
    chrono::nanoseconds measureExectionTime(fn function, args&& ... params) {
        auto startTime = currentTime();
        function(forward<args>(params)...);
        return chrono::duration_cast<chrono::nanoseconds>(currentTime() - startTime);
    };

    template<typename fn, typename...args>
    void benchmark(string const& name, fn function, args&& ... params) {
        cout << "Starting benchmark '" << name << "' now." << endl;
        auto nanos = measureExectionTime(function, forward<args>(params)...).count();
        cout << "Benchmark '" << name << "' finished. Time taken: " << (double)nanos / 1E9 << "s." << endl;
    };
}

#endif //GBHD_UTIL_H
