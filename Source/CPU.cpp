#include "ControlUnit.h"
#include "CPU.h"
#include "Memory.h"
#include "ExecutionContext.h"
#include "VoleExceptions.h"

using namespace std;

// /*/*/*/* ......... */
uint16_t ControlUnit::fetch(uint8_t pc, Memory& mem) {
    return mem.readWord(pc);
}

// /*/*/*/* ......... */
Instruction ControlUnit::decode(uint16_t raw) {
    return Instruction(raw);
}

// /*/*/*/* ......... */
void ControlUnit::execute(const Instruction& inst, ExecutionContext& ctx) {
    switch (inst.getOpcode()) {
        case Opcode::LOAD_MEM:
            execLoadMem(inst, ctx);
            break;
        case Opcode::LOAD_IMM:
            execLoadImm(inst, ctx);
            break;
        case Opcode::STORE:
            execStore(inst, ctx);
            break;
        case Opcode::MOVE:
            execMove(inst, ctx);
            break;
        case Opcode::ADD_INT:
        case Opcode::ADD_FLOAT:
        case Opcode::OR:
        case Opcode::AND:
        case Opcode::XOR:
            execArithLogic(inst, ctx);
            break;
        case Opcode::ROTATE:
            execRotate(inst, ctx);
            break;
        case Opcode::JUMP_EQ:
            execJump(inst, ctx);
            break;
        case Opcode::HALT:
            ctx.cpu.halt();
            break;
        default:
            throw InvalidInstructionException("Unknown opcode");
    }
}

// /*/*/*/* ......... */
void ControlUnit::execLoadMem(const Instruction& i, ExecutionContext& ctx) {
    uint8_t val = ctx.memory.read(i.getXY());
    ctx.registers.set(i.getR(), val);
}

// /*/*/*/* ......... */
void ControlUnit::execLoadImm(const Instruction& i, ExecutionContext& ctx) {
    ctx.registers.set(i.getR(), i.getXY());
}

// /*/*/*/* ......... */
void ControlUnit::execStore(const Instruction& i, ExecutionContext& ctx) {
    uint8_t val = ctx.registers.get(i.getR());
    ctx.memory.write(i.getXY(), val);
}

// /*/*/*/* ......... */
void ControlUnit::execMove(const Instruction& i, ExecutionContext& ctx) {
    uint8_t val = ctx.registers.get(i.getS());
    ctx.registers.set(i.getT(), val);
}

// /*/*/*/* ......... */
void ControlUnit::execArithLogic(const Instruction& i, ExecutionContext& ctx) {
    uint8_t valS = ctx.registers.get(i.getS());
    uint8_t valT = ctx.registers.get(i.getT());
    uint8_t res = 0;

    switch (i.getOpcode()) {
        case Opcode::ADD_INT:
            res = ctx.alu.addInt(valS, valT);
            break;
        case Opcode::ADD_FLOAT:
            res = ctx.alu.addFloat(valS, valT);
            break;
        case Opcode::OR:
            res = ctx.alu.bitOr(valS, valT);
            break;
        case Opcode::AND:
            res = ctx.alu.bitAnd(valS, valT);
            break;
        case Opcode::XOR:
            res = ctx.alu.bitXor(valS, valT);
            break;
        default:
            break;
    }
    ctx.registers.set(i.getR(), res);
}

// /*/*/*/* ......... */
void ControlUnit::execRotate(const Instruction& i, ExecutionContext& ctx) {
    uint8_t val = ctx.registers.get(i.getR());
    int count = i.getT();
    uint8_t res = ctx.alu.rotateRight(val, count);
    ctx.registers.set(i.getR(), res);
}

// /*/*/*/* ......... */
void ControlUnit::execJump(const Instruction& i, ExecutionContext& ctx) {
    uint8_t valR = ctx.registers.get(i.getR());
    uint8_t val0 = ctx.registers.get(0);
    if (valR == val0) {
        ctx.cpu.setPC(i.getXY());
    }
}

// /*/*/*/* ......... */
CPU::CPU() : pc(0), ir(0), halted(false) {}

// /*/*/*/* ......... */
void CPU::reset(uint8_t startAddr) {
    pc = startAddr;
    ir = 0;
    halted = false;
    registers.reset();
}

// /*/*/*/* ......... */
void CPU::step(Memory& mem) {
    if (halted) return;

    ir = cu.fetch(pc, mem);
    validator.validate(ir);
    Instruction inst = cu.decode(ir);

    pc = static_cast<uint8_t>(pc + 2);

    ExecutionContext ctx(registers, mem, alu, *this);
    cu.execute(inst, ctx);
}
