#ifndef REGISTER_H
#define REGISTER_H

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

// /*/*/*/* ......... */
class Register {
private:
    uint8_t value;

public:
    Register() : value(0) {}

    // /*/*/*/* ......... */
    uint8_t get() const {
        return value;
    }

    // /*/*/*/* ......... */
    void set(uint8_t v) {
        value = v;
    }

    // /*/*/*/* ......... */
    void reset() {
        value = 0;
    }

    // /*/*/*/* ......... */
    string toHex() const {
        ostringstream ss;
        ss << uppercase << hex << setw(2) << setfill('0') << (int)value;
        return ss.str();
    }
};

#endif
