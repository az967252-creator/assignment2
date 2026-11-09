#ifndef MINIFLOAT_H
#define MINIFLOAT_H

#include <cstdint>
#include <cmath>

using namespace std;

// /*/*/*/* ......... */
class MiniFloat {
public:
    // /*/*/*/* ......... */
    static double decode(uint8_t bits) {
        if (bits == 0) return 0.0;

        int sign = (bits >> 7) & 1;
        int exponent = (bits >> 4) & 0x7;
        int mantissa = bits & 0xF;

        double mantissaVal = 0.0;
        for (int i = 0; i < 4; i++) {
            if (mantissa & (1 << (3 - i))) {
                mantissaVal += pow(2.0, -(i + 1));
            }
        }

        double result = mantissaVal * pow(2.0, exponent - 4);
        return sign ? -result : result;
    }

    // /*/*/*/* ......... */
    static uint8_t encode(double value) {
        if (value == 0.0) return 0;

        uint8_t sign = 0;
        if (value < 0) {
            sign = 1;
            value = -value;
        }

        int exponent = 4;
        double temp = value;
        while (temp >= 1.0 && exponent < 7) {
            temp /= 2.0;
            exponent++;
        }
        while (temp < 0.5 && exponent > 0) {
            temp *= 2.0;
            exponent--;
        }

        int mantissa = 0;
        for (int i = 0; i < 4; i++) {
            temp *= 2.0;
            if (temp >= 1.0) {
                mantissa |= (1 << (3 - i));
                temp -= 1.0;
            }
        }

        if (exponent < 0) exponent = 0;
        if (exponent > 7) exponent = 7;

        return (sign << 7) | ((exponent & 0x7) << 4) | (mantissa & 0xF);
    }
};

#endif
