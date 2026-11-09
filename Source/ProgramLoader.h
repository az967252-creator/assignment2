#ifndef PROGRAM_LOADER_H
#define PROGRAM_LOADER_H

#include "Memory.h"
#include "VoleExceptions.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cctype>

using namespace std;

// /*/*/*/* ......... */
class ProgramLoader {
public:
    ProgramLoader() = default;

    // /*/*/*/* ......... */
    int load(const string& path, uint8_t startAddr, Memory& mem) {
        checkStartAddress(startAddr);

        ifstream file(path);
        if (!file.is_open()) {
            throw ProgramFormatException("Cannot open file: " + path);
        }

        string line;
        vector<uint16_t> instructions;

        while (getline(file, line)) {
            auto parsed = parseLine(line);
            if (parsed.has_value()) {
                instructions.push_back(parsed.value);
            }
        }

        if (instructions.empty()) {
            throw ProgramFormatException("No valid instructions found in: " + path);
        }

        checkFits(startAddr, static_cast<int>(instructions.size()));

        uint8_t currentAddr = startAddr;
        for (uint16_t inst : instructions) {
            uint8_t high = static_cast<uint8_t>((inst >> 8) & 0xFF);
            uint8_t low = static_cast<uint8_t>(inst & 0xFF);
            mem.write(currentAddr, high);
            mem.write(static_cast<uint8_t>(currentAddr + 1), low);
            currentAddr = static_cast<uint8_t>(currentAddr + 2);
        }

        return static_cast<int>(instructions.size());
    }

private:
    struct OptionalInstruction {
        bool valid;
        uint16_t value;
        bool has_value() const { return valid; }
    };

    // /*/*/*/* ......... */
    OptionalInstruction parseLine(const string& line) {
        size_t commentPos = line.find(';');
        string content = (commentPos != string::npos) ? line.substr(0, commentPos) : line;

        size_t first = content.find_first_not_of(" \t\r\n");
        if (first == string::npos) {
            return {false, 0};
        }
        size_t last = content.find_last_not_of(" \t\r\n");
        string trimmed = content.substr(first, (last - first + 1));

        istringstream iss(trimmed);
        string token;
        if (!(iss >> token)) {
            return {false, 0};
        }

        if (token.length() != 4) {
            throw ProgramFormatException("Instruction must be 4 hex digits: " + token);
        }

        for (char c : token) {
            if (!isxdigit(static_cast<unsigned char>(c))) {
                throw ProgramFormatException("Invalid hex character: " + token);
            }
        }

        uint16_t val = static_cast<uint16_t>(stoul(token, nullptr, 16));
        return {true, val};
    }

    // /*/*/*/* ......... */
    void checkStartAddress(uint8_t addr) {
        if (addr % 2 != 0) {
            throw InvalidAddressException("Start address must be even");
        }
        if (addr >= 0xFF) {
            throw InvalidAddressException("Start address must be less than FF");
        }
    }

    // /*/*/*/* ......... */
    void checkFits(uint8_t startAddr, int count) {
        if (startAddr + count * 2 > 256) {
            throw ProgramTooLargeException("Program does not fit in memory");
        }
    }
};

#endif
