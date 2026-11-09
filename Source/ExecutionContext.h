#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

class RegisterFile;
class Memory;
class ALU;
class CPU;

// /*/*/*/* ......... */
struct ExecutionContext {
    RegisterFile& registers;
    Memory& memory;
    ALU& alu;
    CPU& cpu;

    ExecutionContext(RegisterFile& rf, Memory& mem, ALU& a, CPU& c)
        : registers(rf), memory(mem), alu(a), cpu(c) {}
};

#endif
