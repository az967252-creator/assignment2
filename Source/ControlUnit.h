#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#include "Instruction.h"
#include <cstdint>

using namespace std;

class Memory;
struct ExecutionContext;

// /*/*/*/* ......... */
class ControlUnit {
public:
    ControlUnit() = default;

    // /*/*/*/* ......... */
    uint16_t fetch(uint8_t pc, Memory& mem);

    // /*/*/*/* ......... */
    Instruction decode(uint16_t raw);

    // /*/*/*/* ......... */
    void execute(const Instruction& inst, ExecutionContext& ctx);

private:
    // /*/*/*/* ......... */
    void execLoadMem(const Instruction& i, ExecutionContext& ctx);

    // /*/*/*/* ......... */
    void execLoadImm(const Instruction& i, ExecutionContext& ctx);

    // /*/*/*/* ......... */
    void execStore(const Instruction& i, ExecutionContext& ctx);

    // /*/*/*/* ......... */
    void execMove(const Instruction& i, ExecutionContext& ctx);

    // /*/*/*/* ......... */
    void execArithLogic(const Instruction& i, ExecutionContext& ctx);

    // /*/*/*/* ......... */
    void execRotate(const Instruction& i, ExecutionContext& ctx);

    // /*/*/*/* ......... */
    void execJump(const Instruction& i, ExecutionContext& ctx);
};

#endif
