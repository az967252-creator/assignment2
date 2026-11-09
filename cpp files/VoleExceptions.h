#ifndef VOLE_EXCEPTIONS_H
#define VOLE_EXCEPTIONS_H

#include <stdexcept>
#include <string>

using namespace std;

// /*/*/*/* ......... */
class VoleException : public runtime_error {
public:
    explicit VoleException(const string& msg) : runtime_error(msg) {}
};

// /*/*/*/* ......... */
class InvalidInstructionException : public VoleException {
public:
    explicit InvalidInstructionException(const string& msg)
        : VoleException("Invalid instruction: " + msg) {}
};

// /*/*/*/* ......... */
class InvalidAddressException : public VoleException {
public:
    explicit InvalidAddressException(const string& msg)
        : VoleException("Invalid address: " + msg) {}
};

// /*/*/*/* ......... */
class ProgramFormatException : public VoleException {
public:
    explicit ProgramFormatException(const string& msg)
        : VoleException("Program format error: " + msg) {}
};

// /*/*/*/* ......... */
class ProgramTooLargeException : public VoleException {
public:
    explicit ProgramTooLargeException(const string& msg)
        : VoleException("Program too large: " + msg) {}
};

#endif
