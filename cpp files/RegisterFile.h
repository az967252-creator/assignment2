#ifndef REGISTERFILE_H
#define REGISTERFILE_H

#include "Register.h"
#include <array>
#include <iostream>
#include <iomanip>

using namespace std;

// /*/*/*/* ......... */
class RegisterFile {
private:
    static const int COUNT = 16;
    Register registers[COUNT];

public:
    RegisterFile() = default;

    // /*/*/*/* ......... */
    uint8_t get(int index) const {
        if (index < 0 || index >= COUNT) return 0;
        return registers[index].get();
    }

    // /*/*/*/* ......... */
    void set(int index, uint8_t v) {
        if (index >= 0 && index < COUNT) {
            registers[index].set(v);
        }
    }

    // /*/*/*/* ......... */
    void reset() {
        for (int i = 0; i < COUNT; i++) {
            registers[i].reset();
        }
    }

    // /*/*/*/* ......... */
    array<uint8_t, 16> snapshot() const {
        array<uint8_t, 16> snap;
        for (int i = 0; i < COUNT; i++) {
            snap[i] = registers[i].get();
        }
        return snap;
    }

    // /*/*/*/* ......... */
    void display() const {
        cout << "Registers:\n";
        for (int i = 0; i < COUNT; i++) {
            cout << "  R" << uppercase << hex << i << " = " << registers[i].toHex();
            if ((i + 1) % 4 == 0) cout << "\n";
            else cout << "   ";
        }
    }
};

#endif
