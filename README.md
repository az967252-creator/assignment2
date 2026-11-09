# CS213 - Assignment 2: Vole Machine Simulator

Our implementation of the Vole machine assignment: a console simulator (Part 2) and a Qt GUI (Part 3).

- **Team:** 20250057, 20250060, 20250063, 20250391
- **Instructor:** Dr. Mohammad El-Ramly

## Repository Contents

| Path | Contents |
| --- | --- |
| `Report/` | The report with the updated (as-built) design, plus the original Part 1 report |
| `Source/` | All C++ source code (simulator and GUI). Flat folder: no executables and no Qt libraries |
| `Sample_Programs/` | 21 Vole programs (`.vole`) used for testing |
| `Screenshots/` | `Console_10_programs/`, `GUI/`, and `GitHub/` |
| `Videos.txt` | Links to the AI design video and the GUI video |
| `team.txt` | Team information |

## Part 2: Console Simulator

The console simulator is standard C++17 with no third-party libraries.

### Build

Windows (MinGW g++):

```
cd Source
build_console.bat
```

Linux / macOS:

```
cd Source
./build_console.sh
```

Or compile directly:

```
g++ -std=c++17 A2-S1_S2-*.cpp CPU.cpp -o vole_simulator
```

### Run

```
vole_simulator
```

The program opens a menu:

1. Load a program: type the file path (e.g. `../Sample_Programs/prog04_add_int.vole`), then the start address in hex (e.g. `00`)
2. Edit a register
3. Edit a memory cell
4. Display the machine state
5. Run all at once
6. Run step by step (`Enter` = next step, `q` = stop)
7. Exit

## Part 3: GUI (Qt Widgets, Qt 5 or Qt 6)

### Build

Option A - Qt Creator + CMake:

File > Open File or Project > `Source/CMakeLists.txt`, choose a Qt kit (Desktop), then Build and Run.

Option B - Qt Creator + qmake:

Open `Source/VoleGUI.pro`, choose a kit, then Build and Run.

Command line (Qt must be installed):

```
cd Source
cmake -B build
cmake --build build
```

### How to Use

- Type a start address in hex (even, below `FF`), click **Load Program...**, and pick a `.vole` file.
- **Step (F10)** runs one instruction; **Run All** runs to HALT; **Run Animated** runs with a delay set by the slider.
- **Reset** clears the machine; **Set PC** moves the program counter.
- Double-click a register (Hex column) or a memory cell to edit it.
- Yellow means the next instruction (PC); green means changed by the last step.
- The Screen panel shows everything written to memory address `FF`, in hex and ASCII.

## Source Files (`Source/`)

```
A2-S1_S2-<IDs>.cpp                       console application (main)
UserInterface.h                           UserInterface, Simulator, MenuOption, RunMode
Machine.h  CPU.h  CPU.cpp                 machine facade, CPU and ControlUnit implementation
ControlUnit.h  ExecutionContext.h  Instruction.h  InstructionValidator.h  Opcode.h
ALU.h  MiniFloat.h  Memory.h  Screen.h  Register.h  RegisterFile.h  ProgramLoader.h  VoleExceptions.h
main.cpp  MainWindow.h  MainWindow.cpp  CMakeLists.txt  VoleGUI.pro    Qt GUI
```

## Notes

- Address `FF` is the screen: any write to `FF` prints the byte on the screen.
- Invalid instructions (unknown opcode, or `40RS` / `AR0X` / `C000` with wrong operands) are rejected with a clear error message.
- Malformed program files are rejected when loading.
- For the videos, see `Videos.txt`.
