#ifndef INSTRUCTION_VALIDATOR_H
#define INSTRUCTION_VALIDATOR_H

#include "Opcode.h"
#include "VoleExceptions.h"
#include <cstdint>
#include <sstream>
#include <iomanip>

using namespace std;

// /*/*/*/* ......... */
class InstructionValidator {
public:
    // /*/*/*/* ......... */
    void validate(uint16_t raw) const {
        int op = (raw >> 12) & 0xF;
        if (!isValidOpcode(op)) {
            ostringstream ss;
            ss << "Unknown opcode 0x" << hex << op << " in instruction 0x"
               << setw(4) << setfill('0') << raw;
            throw InvalidInstructionException(ss.str());
        }

        switch (fromNibble(op)) {
            case Opcode::MOVE:
                validateMove(raw);
                break;
            case Opcode::ROTATE:
                validateRotate(raw);
                break;
            case Opcode::HALT:
                validateHalt(raw);
                break;
            default:
                break;
        }
    }

private:
    // /*/*/*/* ......... */
    void validateMove(uint16_t raw) const {
        int secondNibble = (raw >> 8) & 0xF;
        if (secondNibble != 0) {
            ostringstream ss;
            ss << "MOVE instruction must have 0 as 2nd nibble (40RS), got 0x"
               << hex << setw(4) << setfill('0') << raw;
            throw InvalidInstructionException(ss.str());
        }
    }

    // /*/*/*/* ......... */
    void validateRotate(uint16_t raw) const {
        int thirdNibble = (raw >> 4) & 0xF;
        if (thirdNibble != 0) {
            ostringstream ss;
            ss << "ROTATE instruction must have 0 as 3rd nibble (AR0X), got 0x"
               << hex << setw(4) << setfill('0') << raw;
            throw InvalidInstructionException(ss.str());
        }
    }

    // /*/*/*/* ......... */
    void validateHalt(uint16_t raw) const {
        if ((raw & 0x0FFF) != 0) {
            ostringstream ss;
            ss << "HALT instruction must be C000, got 0x"
               << hex << setw(4) << setfill('0') << raw;
            throw InvalidInstructionException(ss.str());
        }
    }
};

#endif
