#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Opcode.h"
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

// /*/*/*/* ......... */
class Instruction {
private:
    uint16_t raw;
    Opcode opcode;
    int r;
    int s;
    int t;
    uint8_t xy;

public:
    Instruction(uint16_t rawValue = 0) : raw(rawValue) {
        int opNibble = (raw >> 12) & 0xF;
        opcode = fromNibble(opNibble);
        r = (raw >> 8) & 0xF;
        s = (raw >> 4) & 0xF;
        t = raw & 0xF;
        xy = static_cast<uint8_t>(raw & 0xFF);
    }

    // /*/*/*/* ......... */
    uint16_t getRaw() const { return raw; }

    // /*/*/*/* ......... */
    Opcode getOpcode() const { return opcode; }

    // /*/*/*/* ......... */
    int getR() const { return r; }

    // /*/*/*/* ......... */
    int getS() const { return s; }

    // /*/*/*/* ......... */
    int getT() const { return t; }

    // /*/*/*/* ......... */
    uint8_t getXY() const { return xy; }

    // /*/*/*/* ......... */
    int getX() const { return s; }

    // /*/*/*/* ......... */
    string toHexString() const {
        ostringstream ss;
        ss << uppercase << hex << setw(4) << setfill('0') << raw;
        return ss.str();
    }
};

#endif
