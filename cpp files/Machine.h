#ifndef MACHINE_H
#define MACHINE_H

#include "CPU.h"
#include "Memory.h"
#include "Screen.h"
#include "ProgramLoader.h"
#include <cstdint>
#include <string>

using namespace std;

// /*/*/*/* ......... */
class Machine {
private:
    uint8_t startAddress;
    Screen screen;
    Memory memory;
    CPU cpu;
    ProgramLoader loader;

public:
    Machine() : startAddress(0), memory(&screen) {}

    // /*/*/*/* ......... */
    int loadProgram(const string& path, uint8_t startAddr) {
        startAddress = startAddr;
        int count = loader.load(path, startAddr, memory);
        cpu.reset(startAddr);
        return count;
    }

    // /*/*/*/* ......... */
    void setRegister(int index, uint8_t v) {
        cpu.getRegisters().set(index, v);
    }

    // /*/*/*/* ......... */
    uint8_t getRegister(int index) const {
        return cpu.getRegisters().get(index);
    }

    // /*/*/*/* ......... */
    void setMemoryCell(uint8_t addr, uint8_t v) {
        memory.write(addr, v);
    }

    // /*/*/*/* ......... */
    uint8_t getMemoryCell(uint8_t addr) const {
        return memory.read(addr);
    }

    // /*/*/*/* ......... */
    void step() {
        cpu.step(memory);
    }

    // /*/*/*/* ......... */
    void runAll() {
        while (!cpu.isHalted()) {
            cpu.step(memory);
        }
    }

    // /*/*/*/* ......... */
    bool isHalted() const {
        return cpu.isHalted();
    }

    // /*/*/*/* ......... */
    void reset() {
        memory.clear();
        screen.clear();
        cpu.reset(0);
        startAddress = 0;
    }

    // /*/*/*/* ......... */
    CPU& getCPU() { return cpu; }

    // /*/*/*/* ......... */
    const CPU& getCPU() const { return cpu; }

    // /*/*/*/* ......... */
    Memory& getMemory() { return memory; }

    // /*/*/*/* ......... */
    const Memory& getMemory() const { return memory; }

    // /*/*/*/* ......... */
    Screen& getScreen() { return screen; }

    // /*/*/*/* ......... */
    const Screen& getScreen() const { return screen; }
};

#endif
