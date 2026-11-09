QT += core gui widgets
CONFIG += c++17

TARGET = VoleGUI
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    CPU.cpp

HEADERS += \
    MainWindow.h \
    ALU.h CPU.h ControlUnit.h ExecutionContext.h Instruction.h \
    InstructionValidator.h Machine.h Memory.h MiniFloat.h Opcode.h \
    ProgramLoader.h Register.h RegisterFile.h Screen.h VoleExceptions.h
