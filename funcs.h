// ELEC2645 Unit 2 Embedded Systems Project -- Engineering Toolkit
// Designed by Qi Chang, Student id：201891723
// Last updated： 02/12/2025

/* This header exposes:                                        *
 *   - The shared SignalStats structure used across modules    *
 *   - External access to the global g_last_signal instance    *
 *   - Menu entry-point functions for all application modules  *
 *   - Sub-functions for the Signal Analyzer                   */
/* All implementations are provided in funcs.c and are invoked from main.c based on user selection from the main menu. */

#ifndef FUNCS_H
#define FUNCS_H

#define MAX_SAMPLES 128  /* Maximum number of samples stored/processed */


/* --------- Shared signal data structure --------- */
/* Used by:  Signal Analyzer / Tools (Auto-test module) / Any other module that needs to access recent analysis results */
typedef struct {
    double samples[MAX_SAMPLES];       /* Raw sample array */
    int    count;                      /* Number of valid samples */
    double mean;                       /* Arithmetic mean */
    double rms;                        /* Root-mean-square */
    double min;                        /* Minimum value */
    double max;                        /* Maximum value */
} SignalStats;

/* Global variable holding the MOST RECENT signal-analysis result */
extern SignalStats g_last_signal;  /* Defined in funcs.c, declared here for cross-module use */

/* -------------------- All the main functions for Menu ------------------- */
/* These functions correspond to the 8 main menu items */
void menu_signal_analyzer(void);          /* 1. Signal Analyzer */
void menu_adc_sensor_converter(void);     /* 2. ADC / Sensor Converter */
void menu_circuit_calculator(void);       /* 3. Circuit Calculator */
void menu_resistor_colour_decoder(void);  /* 4. Resistor Colour Decoder */
void menu_unit_converter(void);           /* 5. Unit Converter */
void menu_engineering_calculator(void);   /* 6. Engineering Calculator */
void menu_tools(void);                    /* 7. Tools (Auto tests & File I/O) */

/* -------------------------- Signal Analyzer Submenu ------------------------- */
/* These functions implement the internal options of theSignal Analyzer module. */
void sa_enter_samples(void);              /* Manual entry of sample data */
void sa_load_from_file(void);             /* Load samples from samples.txt */
void sa_calculate_stats(void);            /* Compute mean, RMS, min, max */
void sa_plot_graph(void);                 /* Print ASCII bar graph */
void sa_save_stats_to_file(void);         /* Save results to signal_stats.txt */

#endif /* FUNCS_H */
