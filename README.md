<h1><span style="color:#F5B7B1;"> Engineering Toolkit — ELEC2645 Unit 2 Project</span></h1>

Developer: Qi Chang

Student ID: 201891723

Last updated：10/12/2025

<h1><span style="color:#F5B7B1;">  Read before enjoy "ELEC2635 Toolkit"</span></h1>

<h2><span style="color:#9B59B6;"> Overview: </span></h2>

The toolkit features modules commonly used in electronics, signal processing, and embedded system work:

<table>
  <tr>
    <td>Signal Analyzer</td>
    <td>ADC / Sensor Converter</td>
  </tr>
  <tr>
    <td>Circuit Calculator</td>
    <td>Resistor Colour Decoder</td>
  </tr>
  <tr>
    <td>Unit Converter</td>
    <td>Tools (Auto-test & file utilities)</td>
  </tr>
</table>

The project emphasises:

✔️ Clean modular design

✔️ Strong input validation

✔️ Clear menu navigation

✔️ File I/O, numerical computation, ASCII graphing

<h2><span style="color:#9B59B6;"> How to run "ELEC2645 Toolkit"</span></h2>

Compile using GCC: "<span style="color:#4FA3FF;">gcc main.c funcs.c -o elec_toolkit -lm</span>"

Run the program by typing in: "<span style="color:#20D27D;">./elec_toolkit</span>"

<h2><span style="color:#9B59B6;">  Features </span></h2>

<h3><span style="color:#FF8C00;">1. Signal Analyzer</span></h3>

Digital signal analysis module providing:

Manual sample input

Load samples from file

Compute: Mean / RMS / Min / Max

ASCII bar-graph plot (negative left, positive right)

Save computed statistics to file

Files used:
+ samples.txt — user-provided sample input
+ samples_auto.txt — auto-generated for testing
+ signal_stats.txt — saved analysis results

<h3><span style="color:#FF8C00;">2. ADC / Sensor Converter</span></h3>

Convenient conversions used in embedded systems:

ADC (0–1023) → Voltage

Voltage → Temperature (LM35: 10 mV/°C)

ADC → Temperature
Uses adjustable reference voltage (default: 5.0 V, could be changed).



<h3><span style="color:#FF8C00;">3. Circuit Calculator</span></h3>

Includes:

RC low-pass cutoff frequency Calculator

Voltage divider Calculator

LED current-limit resistor Calculator

Ohm’s Law Calculator

Capacitor charging Calculator:  V(t) = Vs (1 - e^(-t/RC))

<h3><span style="color:#FF8C00;">4. Resistor Colour Decoder</span></h3>

Supports both 4-band and 5-band IEC resistor colour codes:

Automatic digit & multiplier decoding

Outputs resistance in Ω, kΩ, or MΩ

Displays tolerance percentage

Some logic adapted from online educational resources.

<h3><span style="color:#FF8C00;">5. Unit Converter</span></h3>

General engineering conversions:

dBm ↔ mW

Hz ↔ rad/s

°C ↔ K

°F ↔ °C

V ↔ dBV

<h3><span style="color:#FF8C00;">6. Engineering Calculator (Easter Egg)</span></h3>

This module contains humorous developer messages instead of real calculations — included for entertainment and as an Easter egg.

<h3><span style="color:#FF8C00;">7. Tools (Auto Tests & File Operations)</span></h3>

Designed to demonstrate automation and file handling:

Generate a sample file

Auto-test pipeline:
Load file → Calculate statistics → Draw ASCII graph → Save to signal_stats.txt

<h2><span style="color:#9B59B6;"> Notes & Acknowledgements </span></h2>

Some resistor colour-code logic was inspired by educational videos on YouTube.

The ASCII bar graph plot implementation was also learned and adapted after studying multiple online examples and tutorials.

All core implementation, menu structure, designs and extensions were written by:

Qi Chang (201891723)

Last updated：10/12/2025







