#ifndef CPU_H
#define CPU_H

#include "RegisterFile.h"
#include "ALU.h"
#include "ControlUnit.h"
#include "InstructionValidator.h"
#include <cstdint>

using namespace std;

class Memory;

// /*/*/*/* ......... */
class CPU {
private:
    uint8_t pc;
    uint16_t ir;
    bool halted;
    RegisterFile registers;
    ALU alu;
    ControlUnit cu;
    InstructionValidator validator;

public:
    CPU();

    // /*/*/*/* ......... */
    void step(Memory& mem);

    // /*/*/*/* ......... */
    void reset(uint8_t startAddr = 0);

    // /*/*/*/* ......... */
    uint8_t getPC() const { return pc; }

    // /*/*/*/* ......... */
    void setPC(uint8_t v) { pc = v; }

    // /*/*/*/* ......... */
    uint16_t getIR() const { return ir; }

    // /*/*/*/* ......... */
    bool isHalted() const { return halted; }

    // /*/*/*/* ......... */
    void halt() { halted = true; }

    // /*/*/*/* ......... */
    RegisterFile& getRegisters() { return registers; }

    // /*/*/*/* ......... */
    const RegisterFile& getRegisters() const { return registers; }

    // /*/*/*/* ......... */
    ALU& getALU() { return alu; }
};

#endif
