#ifndef SCREEN_H
#define SCREEN_H

#include <cstdint>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// /*/*/*/* ......... */
class Screen {
private:
    string buffer;

public:
    Screen() : buffer("") {}

    // /*/*/*/* ......... */
    void print(uint8_t v) {
        ostringstream ss;
        ss << uppercase << hex << setw(2) << setfill('0') << (int)v;
        buffer += ss.str() + " ";
    }

    // /*/*/*/* ......... */
    string getContents() const {
        return buffer;
    }

    // /*/*/*/* ......... */
    void clear() {
        buffer.clear();
    }

    // /*/*/*/* ......... */
    void display() const {
        cout << "Screen Output: " << (buffer.empty() ? "(empty)" : buffer) << "\n";
    }
};

#endif
