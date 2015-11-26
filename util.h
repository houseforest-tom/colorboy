//
// Created by Tom on 25.11.2015.
//

#ifndef GBHD_UTIL_H
#define GBHD_UTIL_H

/**
 * Common STL includes.
 */

#include <sstream>
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

namespace colorboy {
    using namespace std;

    typedef uint8_t byte;  // 8-bit.
    typedef uint16_t word;  // 16-bit

    // Wraps static utility functions.
    struct util {

        // Low-level utilities i.e. operations on bits / bytes.
        struct lowlevel {

            // Readable 8-bit hex string.
            static string hex(byte value) {
                char buf[5];
                snprintf(buf, 5, "0x%02x", value);
                return string(buf);
            }

            // Readable 16-bit hex string.
            static string hex(word value) {
                char buf[7];
                snprintf(buf, 7, "0x%04x", value);
                return string(buf);
            }

            // Concatenates 2 8-bit wide operands to a 16-bit word.
            static word concat(byte hi, byte lo) {
                return ((hi << 8) | lo);
            }
        };

        // Timing utilities.
        struct timing {

            // Point in time.
            using timestamp = chrono::high_resolution_clock::time_point;

            // Returns the current point in time
            static timestamp currentTime(void) {
                return chrono::high_resolution_clock::now();
            }

            template<typename fn, typename...args>
            static double measureExectionTime(fn function, args&& ... params) {
                auto startTime = currentTime();
                function(forward<args>(params)...);
                return chrono::duration_cast<chrono::nanoseconds>(currentTime() - startTime).count() / 1E9;
            };

            template<typename fn, typename...args>
            static void benchmark(string const& name, fn function, args&& ... params) {
                cout << "Starting benchmark '" << name << "' now." << endl;
                double dt = measureExectionTime(function, forward<args>(params)...);
                cout << "Benchmark '" << name << "' finished. Time taken: " << dt << "s." << endl;
            };
        };
    };
}

#endif //GBHD_UTIL_H
