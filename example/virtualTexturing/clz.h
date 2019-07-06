//
// Created by felix on 06/07/2019.
//

#ifndef WAGL2_CLZ_H
#define WAGL2_CLZ_H

#include <cstdint>

inline uint64_t msb_n(uint64_t x, uint64_t n) {
    uint64_t r = 0;
    while(x >>= n) r++;
    return r;
}

inline uint64_t msb(uint64_t x) {
    return msb_n(x, 1u);
}

inline uint64_t clz_n(uint64_t x, uint64_t n) {
    if (x == 0) {
        return 64;
    }
    return 64u - msb_n(x, n);
}

inline uint64_t clz(uint64_t x) {
    return clz_n(x, 1);
}

#endif //WAGL2_CLZ_H
