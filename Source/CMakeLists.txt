cmake_minimum_required(VERSION 3.16)
project(VoleGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Works with both Qt 5 and Qt 6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(VoleGUI
    main.cpp
    MainWindow.cpp
    MainWindow.h
    CPU.cpp          # ControlUnit + CPU implementation (your original file)

    # your original headers (listed so they show up in Qt Creator)
    ALU.h CPU.h ControlUnit.h ExecutionContext.h Instruction.h
    InstructionValidator.h Machine.h Memory.h MiniFloat.h Opcode.h
    ProgramLoader.h Register.h RegisterFile.h Screen.h VoleExceptions.h
)

target_link_libraries(VoleGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32)
    set_target_properties(VoleGUI PROPERTIES WIN32_EXECUTABLE ON)
endif()
