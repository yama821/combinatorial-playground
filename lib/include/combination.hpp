#ifndef LIB_COMBINATION_HPP
#define LIB_COMBINATION_HPP 1

#include <algorithm>
/**
 * TODO: update comb function
 * 1. use large prime and precalculation
 * 2. use DP
 */

long long comb(int n, int r) {
    if (r < 0 || n < 0) return 0;
    if (r > n) return 0;
    if (r == 0 || r == n) return 1;
    if (n < 0) return 0;
    if (r > n) return 0;
    if (r == n) return 1;

    r = std::min(r, n - r);

    long long ret = 1;
    for (int i = 1; i <= r; i++) {
        ret *= n - i;
        ret = ret * (n - r + i) / i;
    }
    return ret;
}

#endif // LIB_COMBINATION_HPP
