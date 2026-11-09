#!/bin/sh
# Builds the console simulator (needs g++)
g++ -std=c++17 -Wall A2-S1_S2-20250057_20250060_20250063_20250391.cpp CPU.cpp -o vole_simulator && echo "Built ./vole_simulator"
