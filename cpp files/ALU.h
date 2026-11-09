#ifndef ALU_H
#define ALU_H

#include "MiniFloat.h"
#include <cstdint>

using namespace std;

// /*/*/*/* ......... */
class ALU {
public:
    // /*/*/*/* ......... */
    uint8_t addInt(uint8_t a, uint8_t b) const {
        return (uint8_t)(a + b);
    }

    // /*/*/*/* ......... */
    uint8_t addFloat(uint8_t a, uint8_t b) const {
        double fa = MiniFloat::decode(a);
        double fb = MiniFloat::decode(b);
        return MiniFloat::encode(fa + fb);
    }

    // /*/*/*/* ......... */
    uint8_t bitOr(uint8_t a, uint8_t b) const {
        return a | b;
    }

    // /*/*/*/* ......... */
    uint8_t bitAnd(uint8_t a, uint8_t b) const {
        return a & b;
    }

    // /*/*/*/* ......... */
    uint8_t bitXor(uint8_t a, uint8_t b) const {
        return a ^ b;
    }

    // /*/*/*/* ......... */
    uint8_t rotateRight(uint8_t v, int n) const {
        n = n % 8;
        if (n == 0) return v;
        return (v >> n) | (v << (8 - n));
    }
};

#endif
