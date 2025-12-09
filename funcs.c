// ELEC2645 Unit 2 Embedded Systems Project -- Engineering Toolkit
// Designed by Qi Chang, Student id：201891723
// Last updated： 09/12/2025

/* ================================================== *
 * Function implementations for "Engineering Toolkit" *
 * ================================================== *
 *
 * Contains the implementations of all menu handler functions and 
 * helper routines used by the command-line engineering toolkit:
 *
 * - Signal Analyzer module (input, statistics, ASCII bar plot, file Saving)
 * - ADC / Sensor Converter
 * - Circuit Calculator
 * - Resistor Colour Decoder
 * - Unit Converter
 * - Engineering Calculator placeholder
 * - Tools (auto-test and extra file operations for the Signal Analyzer (generate test data) )
 */

#include <stdio.h>                               
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "funcs.h"

/* System doesnt auto define M_PI */
#ifndef M_PI 
#define M_PI 3.14159265358979323846
#endif

/* Global signal-statistics structure */
SignalStats g_last_signal; /* defined here, but declared as extern in funcs.h */

/* ================= *
 * Local prototypes  *
 * ================= */

/* Signal Analyzer */
static void sa_print_menu(void);
static int  sa_get_choice(void);
static int  sa_is_integer(const char *s);

/* Unit Converter */
static void uc_print_menu(void);
static int  uc_get_choice(void);
static double uc_get_double(const char *prompt);

/* Circuit Calculator */
static void cc_rc_filter(void);
static void cc_voltage_divider(void);
static void cc_led_resistor(void);
static void cc_ohms_law(void);
static void cc_cap_charging(void);

/* Resistor colour decoder helpers (4-band / 5-band decoders) */
static void str_to_lower(char *s);
static int  resistor_digit_from_color(const char *s);
static int  resistor_multiplier_from_color(const char *s, double *mult);
static int  resistor_tolerance_from_color(const char *s, double *tol);
static void resistor_decode_4band(void);
static void resistor_decode_5band(void);

/* Tools (Auto Tests & File Operations for Signal Analyzer) */
static void tools_generate_sample_file(void);
static void tools_run_auto_test(void);
/* Generic loader used both by the Signal Analyzer and Tools */
static int sa_load_from_file_generic(const char *filename);  /* Loads samples from an arbitrary filename into g_last_signal */


/* ========================== *
 * 1. Signal Analyzer Module  *
 * ========================== */       /* reads a choice (1~5), and calls the appropriate function */
void menu_signal_analyzer(void)        /* until the user selects “Back to Main Menu” (6) */
{
    int running = 1;

    while (running) {
        sa_print_menu();
        int choice = sa_get_choice();

        switch (choice) {
            case 1:
                sa_enter_samples();
                break;
            case 2:
                sa_load_from_file();
                break;
            case 3:
                sa_calculate_stats();
                break;
            case 4:
                sa_plot_graph();
                break;
            case 5:
                sa_save_stats_to_file();
                break;
            case 6:
                /* Back to main menu */
                running = 0;
                break;
            default:
                printf("Unknown option.\n");
                break;
        }
    }
}                                     
static void sa_print_menu(void)       /* Print Signal Analyzer submenu */
{
    printf("\n------ Signal Analyzer ------\n");
    printf("1. Enter samples manually\n");
    printf("2. Load samples from file\n");
    printf("3. Calculate statistics (mean, min, max, RMS)\n");
    printf("4. Plot ASCII bar graph\n");
    printf("5. Save stats to file\n");
    printf("6. Back to Main Menu\n");
    printf("-----------------------------\n");
}
static int sa_get_choice(void)        /* Retrieve menu options 1–6 with basic input validation */
{
    enum { MIN_OPT = 1, MAX_OPT = 6 };
    char buf[64];
    int value = 0;

    for (;;) {
        printf("Select option (%d-%d): ", MIN_OPT, MAX_OPT);
        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\nInput error, returning to main menu.\n");
            return MAX_OPT;     /* Back to Main Menu */
        }

        buf[strcspn(buf, "\r\n")] = '\0';  /* Remove line breaks */

        if (!sa_is_integer(buf)) {
            printf("Please enter an integer!\n");
            continue;
        }

        value = (int)strtol(buf, NULL, 10);
        if (value < MIN_OPT || value > MAX_OPT) {
            printf("Invalid option, choose between %d and %d.\n",
                   MIN_OPT, MAX_OPT);
            continue;
        }

        return value;
    }
}

/* Local integer-check helper used by both Signal Analyzer and Unit Converter. */
static int sa_is_integer(const char *s)
{
    if (!s || !*s) return 0;

    if (*s == '+' || *s == '-') s++;

    if (!isdigit((unsigned char)*s)) return 0;

    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

/* ============================= *
 * 1. Signal Analyzer Functions  *
 * ============================= */
void sa_enter_samples(void)
{
    char buf[64];
    int count = 0;

    /* 1. Ask User for sample number */
    while (1) {
        printf("\nHow many samples do you want to enter? (1-%d): ",
               MAX_SAMPLES);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\nInput error. Cancelling.\n");
            return;
        }
        buf[strcspn(buf, "\r\n")] = '\0';  /* Remove line breaks */

        if (!sa_is_integer(buf)) {
            printf("Please enter an integer number.\n");
            continue;
        }

        count = (int)strtol(buf, NULL, 10);
        if (count < 1 || count > MAX_SAMPLES) {
            printf("Please choose a value between 1 and %d.\n", MAX_SAMPLES);
            continue;
        }

        break;  /* if it can run then exit loop */
    }

    g_last_signal.count = count;

    /* 2. Input the samples */
    for (int i = 0; i < count; i++) {
        double value = 0.0;
        while (1) {
            printf("Enter sample %d: ", i + 1);

            if (!fgets(buf, sizeof(buf), stdin)) {
                printf("\nInput error while reading sample. "
                       "Stopping input.\n");
                g_last_signal.count = i;  /* Number successfully read */
                return;
            }
            buf[strcspn(buf, "\r\n")] = '\0';

            char *endptr = NULL;
            value = strtod(buf, &endptr);

            /* Check whether the entire string is a valid floating-point number. */
            if (endptr == buf || *endptr != '\0') {
                printf("Please enter a valid number.\n");
                continue;
            }

            g_last_signal.samples[i] = value;
            break;  /* Exit the inner loop */
        }
    }

    /* 3. Temporarily reset the statistics to zero; the next step will involve the actual calculation by `sa_calculate_stats`. */
    g_last_signal.mean = 0.0;
    g_last_signal.rms  = 0.0;
    g_last_signal.min  = 0.0;
    g_last_signal.max  = 0.0;

    printf("\nStored %d samples successfully.\n", g_last_signal.count);
}
static int sa_load_from_file_generic(const char *filename) /* Generic loading function for Tool (Menu 7): reads samples from any filename*/    
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("\nError: Could not open '%s'. "
               "Make sure the file exists in the same folder as the program.\n",
               filename);
        return 0;
    }

    double value = 0.0;
    int count = 0;

    while (count < MAX_SAMPLES && fscanf(fp, "%lf", &value) == 1) {
        g_last_signal.samples[count] = value;
        count++;
    }

    fclose(fp);

    if (count == 0) {
        printf("\nNo valid samples were found in '%s'.\n", filename);
        g_last_signal.count = 0;
        return 0;
    }

    g_last_signal.count = count;

    /* Clear old statistics to avoid confusion; the actual calculation is performed within `sa_calculate_stats`. */
    g_last_signal.mean = 0.0;
    g_last_signal.rms  = 0.0;
    g_last_signal.min  = 0.0;
    g_last_signal.max  = 0.0;

    printf("\nLoaded %d samples from '%s'.\n", count, filename);

    return 1;
}
void sa_load_from_file(void)         /* Read from sample.txt */
{
    const char *filename = "samples.txt";

    if (sa_load_from_file_generic(filename)) {
        printf("You can now choose option 3 to calculate statistics.\n");
    }
}
void sa_calculate_stats(void)
{
    int n = g_last_signal.count;

    /* 1. Check whether there are any samples */
    if (n <= 0) {
        printf("\nNo samples available. "
               "Please enter samples (option 1) or load from file (option 2) first.\n");
        return;
    }

    double sum     = 0.0;
    double sum_sq  = 0.0;
    double min_val = g_last_signal.samples[0];
    double max_val = g_last_signal.samples[0];

    /* 2. cumulatively summing and squaring the sums, whilst updating the minimum/maximum values */
    for (int i = 0; i < n; i++) {
        double x = g_last_signal.samples[i];

        sum    += x;
        sum_sq += x * x;

        if (x < min_val) min_val = x;
        if (x > max_val) max_val = x;
    }

    /* 3. Calculate the mean and RMS */
    double mean = sum / n;
    double rms  = sqrt(sum_sq / n);

    /* 4. Save back to global structure */
    g_last_signal.mean = mean;
    g_last_signal.rms  = rms;
    g_last_signal.min  = min_val;
    g_last_signal.max  = max_val;

    /* 5. Printed output */
    printf("\nSignal statistics:\n");
    printf("  Count : %d\n", n);
    printf("  Min   : %.6f\n", min_val);
    printf("  Max   : %.6f\n", max_val);
    printf("  Mean  : %.6f\n", mean);
    printf("  RMS   : %.6f\n", rms);
}
void sa_plot_graph(void)
{
    const int MAX_BAR = 20;  /* 20 squares in each of the negative and positive directions */

    int n = g_last_signal.count;
    if (n <= 0) {
        printf("\nNo samples available. "
               "Please enter samples (option 1) or load from file (option 2) first.\n");
        return;
    }

    /* 1. Find the maximum absolute value for scaling purposes. */
    double max_abs = 0.0;
    for (int i = 0; i < n; i++) {
        double v = fabs(g_last_signal.samples[i]);
        if (v > max_abs) {
            max_abs = v;
        }
    }

    if (max_abs == 0.0) {
        printf("\nAll samples are zero. Nothing to plot.\n");
        return;
    }

    printf("\nASCII Signal Plot (negative left, positive right)\n\n");

    /* 2. Draw line for each sample */
    for (int i = 0; i < n; i++) {
        double x = g_last_signal.samples[i];

        /* Mapping to integer lengths ranging from 0 to MAX_BAR based on absolute value magnitude */
        int left_bar  = 0;
        int right_bar = 0;

        if (x < 0) {
            left_bar = (int)((fabs(x) / max_abs) * MAX_BAR + 0.5);   /* 四舍五入 */
            if (left_bar > MAX_BAR) left_bar = MAX_BAR;
        } else if (x > 0) {
            right_bar = (int)((x / max_abs) * MAX_BAR + 0.5);
            if (right_bar > MAX_BAR) right_bar = MAX_BAR;
        } else {
            /* x == 0，no action */
        }

        printf("%3d: ", i + 1);

        int spaces = MAX_BAR - left_bar;
        for (int s = 0; s < spaces; s++) {
            putchar(' ');
        }
        for (int j = 0; j < left_bar; j++) {
            putchar('#');
        }

        putchar('|');

        for (int j = 0; j < right_bar; j++) {
            putchar('#');
        }
        for (int s = right_bar; s < MAX_BAR; s++) {
            putchar(' ');
        }

        printf("  (% .6f)\n", x);
    }
}
void sa_save_stats_to_file(void)
{
    const char *filename = "signal_stats.txt";

    if (g_last_signal.count <= 0) {
        printf("\nNo samples available. "
               "Please enter or load samples first.\n");
        return;
    }

    /* Ensure that statistical data is up to date */
    sa_calculate_stats();

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("\nError: Could not open '%s' for writing.\n", filename);
        return;
    }

    fprintf(fp, "Signal statistics\n");
    fprintf(fp, "Count = %d\n",  g_last_signal.count);
    fprintf(fp, "Min   = %.6f\n", g_last_signal.min);
    fprintf(fp, "Max   = %.6f\n", g_last_signal.max);
    fprintf(fp, "Mean  = %.6f\n", g_last_signal.mean);
    fprintf(fp, "RMS   = %.6f\n", g_last_signal.rms);

    fclose(fp);

    printf("\nStatistics saved to '%s'.\n", filename);
}

/* ========================== *
 * 5. Unit Converter Module   *
 * ========================== */

/* Provides several common engineering unit conversions:   *
 *  1) dBm ↔ mW     (RF / power)                           *
 *  2) Hz  ↔ rad/s  (frequency ↔ angular frequency)        *
 *  3) °C  ↔ K      (absolute temperature)                 *
 *  4) °F  ↔ °C     (temperature between scales)           *
 *  5) V   ↔ dBV    (voltage level in dBV reference)       */

void menu_unit_converter(void)
{
    int running = 1;

    while (running) {
        uc_print_menu();
        int choice = uc_get_choice();

        switch (choice) {
            case 1: { /* dBm <-> mW */
                printf("\n1) dBm -> mW\n2) mW -> dBm\n");
                int dir = uc_get_choice();
                if (dir == 1) {                        /* dBm to mW: P(mW) = 10^(dBm/10) */
                    double dbm = uc_get_double("Enter power in dBm: ");
                    double mw  = pow(10.0, dbm / 10.0);
                    printf("Result: %.6f mW\n", mw);
                } else if (dir == 2) {                 /* mW to dBm: dBm = 10·log10(P(mW)), P > 0 */
                    double mw = 0.0;
                    do {
                        mw = uc_get_double("Enter power in mW (> 0): ");
                        if (mw <= 0.0) {
                            printf("Power must be > 0 for dBm conversion.\n");
                        }
                    } while (mw <= 0.0);
                    double dbm = 10.0 * log10(mw);
                    printf("Result: %.6f dBm\n", dbm);
                }
                break;
            }
            case 2: { /* Hz <-> rad/s */
                printf("\n1) Hz -> rad/s\n2) rad/s -> Hz\n");
                int dir = uc_get_choice();
                if (dir == 1) {                            /* f [Hz] → ω [rad/s]: ω = 2πf */
                    double hz = uc_get_double("Enter frequency in Hz: ");
                    double rad = 2.0 * M_PI * hz;
                    printf("Result: %.6f rad/s\n", rad);
                } else if (dir == 2) {                     /* ω [rad/s] → f [Hz]: f = ω / (2π) */
                    double rad = uc_get_double("Enter angular frequency in rad/s: ");
                    double hz = rad / (2.0 * M_PI);
                    printf("Result: %.6f Hz\n", hz);
                }
                break;
            }
            case 3: { /* °C <-> K */
                printf("\n1) °C -> K\n2) K -> °C\n");
                int dir = uc_get_choice();
                if (dir == 1) {                        /* °C to K: K = °C + 273.15 */
                    double c = uc_get_double("Enter temperature in °C: ");
                    double k = c + 273.15;
                    printf("Result: %.2f K\n", k);
                } else if (dir == 2) {                 /* K to °C: °C = K - 273.15 */
                    double k = uc_get_double("Enter temperature in K: ");
                    double c = k - 273.15;
                    printf("Result: %.2f °C\n", c);
                }
                break;
            }
            case 4: { /* °F <-> °C */
                printf("\n1) °F -> °C\n2) °C -> °F\n");
                int dir = uc_get_choice();
                if (dir == 1) {                           /* °F to °C: °C = (°F − 32)·5/9 */
                    double f = uc_get_double("Enter temperature in °F: ");
                    double c = (f - 32.0) * 5.0 / 9.0;
                    printf("Result: %.2f °C\n", c);
                } else if (dir == 2) {                    /* °C to °F: °F = °C·9/5 + 32 */
                    double c = uc_get_double("Enter temperature in °C: ");
                    double f = (c * 9.0 / 5.0) + 32.0;
                    printf("Result: %.2f °F\n", f);
                }
                break;
            }
            case 5: { /* V <-> dBV */
                printf("\n1) V -> dBV\n2) dBV -> V\n");
                int dir = uc_get_choice();
                if (dir == 1) {                          /* V to dBV: dBV = 20·log10(V), V > 0 */
                    double v = 0.0;
                    do {
                        v = uc_get_double("Enter voltage in V (> 0): ");
                        if (v <= 0.0) {
                            printf("Voltage must be > 0 for dBV conversion.\n");
                        }
                    } while (v <= 0.0);
                    double dbv = 20.0 * log10(v);
                    printf("Result: %.6f dBV\n", dbv);
                } else if (dir == 2) {                   /* dBV to V: V = 10^(dBV/20) */
                    double dbv = uc_get_double("Enter level in dBV: ");
                    double v = pow(10.0, dbv / 20.0);
                    printf("Result: %.6f V\n", v);
                }
                break;
            }
            case 6:
                /* Back to main menu */
                running = 0;
                break;
            default:
                printf("Unknown option.\n");
                break;
        }
    }
}
static void uc_print_menu(void)     /* Print the Unit Converter submenu options. */
{
    printf("\n------ Unit Converter ------\n");
    printf("1. dBm <-> mW\n");
    printf("2. Hz  <-> rad/s\n");
    printf("3. °C  <-> K\n");
    printf("4. °F  <-> °C\n");
    printf("5. V   <-> dBV\n");
    printf("6. Back to Main Menu\n");
    printf("----------------------------\n");
}
static int uc_get_choice(void)      /* The same integer-validation logic as the Signal Analyzer */
{
    char buf[64];
    int value = 0;

    for (;;) {
        printf("Select option: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\nInput error, returning.\n");
            return 6;
        }
        buf[strcspn(buf, "\r\n")] = '\0';

        if (!sa_is_integer(buf)) {
            printf("Please enter an integer!\n");
            continue;
        }

        value = (int)strtol(buf, NULL, 10);
        return value;
    }
}
static double uc_get_double(const char *prompt) /* Prompts the user for a floating-point value and validates that the entire input line represents a valid double.*/
{
    char buf[64];
    double value = 0.0;

    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\nInput error, using 0.\n");
            return 0.0;
        }
        buf[strcspn(buf, "\r\n")] = '\0';

        char *endptr = NULL;
        value = strtod(buf, &endptr);
        if (endptr == buf || *endptr != '\0') {
            printf("Please enter a valid number.\n");
            continue;
        }
        return value;
    }
}

/* ================================== *
 * 2. ADC / Sensor Converter Module   *
 * ================================== */

/* Simple ADC + temperature sensor helper:                          *
 *  1) - ADC code → Voltage      (using VREF and 10-bit ADC 0–1023) *
 *  2) - Voltage  → Temperature  (LM35 style: 10 mV / °C)           *
 *  3) - ADC code → Temperature  (combines the two steps above)     */
void menu_adc_sensor_converter(void)
{
    const double VREF = 5.0;    /* ADC reference voltage, e.g. 3.3 V or 5.0 V */

    int running = 1;

    while (running) {
        printf("\n------ ADC / Sensor Converter ------\n");
        printf("1. ADC code -> Voltage\n");
        printf("2. Voltage  -> Temperature (°C)\n");
        printf("3. ADC code -> Temperature (°C)\n");
        printf("4. Back to Main Menu\n");
        printf("------------------------------------\n");

        int choice = uc_get_choice();      /* Reuse the integer choice helper from the Unit Converter. */

        switch (choice) {

            /* --- 1. ADC -> Voltage --- */
            case 1: {
                int adc = (int)uc_get_double("Enter ADC code (0–1023): ");       /* Assume a 10-bit ADC, codes 0–1023 inclusive. */
                if (adc < 0 || adc > 1023) {
                    printf("ADC code must be 0–1023.\n");
                    break;
                }
                double voltage = (adc / 1023.0) * VREF;      /* V = (code / 1023) * VREF */
                printf("Voltage = %.4f V\n", voltage);
                break;
            }

            /* --- 2. Voltage -> Temperature --- */
            case 2: {
                double volt = uc_get_double("Enter voltage in V: ");
                if (volt < 0) {
                    printf("Voltage must be >= 0.\n");
                    break;
                }                                /* LM35 model: 10mV/°C at 0 °C */
                double temp = volt * 100.0;      /* 10 mV/°C => 0.01 V / °C */
                printf("Temperature = %.2f °C\n", temp);
                break;
            }

            /* --- 3. ADC -> Temperature --- */
            case 3: {
                int adc = (int)uc_get_double("Enter ADC code (0–1023): ");
                if (adc < 0 || adc > 1023) {
                    printf("ADC code must be 0–1023.\n");
                    break;
                }                                       /* First convert ADC code -> voltage, then voltage -> °C */
                double voltage = (adc / 1023.0) * VREF;
                double temp = voltage * 100.0;  /* 10mV/°C */
                printf("Voltage = %.4f V\n", voltage);
                printf("Temperature = %.2f °C\n", temp);
                break;
            }

            case 4:            /* Back to Main Menu */
                running = 0;
                break;

            default:
                printf("Unknown option.\n");
                break;
        }
    }
}

/* ============================== *
 * 3. Circuit Calculator Module   *
 * ============================== */

/* A small collection of analogue circuit formula helpers:     *
 *  1) RC filter cutoff frequency   fc = 1/(2πRC)              *
 *  2) Resistive voltage divider    Vout = Vin * R2/(R1+R2)    *
 *  3) LED current-limit resistor   R = (Vs − Vf) / If         *
 *  4) Ohm's law helper             V = I·R, I = V/R, R = V/I  *
 *  5) Capacitor charging curve     V(t) = Vs(1 − e^(-t/RC))   */
void menu_circuit_calculator(void)
{
    int running = 1;

    while (running) {
        printf("\n------ Circuit Calculator ------\n");
        printf("1. RC filter cutoff frequency\n");
        printf("2. Voltage divider\n");
        printf("3. LED current-limit resistor\n");
        printf("4. Ohm's law (V = I * R)\n");
        printf("5. Capacitor charging V(t)\n");
        printf("6. Back to Main Menu\n");
        printf("--------------------------------\n");

        int choice = uc_get_choice();

        switch (choice) {
            case 1: cc_rc_filter();       break;
            case 2: cc_voltage_divider(); break;
            case 3: cc_led_resistor();    break;
            case 4: cc_ohms_law();        break;
            case 5: cc_cap_charging();    break;
            case 6: running = 0;          break;
            default:
                printf("Unknown option.\n");
                break;
        }
    }
}

/* ================================= *
 * 3. Circuit Calculator Funcitions  *
 * ================================= */
static void cc_rc_filter(void)  /* Inputs R (ohms) and C (microfarads), converted to farads then calculating fc */
{
    printf("\n[RC low-pass filter cutoff]\n");
    double R = 0.0;
    double C_uF = 0.0;

    R = uc_get_double("Enter R in ohms: ");
    C_uF = uc_get_double("Enter C in microfarads (uF): ");

    if (R <= 0.0 || C_uF <= 0.0) {
        printf("R and C must be > 0.\n");
        return;
    }

    double C = C_uF * 1e-6;  /* uF -> F */
    double fc = 1.0 / (2.0 * M_PI * R * C);

    printf("Cutoff frequency fc = %.3f Hz\n", fc);
}
static void cc_voltage_divider(void)  /* Two-resistor voltage divider */
{
    printf("\n[Voltage divider]\n");

    double Vin = uc_get_double("Enter Vin (V): ");
    double R1  = uc_get_double("Enter R1 (ohms): ");
    double R2  = uc_get_double("Enter R2 (ohms): ");

    if (R1 <= 0.0 || R2 <= 0.0) {
        printf("R1 and R2 must be > 0.\n");
        return;
    }

    double Vout = Vin * (R2 / (R1 + R2));

    printf("Vout = %.4f V\n", Vout);
    /* Simultaneously output the ratio of Vout to Vout/Vin. */
    printf("Divider ratio = Vout/Vin = %.4f\n", (Vin != 0.0) ? (Vout / Vin) : 0.0);
}
static void cc_led_resistor(void)  /* Calculate THE LED Current-Limiting Resistor */
{
    printf("\n[LED current-limit resistor]\n");

    double Vs = uc_get_double("Enter supply voltage Vs (V): ");
    double Vf = uc_get_double("Enter LED forward voltage Vf (V): ");
    double If_mA = uc_get_double("Enter desired LED current If (mA): ");

    double If = If_mA / 1000.0;  /* mA -> A */

    if (If <= 0.0) {
        printf("Current must be > 0.\n");
        return;
    }

    double Vr = Vs - Vf;
    if (Vr <= 0.0) {
        printf("Vs must be greater than Vf.\n");
        return;
    }

    double R = Vr / If;

    printf("Required series resistor R = %.2f ohms\n", R);
}
static void cc_ohms_law(void) /* Simple Ohms Law calculator :) */
{
    printf("\n[Ohm's law]\n");
    printf("1. Solve V (given I and R)\n");
    printf("2. Solve I (given V and R)\n");
    printf("3. Solve R (given V and I)\n");

    int mode = uc_get_choice();

    if (mode == 1) {
        double I = uc_get_double("Enter current I (A): ");
        double R = uc_get_double("Enter resistance R (ohms): ");
        double V = I * R;
        printf("V = %.4f V\n", V);
    } else if (mode == 2) {
        double V = uc_get_double("Enter voltage V (V): ");
        double R = uc_get_double("Enter resistance R (ohms): ");
        if (R == 0.0) {
            printf("R must not be 0.\n");
            return;
        }
        double I = V / R;
        printf("I = %.6f A\n", I);
    } else if (mode == 3) {
        double V = uc_get_double("Enter voltage V (V): ");
        double I = uc_get_double("Enter current I (A): ");
        if (I == 0.0) {
            printf("I must not be 0.\n");
            return;
        }
        double R = V / I;
        printf("R = %.2f ohms\n", R);
    } else {
        printf("Invalid mode.\n");
    }
}
static void cc_cap_charging(void) /* Cap Charging: V(t) = Vs * (1 − e^(−t/RC)) */
{
    printf("\n[Capacitor charging curve]\n");

    double Vs   = uc_get_double("Enter supply voltage Vs (V): ");
    double R    = uc_get_double("Enter R (ohms): ");
    double C_uF = uc_get_double("Enter C (microfarads, uF): ");
    double t    = uc_get_double("Enter time t (seconds): ");

    if (R <= 0.0 || C_uF <= 0.0 || t < 0.0) {
        printf("R, C must be > 0 and t >= 0.\n");
        return;
    }

    double C  = C_uF * 1e-6;   /* uF -> F */
    double tau = R * C;        /* time constant RC */

    double Vt = Vs * (1.0 - exp(-t / tau));

    printf("Time constant tau = R*C = %.6f s\n", tau);
    printf("V(t) = %.4f V at t = %.4f s\n", Vt, t);
}

/* ================================== *
 * 4. Resistor Colour Decoder Module  *
 * ================================== */
void menu_resistor_colour_decoder(void)       /* Submenu */
{
    int running = 1;

    while (running) {
        printf("\n[Resistor Colour Decoder]\n");
        printf("1. Decode 4-band resistor\n");
        printf("2. Decode 5-band resistor\n");
        printf("3. Back to Main Menu\n");
        printf("---------------------------\n");

        int choice = uc_get_choice();

        switch (choice) {
            case 1:
                resistor_decode_4band();
                break;
            case 2:
                resistor_decode_5band();
                break;
            case 3:
                running = 0;
                break;
            default:
                printf("Unknown option.\n");
                break;
        }
    }
}

/* ===================================== *
 * 4. Resistor Colour Decoder Functions  *
 * ===================================== */
static void resistor_decode_4band(void)       /* 4 band resistor */
{
    char b1[32], b2[32], b3[32], b4[32];

    printf("\n[4-band resistor colour decoder]\n");
    printf("Supported colours:\n");
    printf("  black, brown, red, orange, yellow, green, blue,\n");
    printf("  violet, grey, white, gold, silver (tolerance only)\n\n");

    /* Read 4 band colours */
    printf("Enter band 1 colour (first digit): ");
    if (!fgets(b1, sizeof(b1), stdin)) return;
    printf("Enter band 2 colour (second digit): ");
    if (!fgets(b2, sizeof(b2), stdin)) return;
    printf("Enter band 3 colour (multiplier): ");
    if (!fgets(b3, sizeof(b3), stdin)) return;
    printf("Enter band 4 colour (tolerance): ");
    if (!fgets(b4, sizeof(b4), stdin)) return;

    /* Remove line breaks and convert to lowercase / 去掉换行并转小写 */
    b1[strcspn(b1, "\r\n")] = '\0';
    b2[strcspn(b2, "\r\n")] = '\0';
    b3[strcspn(b3, "\r\n")] = '\0';
    b4[strcspn(b4, "\r\n")] = '\0';

    str_to_lower(b1);
    str_to_lower(b2);
    str_to_lower(b3);
    str_to_lower(b4);

    int d1 = resistor_digit_from_color(b1);
    int d2 = resistor_digit_from_color(b2);
    double mult = 0.0;
    double tol  = 0.0;

    if (d1 < 0 || d2 < 0) {
        printf("Error: invalid colour in first or second band.\n");
        return;
    }
    if (resistor_multiplier_from_color(b3, &mult) != 0) {
        printf("Error: invalid multiplier colour.\n");
        return;
    }
    if (resistor_tolerance_from_color(b4, &tol) != 0) {
        printf("Error: invalid tolerance colour.\n");
        return;
    }

    int base = d1 * 10 + d2;       /* The first two digits */
    double R = base * mult;        /* OHM */

    double display_value = R;
    const char *unit = "Ω";

    if (R >= 1e6) {
        display_value = R / 1e6;
        unit = "MΩ";
    } else if (R >= 1e3) {
        display_value = R / 1e3;
        unit = "kΩ";
    }

    printf("\nResult (4-band):\n");
    printf("  Nominal resistance: %.3f %s\n", display_value, unit);
    printf("  Tolerance: ±%.2f %%\n", tol);
}
static void resistor_decode_5band(void)       /* 5 band resistor */
{
    char b1[32], b2[32], b3[32], b4[32], b5[32];

    printf("\n[5-band resistor colour decoder]\n");
    printf("Supported colours (digits & multiplier):\n");
    printf("  black, brown, red, orange, yellow, green, blue,\n");
    printf("  violet, grey, white, gold, silver (mult/tolerance)\n\n");

    /* Read 5 band colours */
    printf("Enter band 1 colour (first digit): ");
    if (!fgets(b1, sizeof(b1), stdin)) return;
    printf("Enter band 2 colour (second digit): ");
    if (!fgets(b2, sizeof(b2), stdin)) return;
    printf("Enter band 3 colour (third digit): ");
    if (!fgets(b3, sizeof(b3), stdin)) return;
    printf("Enter band 4 colour (multiplier): ");
    if (!fgets(b4, sizeof(b4), stdin)) return;
    printf("Enter band 5 colour (tolerance): ");
    if (!fgets(b5, sizeof(b5), stdin)) return;

    /* Remove line breaks and convert to lowercase / 去掉换行并转小写 */
    b1[strcspn(b1, "\r\n")] = '\0';
    b2[strcspn(b2, "\r\n")] = '\0';
    b3[strcspn(b3, "\r\n")] = '\0';
    b4[strcspn(b4, "\r\n")] = '\0';
    b5[strcspn(b5, "\r\n")] = '\0';

    str_to_lower(b1);
    str_to_lower(b2);
    str_to_lower(b3);
    str_to_lower(b4);
    str_to_lower(b5);

    int d1 = resistor_digit_from_color(b1);
    int d2 = resistor_digit_from_color(b2);
    int d3 = resistor_digit_from_color(b3);
    double mult = 0.0;
    double tol  = 0.0;

    if (d1 < 0 || d2 < 0 || d3 < 0) {
        printf("Error: invalid colour in first, second or third band.\n");
        return;
    }
    if (resistor_multiplier_from_color(b4, &mult) != 0) {
        printf("Error: invalid multiplier colour.\n");
        return;
    }
    if (resistor_tolerance_from_color(b5, &tol) != 0) {
        printf("Error: invalid tolerance colour.\n");
        return;
    }

    int base = d1 * 100 + d2 * 10 + d3;    /* 5-band: Three digits */
    double R = base * mult;                /* OHM */

    double display_value = R;
    const char *unit = "Ω";

    if (R >= 1e6) {
        display_value = R / 1e6;
        unit = "MΩ";
    } else if (R >= 1e3) {
        display_value = R / 1e3;
        unit = "kΩ";
    }

    printf("\nResult (5-band):\n");
    printf("  Nominal resistance: %.3f %s\n", display_value, unit);
    printf("  Tolerance: ±%.2f %%\n", tol);
}
static void str_to_lower(char *s)    /* converts a C-string to lowercase in-place */
{         
    while (*s) {
        *s = (char)tolower((unsigned char)*s);
        s++;
    }
}
static int resistor_digit_from_color(const char *s) /* Maps a colour name to its corresponding digit 0–9 */
{
    if      (strcmp(s, "black")  == 0) return 0;
    else if (strcmp(s, "brown")  == 0) return 1;
    else if (strcmp(s, "red")    == 0) return 2;
    else if (strcmp(s, "orange") == 0) return 3;
    else if (strcmp(s, "yellow") == 0) return 4;
    else if (strcmp(s, "green")  == 0) return 5;
    else if (strcmp(s, "blue")   == 0) return 6;
    else if (strcmp(s, "violet") == 0) return 7;
    else if (strcmp(s, "grey")   == 0 || strcmp(s, "gray") == 0) return 8;
    else if (strcmp(s, "white")  == 0) return 9;
    return -1; /* invalid */
}
static int resistor_multiplier_from_color(const char *s, double *mult)  /* Maps a colour name to its multiplier (×10^n) as a double */
{
    if      (strcmp(s, "black")  == 0) *mult = 1.0;
    else if (strcmp(s, "brown")  == 0) *mult = 10.0;
    else if (strcmp(s, "red")    == 0) *mult = 100.0;
    else if (strcmp(s, "orange") == 0) *mult = 1e3;
    else if (strcmp(s, "yellow") == 0) *mult = 1e4;
    else if (strcmp(s, "green")  == 0) *mult = 1e5;
    else if (strcmp(s, "blue")   == 0) *mult = 1e6;
    else if (strcmp(s, "violet") == 0) *mult = 1e7;
    else if (strcmp(s, "grey")   == 0 || strcmp(s, "gray") == 0) *mult = 1e8;
    else if (strcmp(s, "white")  == 0) *mult = 1e9;
    else if (strcmp(s, "gold")   == 0) *mult = 0.1;
    else if (strcmp(s, "silver") == 0) *mult = 0.01;
    else return -1;
    return 0;
}
static int resistor_tolerance_from_color(const char *s, double *tol)  /* Maps a colour name to its tolerance percentage */
{
    /* Percentage */
    if      (strcmp(s, "brown")  == 0) *tol = 1.0;
    else if (strcmp(s, "red")    == 0) *tol = 2.0;
    else if (strcmp(s, "green")  == 0) *tol = 0.5;
    else if (strcmp(s, "blue")   == 0) *tol = 0.25;
    else if (strcmp(s, "violet") == 0) *tol = 0.10;
    else if (strcmp(s, "grey")   == 0 || strcmp(s, "gray") == 0) *tol = 0.05;
    else if (strcmp(s, "gold")   == 0) *tol = 5.0;
    else if (strcmp(s, "silver") == 0) *tol = 10.0;
    else return -1;
    return 0;
}

/* ================================= *
 * 6. Engineering Calculator Module  *
 * ================================= */
void menu_engineering_calculator(void)
{
    printf("\n[Engineering Calculator]\n");
    printf("This feature slot is reserved for a future DLC.\n"); 
    printf("For general arithmetic and scientific functions,\n");
    printf("please use a normal calculator.\n");
    printf("This project focuses on signal and circuit tools.\n");
}
/* ======================================================================= *
 *   This feature has been delayed......again.                             *
 *   Release date: TBD (just like GTA 6 before 2025).                      *
 *                                                                         *
 *              ▄██████▄      ╔═╗┬ ┬┌─┐┬┌─                                 *
 *             ███    ███     ║═╬└┬┘├─┤├┴┐  (Coming Soon™)                 *   
 *             ███    ███     ╚═╝ ┴ ┴ ┴┴ ┴                                 *
 *             ███    ███                                                  * 
 *             ▀██████▀                                                    *
 *                                                                         *
 *   Please enjoy other fully implemented modules while waiting :)         *
 *                                                                         *
 *     /\_/\     Oh my, there's a lovely little kitten here.               *
 *    ( o.o )    It's the companion we shipped pre-DLC.                    *
 *     > ^ <     And littele kitten believes in your math skills.          * 
 *                                                                         *
 *   If all else fails, you can always team up with our other legendary    *
 *   companion the Casio FX-991EX, to tackle the challenges!               *
 *                                                                         *
 * ======================================================================= */


/* ============================================================ *
 * 7. Tools (Auto Tests & File Operations for Signal Analyzer)  *
 * ============================================================ */
void menu_tools(void)
{
    int running = 1;

    while (running) {
        printf("\n------ Tools (Auto Tests & Files for Signal Analyzer) ------\n");
        printf("1. Generate sample file for Signal Analyzer\n");
        printf("2. Auto-test Signal Analyzer (load + stats + graph)\n");
        printf("3. Back to Main Menu\n");
        printf("----------------------------------------\n");

        int choice = uc_get_choice();

        switch (choice) {
            case 1:
                tools_generate_sample_file();
                break;
            case 2:
                tools_run_auto_test();
                break;
            case 3:
                running = 0;
                break;
            default:
                printf("Unknown option.\n");
                break;
        }
    }
}
static void tools_generate_sample_file(void)      /* Generate a file with simple test data, 5 samples already put in */
{
    const char *filename = "samples_auto.txt";

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("\nError: Could not open '%s' for writing.\n", filename);
        return;
    }

    /* put in 5 samples */
    fprintf(fp, "1.0\n");
    fprintf(fp, "2.0\n");
    fprintf(fp, "3.0\n");
    fprintf(fp, "4.0\n");
    fprintf(fp, "5.0\n");

    fclose(fp);

    printf("\n[Tools] Wrote 5 samples to '%s'.\n", filename);
    printf("Values: 1.0, 2.0, 3.0, 4.0, 5.0\n");
    printf("You can now use 'Tools -> Auto-test' to analyse this file.\n");
}
static void tools_save_stats_silent(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: could not open '%s' for writing.\n", filename);
        return;
    }

    fprintf(fp, "Count: %d\n", g_last_signal.count);
    fprintf(fp, "Min: %f\n",  g_last_signal.min);
    fprintf(fp, "Max: %f\n",  g_last_signal.max);
    fprintf(fp, "Mean: %f\n", g_last_signal.mean);
    fprintf(fp, "RMS: %f\n",  g_last_signal.rms);

    fclose(fp);
    printf("Saving results to '%s'... done.\n", filename);
}
static void tools_run_auto_test(void)     /* Auto Test */
{
    const char *filename = "samples_auto.txt";

    printf("\n[Tools] Running Auto-Test on '%s'...\n", filename);

    /* 1. Load sample file */
    if (!sa_load_from_file_generic(filename)) {
        printf("[Tools] Auto-Test aborted (could not load samples).\n");
        return;
    }

    /* 2. Compute the statistics result and then print it */
    printf("\n[Tools] Calculating statistics...\n");
    sa_calculate_stats();

    /* 3. Print ASCII Graph */
    printf("\n[Tools] Drawing ASCII plot...\n");
    sa_plot_graph();

    /* 4. Save the results and inform the user that the save was successful. */
    printf("\n[Tools] Saving results...\n");
    tools_save_stats_silent("signal_stats.txt");

    printf("\n[Tools] Auto-Test complete.\n");
}