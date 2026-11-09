@echo off
REM Builds the console simulator (needs g++ / MinGW)
g++ -std=c++17 -Wall A2-S1_S2-20250057_20250060_20250063_20250391.cpp CPU.cpp -o vole_simulator.exe
if %errorlevel%==0 (echo Built vole_simulator.exe) else (echo Build failed)
