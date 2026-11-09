#ifndef OPCODE_H
#define OPCODE_H

#include <cstdint>

// /*/*/*/* ......... */
enum class Opcode : uint8_t {
    LOAD_MEM   = 0x1,
    LOAD_IMM   = 0x2,
    STORE      = 0x3,
    MOVE       = 0x4,
    ADD_INT    = 0x5,
    ADD_FLOAT  = 0x6,
    OR         = 0x7,
    AND        = 0x8,
    XOR        = 0x9,
    ROTATE     = 0xA,
    JUMP_EQ    = 0xB,
    HALT       = 0xC,
    INVALID    = 0xFF
};

// /*/*/*/* ......... */
inline Opcode fromNibble(int n) {
    switch (n) {
        case 1:  return Opcode::LOAD_MEM;
        case 2:  return Opcode::LOAD_IMM;
        case 3:  return Opcode::STORE;
        case 4:  return Opcode::MOVE;
        case 5:  return Opcode::ADD_INT;
        case 6:  return Opcode::ADD_FLOAT;
        case 7:  return Opcode::OR;
        case 8:  return Opcode::AND;
        case 9:  return Opcode::XOR;
        case 0xA: return Opcode::ROTATE;
        case 0xB: return Opcode::JUMP_EQ;
        case 0xC: return Opcode::HALT;
        default: return Opcode::INVALID;
    }
}

// /*/*/*/* ......... */
inline bool isValidOpcode(int n) {
    return n >= 1 && n <= 0xC;
}

#endif
