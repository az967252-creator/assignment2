#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Machine.h"
#include "VoleExceptions.h"
#include <iostream>
#include <string>
#include <limits>
#include <utility>
#include <sstream>
#include <cctype>
#include <cstdint>

using namespace std;

// /*/*/*/* ......... */
enum class MenuOption {
    LOAD_PROGRAM = 1,
    EDIT_REGISTER,
    EDIT_MEMORY,
    DISPLAY_STATE,
    RUN_ALL,
    RUN_STEP,
    EXIT
};

// /*/*/*/* ......... */
enum class RunMode {
    RUN_ALL = 1,
    STEP_BY_STEP
};

// /*/*/*/* ......... */
class UserInterface {
public:
    // /*/*/*/* ......... */
    MenuOption showMenu() {
        cout << "\n==========================================" << endl;
        cout << "      VOLE MACHINE SIMULATOR - MENU       " << endl;
        cout << "==========================================" << endl;
        cout << "1. Load a new program from file" << endl;
        cout << "2. Edit register content" << endl;
        cout << "3. Edit memory cell content" << endl;
        cout << "4. Display machine state" << endl;
        cout << "5. Run program (All at once)" << endl;
        cout << "6. Run program (Step by step)" << endl;
        cout << "7. Exit" << endl;
        cout << "------------------------------------------" << endl;
        cout << "Choose an option (1-7): ";

        int choice;
        while (!(cin >> choice) || choice < 1 || choice > 7) {
            if (cin.eof()) {
                return MenuOption::EXIT;   // input ended, so exit instead of looping forever
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice! Enter a number between 1 and 7: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return static_cast<MenuOption>(choice);
    }

    // /*/*/*/* ......... */
    string askFilePath() {
        cout << "Enter program file path (.vole): ";
        string path;
        getline(cin, path);
        return path;
    }

    // /*/*/*/* ......... */
    uint8_t askStartAddress() {
        unsigned int addr = readHex("Enter starting memory address in hex (e.g. 00, 0A, 10): ", 2,
                                    "start address must be a hex value between 00 and FF");
        return static_cast<uint8_t>(addr);
    }

    // /*/*/*/* ......... */
    pair<int, uint8_t> askRegisterEdit() {
        unsigned int reg = readHex("Enter register index in hex (0-F): ", 1,
                                   "register index must be a single hex digit 0-F");
        unsigned int val = readHex("Enter new 8-bit value in hex (00-FF): ", 2,
                                   "value must be a hex number between 00 and FF");
        return {static_cast<int>(reg), static_cast<uint8_t>(val)};
    }

    // /*/*/*/* ......... */
    pair<uint8_t, uint8_t> askMemoryEdit() {
        unsigned int addr = readHex("Enter memory address in hex (00-FF): ", 2,
                                    "address must be a hex number between 00 and FF");
        unsigned int val = readHex("Enter new 8-bit value in hex (00-FF): ", 2,
                                   "value must be a hex number between 00 and FF");
        return {static_cast<uint8_t>(addr), static_cast<uint8_t>(val)};
    }

    // /*/*/*/* ......... */
    bool askExitOrLoadAnother() {
        cout << "\nExecution finished. Load another program? (y/n): ";
        char c;
        cin >> c;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return (c == 'y' || c == 'Y');
    }

    // /*/*/*/* ......... */
    // Returns false if the user typed q (stop) or the input ended.
    bool waitForEnter() {
        cout << "Press [Enter] for the next step (or type q then [Enter] to stop)...";
        string line;
        if (!getline(cin, line)) {
            return false;
        }
        return !(line == "q" || line == "Q");
    }

    // /*/*/*/* ......... */
    void showMachineState(const Machine& m, bool programLoaded) {
        cout << "\n---------------- CURRENT STATE ----------------" << endl;
        const CPU& cpu = m.getCPU();
        cout << "PC: 0x" << uppercase << hex << setw(2) << setfill('0')
             << (int)cpu.getPC()
             << "   |   IR: 0x" << setw(4) << setfill('0') << cpu.getIR()
             << "   |   Status: "
             << (!programLoaded ? "NO PROGRAM" : (cpu.isHalted() ? "HALTED" : "RUNNING")) << endl;
        cout << endl;
        cpu.getRegisters().display();
        cout << endl;
        m.getScreen().display();
        cout << endl;
        m.getMemory().displayMatrix();
        cout << "-----------------------------------------------" << endl;
    }

    // /*/*/*/* ......... */
    void showError(const string& msg) {
        cerr << "\n[ERROR]: " << msg << endl;
    }

    // /*/*/*/* ......... */
    void showMessage(const string& msg) {
        cout << "\n[INFO]: " << msg << endl;
    }

private:
    // Strict hex parser: only 0-9 / A-F (optional 0x prefix), 1..maxDigits digits.
    static unsigned int parseHex(string text, size_t maxDigits, const string& what) {
        if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
            text = text.substr(2);
        }
        if (text.empty() || text.size() > maxDigits) {
            throw VoleException("Invalid input: " + what);
        }
        unsigned int value = 0;
        for (char c : text) {
            if (!isxdigit(static_cast<unsigned char>(c))) {
                throw VoleException("Invalid input: " + what);
            }
            int digit = isdigit(static_cast<unsigned char>(c)) ? (c - '0')
                                                              : (toupper(static_cast<unsigned char>(c)) - 'A' + 10);
            value = value * 16 + static_cast<unsigned int>(digit);
        }
        return value;
    }

    // Prints a prompt, reads one word, discards the rest of the line, validates it.
    static unsigned int readHex(const string& prompt, size_t maxDigits, const string& what) {
        cout << prompt;
        string text;
        cin >> text;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return parseHex(text, maxDigits, what);
    }
};

// /*/*/*/* ......... */
class Simulator {
private:
    Machine machine;
    UserInterface ui;
    bool programLoaded;

public:
    Simulator() : programLoaded(false) {}

    // /*/*/*/* ......... */
    void run() {
        bool running = true;
        while (running) {
            try {
                MenuOption opt = ui.showMenu();
                switch (opt) {
                    case MenuOption::LOAD_PROGRAM:
                        handleLoad();
                        break;
                    case MenuOption::EDIT_REGISTER:
                        handleEditRegister();
                        break;
                    case MenuOption::EDIT_MEMORY:
                        handleEditMemory();
                        break;
                    case MenuOption::DISPLAY_STATE:
                        ui.showMachineState(machine, programLoaded);
                        break;
                    case MenuOption::RUN_ALL:
                        executeProgram(RunMode::RUN_ALL);
                        break;
                    case MenuOption::RUN_STEP:
                        executeProgram(RunMode::STEP_BY_STEP);
                        break;
                    case MenuOption::EXIT:
                        running = false;
                        ui.showMessage("Exiting Vole Simulator. Goodbye!");
                        break;
                }
            } catch (const VoleException& ve) {
                ui.showError(ve.what());
            } catch (const exception& e) {
                ui.showError(string("Unexpected error: ") + e.what());
            }
        }
    }

private:
    // /*/*/*/* ......... */
    void handleLoad() {
        string path = ui.askFilePath();
        uint8_t startAddr = ui.askStartAddress();
        machine.reset();            // always start from a clean machine
        programLoaded = false;      // stays false if loading fails below
        int count = machine.loadProgram(path, startAddr);
        programLoaded = true;
        ui.showMessage("Successfully loaded " + to_string(count) + " instructions.");
        ui.showMachineState(machine, programLoaded);
    }

    // /*/*/*/* ......... */
    void handleEditRegister() {
        if (!programLoaded) {
            ui.showMessage("Note: loading a program (option 1) resets all registers and memory.");
        }
        auto [reg, val] = ui.askRegisterEdit();
        machine.setRegister(reg, val);
        ostringstream ss;
        ss << uppercase << hex << reg;
        ui.showMessage("Register R" + ss.str() + " updated.");
    }

    // /*/*/*/* ......... */
    void handleEditMemory() {
        if (!programLoaded) {
            ui.showMessage("Note: loading a program (option 1) resets all registers and memory.");
        }
        auto [addr, val] = ui.askMemoryEdit();
        machine.setMemoryCell(addr, val);
        ostringstream ss;
        ss << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(addr);
        ui.showMessage("Memory cell " + ss.str() + " updated.");
    }

    // /*/*/*/* ......... */
    void executeProgram(RunMode mode) {
        if (!programLoaded) {
            ui.showError("No program loaded! Please load a program first (option 1).");
            return;
        }

        if (machine.isHalted()) {
            ui.showError("The program has already halted. Load it again with option 1 to run it again.");
            return;
        }

        if (mode == RunMode::RUN_ALL) {
            if (!machine.runAll()) {
                ui.showError("Stopped after 1,000,000 steps without reaching HALT (possible infinite loop).");
                ui.showMachineState(machine, programLoaded);
                return;
            }
            ui.showMessage("Program execution finished.");
            ui.showMachineState(machine, programLoaded);
        } else {
            while (!machine.isHalted()) {
                machine.step();
                ui.showMachineState(machine, programLoaded);
                if (!machine.isHalted() && !ui.waitForEnter()) {
                    ui.showMessage("Stopped. Choose option 6 to continue from here.");
                    return;
                }
            }
            ui.showMessage("Program halted.");
        }

        if (ui.askExitOrLoadAnother()) {
            handleLoad();
        }
    }
};

#endif