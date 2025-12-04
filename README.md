# Engineering Toolkit — ELEC2645 Unit 2 Project
Developer: Qi Chang
Student ID: 201891723
Last updated：04/12/2025

# Read before enjoy "ELEC2635 Toolkit"

# Overview:
This project is an Engineering Toolkit implemented in C for the ELEC2645 Embedded Systems module.
It is a menu-driven command-line application that provides a collection of small engineering utilities commonly used in electronics, signal processing, and embedded system work.

The toolkit includes a Signal Analyzer, ADC/Sensor converters, circuit calculators, resistor colour decoders, unit converters, and a Tools module featuring auto-testing and file operations.

The project emphasises:
+ Clean modular design
+ Robust user input validation
+ Clear menu-driven workflow
+ File I/O, numerical computation, and ASCII display techniques

# How to run "ELEC2645 Toolkit":
Compile using GCC: "gcc main.c funcs.c -o elec_toolkit -lm"
Run the program by type in: "./elec_toolkit"

# Features:
1. Signal Analyzer
Provides simple digital signal analysis functions.
Functions:
Manual sample entry  /  Load sample list from text file  /  Compute: Mean, RMS, Min or Max
ASCII bar graph plot (negative left, positive right)  /  Save computed statistics to a file

Files used:
samples.txt — user-provided sample input
samples_auto.txt — auto-generated for testing
signal_stats.txt — saved analysis results

2. ADC / Sensor Converter
Convenience utilities for embedded sensor systems.
Conversions: 
ADC (0–1023) → Voltage / Voltage → Temperature (LM35: 10 mV/°C) / ADC → Temperature
A simple reference voltage (5 V by default, could be changed) is used for scaling.

3. Circuit Calculator
A collection of common circuit-related formulas.
Includes:
RC low-pass filter cutoff frequency / Voltage divider / LED current-limit resistor / 
Ohm’s law / Capacitor charging equation: V(t) = Vs (1 - e^(-t/RC))

4. Resistor Colour Decoder
Supports both 4-band and 5-band resistors.
Output: Nominal resistance / Automatic unit scaling (Ω, kΩ, MΩ) / Tolerance
Supports all IEC standard colours.

5. Unit Converter
General engineering unit conversions: 
dBm ↔ mW / Hz ↔ rad/s / °C ↔ K / °F ↔ °C / V ↔ dBV

6. Engineering Calculator
It includes humorous developer comments as an Easter egg.

7. Tools (Auto Tests & File Operations)
Designed to demonstrate automation and file handling.
Functions:
Generate a sample auto-test file
Run automated pipeline: Load file -> Calculate statistics -> Draw ASCII plot -> Save results to "signal_stats.txt"

# Notes & Acknowledgements
Some resistor colour-code logic was inspired by educational videos on YouTube.
The ASCII bar graph plot implementation was also learned and adapted after studying multiple online examples and tutorials.
All core implementation, menu structure, designs and extensions were written by:
Qi Chang (201891723)
Last updated：04/12/2025








