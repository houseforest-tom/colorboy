//
// Created by Tom on 25.11.2015.
//

#ifndef GBHD_UTIL_H
#define GBHD_UTIL_H

#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>

#define dividable(num, denom) (!(num % denom))

namespace gbhd {
    using namespace std;

    chrono::high_resolution_clock::time_point currentTime(void) {
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
