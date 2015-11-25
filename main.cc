#include "util.h"

namespace gbhd {

    template<typename t>
    t factorial(t n) {
        return n <= 1 ? 1 : n-- * factorial(n);
    }

    template<typename t>
    vector<t> primeFactors(t n) {

        t root = sqrt(n);
        vector<t> factors;
        factors.reserve(root);

        if (n < 1) return { };
        if (n == 1) return { 1 };

        while (dividable(n, 2)) {
            factors.push_back(2);
            n /= 2;
        }

        for (t m = 3; m <= root; m += 2) {
            while (dividable(n, m)) {
                factors.push_back(m);
                n /= m;
            }
        }

        return factors;
    }
}

int main(void) {

    gbhd::benchmark("factorial", [ ](void) {
        for (uint64_t n = 1; n < 100; ++n) {
            std::cout << n << "! = " << gbhd::factorial(n) << std::endl;
        }
    });

    gbhd::benchmark("primeFactors", [ ](void) {
        for (uint64_t n = 1; n < 100; ++n) {
            std::cout << n << " = ";
            for (auto const& factor : gbhd::primeFactors(n)) {
                std::cout << factor << " * ";
            }
            std::cout << std::endl;
        }
    });

    return 0;
}