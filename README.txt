CS213 - Assignment 2: Vole Machine Simulator (Part 2 console simulator + Part 3 Qt GUI)
Team: 20250057, 20250060, 20250063, 20250391      Instructor: Dr. Mohammad El-Ramly

FOLDERS
  Report/            Report with the updated (as-built) design + the original Part 1 report
  Source/            All C++ source (simulator + GUI), flat folder, no exe and no Qt libraries
  Sample_Programs/   21 Vole programs (.vole) used for testing
  Screenshots/       Console_10_programs/, GUI/, GitHub/
  Videos.txt         Links to the AI design video and the GUI video
  team.txt           Team information

1) CONSOLE SIMULATOR (standard C++17, no third-party libraries)
   Windows (MinGW g++):   cd Source  then  build_console.bat
   Linux / macOS:         cd Source  then  ./build_console.sh
   Or directly:           g++ -std=c++17 A2-S1_S2-*.cpp CPU.cpp -o vole_simulator
   Run:                   vole_simulator
   Use the menu: 1 load a program (type the file path, e.g. ../Sample_Programs/prog04_add_int.vole,
   then the start address in hex such as 00), 2 edit a register, 3 edit a memory cell,
   4 display the machine state, 5 run all at once, 6 run step by step (Enter = next step, q = stop), 7 exit.

2) GUI (Qt Widgets, Qt 5 or Qt 6)
   Option A - Qt Creator + CMake: File > Open File or Project > Source/CMakeLists.txt,
              choose a Qt kit (Desktop), then Build and Run.
   Option B - Qt Creator + qmake: open Source/VoleGUI.pro, choose a kit, Build and Run.
   Command line:  cd Source && cmake -B build && cmake --build build   (Qt must be installed)
   How to use:
     - Type a start address in hex (even, below FF), click "Load Program...", pick a .vole file.
     - "Step (F10)" runs one instruction; "Run All" runs to HALT; "Run Animated" runs with a delay
       (slider); "Reset" clears the machine; "Set PC" moves the program counter.
     - Double-click a register (Hex column) or a memory cell to edit it.
     - Yellow = next instruction (PC), green = changed by the last step. The Screen panel shows
       everything written to memory address FF (hex and ASCII).

FILE OVERVIEW (Source/)
  A2-S1_S2-<IDs>.cpp          console application (main)
  UserInterface.h               UserInterface, Simulator, MenuOption, RunMode
  Machine.h  CPU.h  CPU.cpp     machine facade, CPU and ControlUnit implementation
  ControlUnit.h ExecutionContext.h Instruction.h InstructionValidator.h Opcode.h
  ALU.h MiniFloat.h Memory.h Screen.h Register.h RegisterFile.h ProgramLoader.h VoleExceptions.h
  main.cpp MainWindow.h MainWindow.cpp CMakeLists.txt VoleGUI.pro     Qt GUI

NOTES
  - Address FF is the screen: any write to FF prints the byte on the screen.
  - Invalid instructions (unknown opcode, 40RS / AR0X / C000 with wrong operands) are rejected with a
    clear error message; malformed program files are rejected when loading.
  - Videos: see Videos.txt.
