#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Machine.h"
#include "VoleExceptions.h"
#include <iostream>
#include <string>
#include <limits>
#include <utility>

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
        cout << "Enter starting memory address in hex (e.g. 00, 0A, 10): ";
        string hexStr;
        cin >> hexStr;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        unsigned int addr = 0;
        stringstream ss;
        ss << hex << hexStr;
        ss >> addr;
        return static_cast<uint8_t>(addr & 0xFF);
    }

    // /*/*/*/* ......... */
    pair<int, uint8_t> askRegisterEdit() {
        cout << "Enter register index in hex (0-F): ";
        string regStr;
        cin >> regStr;
        int reg = stoi(regStr, nullptr, 16);

        cout << "Enter new 8-bit value in hex (00-FF): ";
        string valStr;
        cin >> valStr;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        unsigned int val = stoul(valStr, nullptr, 16);

        return {reg, static_cast<uint8_t>(val & 0xFF)};
    }

    // /*/*/*/* ......... */
    pair<uint8_t, uint8_t> askMemoryEdit() {
        cout << "Enter memory address in hex (00-FF): ";
        string addrStr;
        cin >> addrStr;
        unsigned int addr = stoul(addrStr, nullptr, 16);

        cout << "Enter new 8-bit value in hex (00-FF): ";
        string valStr;
        cin >> valStr;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        unsigned int val = stoul(valStr, nullptr, 16);

        return {static_cast<uint8_t>(addr & 0xFF), static_cast<uint8_t>(val & 0xFF)};
    }

    // /*/*/*/* ......... */
    RunMode askRunMode() {
        cout << "Execution mode:\n 1. Run at once\n 2. Step by step\nSelect (1/2): ";
        int mode;
        while (!(cin >> mode) || (mode != 1 && mode != 2)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter 1 or 2: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return static_cast<RunMode>(mode);
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
    void waitForEnter() {
        cout << "Press [Enter] to execute next step...";
        cin.get();
    }

    // /*/*/*/* ......... */
    void showMachineState(const Machine& m) {
        cout << "\n---------------- CURRENT STATE ----------------" << endl;
        const CPU& cpu = m.getCPU();
        cout << "PC: 0x" << uppercase << hex << setw(2) << setfill('0')
             << (int)cpu.getPC()
             << "   |   IR: 0x" << setw(4) << setfill('0') << cpu.getIR()
             << "   |   Status: " << (cpu.isHalted() ? "HALTED" : "RUNNING") << endl;
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
                        ui.showMachineState(machine);
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
        int count = machine.loadProgram(path, startAddr);
        programLoaded = true;
        ui.showMessage("Successfully loaded " + to_string(count) + " instructions.");
        ui.showMachineState(machine);
    }

    // /*/*/*/* ......... */
    void handleEditRegister() {
        auto [reg, val] = ui.askRegisterEdit();
        machine.setRegister(reg, val);
        ui.showMessage("Register R" + to_string(reg) + " updated.");
    }

    // /*/*/*/* ......... */
    void handleEditMemory() {
        auto [addr, val] = ui.askMemoryEdit();
        machine.setMemoryCell(addr, val);
        ui.showMessage("Memory cell updated.");
    }

    // /*/*/*/* ......... */
    void executeProgram(RunMode mode) {
        if (!programLoaded) {
            ui.showError("No program loaded! Please load a program first.");
            return;
        }

        if (machine.isHalted()) {
            ui.showError("Machine is currently halted. Reload or reset before running.");
            return;
        }

        if (mode == RunMode::RUN_ALL) {
            machine.runAll();
            ui.showMessage("Program execution finished.");
            ui.showMachineState(machine);
        } else {
            while (!machine.isHalted()) {
                machine.step();
                ui.showMachineState(machine);
                if (!machine.isHalted()) {
                    ui.waitForEnter();
                }
            }
            ui.showMessage("Program halted.");
        }

        if (ui.askExitOrLoadAnother()) {
            machine.reset();
            handleLoad();
        }
    }
};

#endif
