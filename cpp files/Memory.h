#ifndef MEMORY_H
#define MEMORY_H

#include "Screen.h"
#include <cstdint>
#include <array>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

// /*/*/*/* ......... */
class Memory {
private:
    static const int SIZE = 256;
    static const uint8_t SCREEN_ADDR = 0xFF;
    uint8_t cells[SIZE];
    Screen* screen;

public:
    Memory(Screen* scr) : screen(scr) {
        clear();
    }

    // /*/*/*/* ......... */
    uint8_t read(uint8_t addr) const {
        return cells[addr];
    }

    // /*/*/*/* ......... */
    void write(uint8_t addr, uint8_t v) {
        cells[addr] = v;
        if (addr == SCREEN_ADDR && screen) {
            screen->print(v);
        }
    }

    // /*/*/*/* ......... */
    uint16_t readWord(uint8_t addr) const {
        uint16_t high = cells[addr];
        uint16_t low = cells[(uint8_t)(addr + 1)];
        return (high << 8) | low;
    }

    // /*/*/*/* ......... */
    void clear() {
        memset(cells, 0, SIZE);
    }

    // /*/*/*/* ......... */
    array<uint8_t, 256> snapshot() const {
        array<uint8_t, 256> snap;
        for (int i = 0; i < SIZE; i++) {
            snap[i] = cells[i];
        }
        return snap;
    }

    // /*/*/*/* ......... */
    void displayMatrix() const {
        cout << "Memory (16x16 matrix):\n     ";
        for (int c = 0; c < 16; c++) {
            cout << uppercase << hex << setw(2) << c << " ";
        }
        cout << "\n    " << string(48, '-') << "\n";

        for (int r = 0; r < 16; r++) {
            cout << uppercase << hex << setw(2) << setfill('0') << (r * 16) << " | ";
            for (int c = 0; c < 16; c++) {
                int addr = r * 16 + c;
                cout << uppercase << hex << setw(2) << setfill('0') << (int)cells[addr] << " ";
            }
            cout << "\n";
        }
    }
};

#endif
