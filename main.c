// ELEC2645 Unit 2 Embedded Systems Project -- Engineering Toolkit
// Designed by Qi Chang, Student id：201891723
// Last updated： 03/12/2025

/*
 *                     Toolkit Functions Description:
 * This application is a menu-driven command line “Engineering toolkit”
 * It provides a collection of small utilities for signals and basic circuit calculations.
 * The main program displays a top-level menu, validates the user’s choice, 
 * and dispatches to the corresponding module implemented in funcs.c.
 * 
 * The toolkit includes:
 *     - Signal Analyzer: manual / file input, statistics, ASCII plots,
 *       and saving results to a text file.
 * 
 *     - ADC / Sensor Converter: converts ADC codes to voltage and
 *       temperature using a simple linear sensor model.
 * 
 *     - Circuit Calculator: RC cutoff frequency, voltage divider,
 *       LED current-limit resistor, Ohm's law and capacitor charging.
 * 
 *     - Resistor Colour Decoder: decodes 4-band resistor values.
 * 
 *     - Unit Converter: common engineering unit conversions
 *       (dBm↔mW, Hz↔rad/s, °C↔K, °F↔°C, V↔dBV).
 * 
 *     - Tools (Auto Tests & File Operations for Signal Analyzer):
 *       generates sample data and runs an automated test pipeline
 *       for the signal-analysis functions and save the data.
 */

/* This file contains the main() function and the top-level menu handling code. *
 * Each menu item calls into functions implemented in funcs.c.                  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "funcs.h"

/* ------------ Local function prototypes (main.c only) ------------ */
static void main_menu(void);              /* Display and handle main menu */
static void print_main_menu(void);        /* Print main menu text */
static int  get_user_input(void);         /* Read & validate integer choice */
static void select_menu_item(int input);  /* Dispatch to selected module  */
static void go_back_to_main(void);        /* Wait for 'b' / 'B' to continue */
static int  is_integer(const char *s);    /* Check if string is an integer */

/*                        Program entry point.                           *
 * Runs an infinite loop that repeatedly shows the main menu and         *
 * processes the user’s selection until the user chooses the Exit option */
int main(void)
{
    /* Run forever until the user selects "Exit" in the main menu */
    for(;;) {
        main_menu();
    }
    /* not reached, but kept for completeness */
    return 0;
}

/*                           Main Menu                                      *
 * RPrints the main menu and obtains a validated integer selection from the *
 * user, then passes it to select_menu_item() to run the chosen module.     */
static void main_menu(void)
{
    print_main_menu();
    {
        int input = get_user_input();
        select_menu_item(input);
    }
}

/* Get User's inputs */
static int get_user_input(void)
{
    enum { MENU_ITEMS = 8 };   /* 1..7 = items, 8 = Exit */
    char buf[128];
    int valid_input = 0;
    int value = 0;

    do {
        printf("\nSelect item: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            /* EOF or error; bail out gracefully */
            puts("\nInput error. Exiting.");
            exit(1);
        }

        /* Strip trailing newline characters */
        buf[strcspn(buf, "\r\n")] = '\0';

        if (!is_integer(buf)) {
            printf("Enter an integer!\n");
            valid_input = 0;
        } else {
            value = (int)strtol(buf, NULL, 10);
            if (value >= 1 && value <= MENU_ITEMS) {
                valid_input = 1;
            } else {
                printf("Invalid menu item!\n");
                valid_input = 0;
            }
        }
    } while (!valid_input);

    return value;
}

/* Menu selecting： 1–7: Run corresponding menu items / 8: Exit */
static void select_menu_item(int input)
{
    switch (input) {
        case 1:
            /* 1. Signal Analyzer */
            menu_signal_analyzer();
            go_back_to_main();
            break;

        case 2:
            /* 2. ADC / Sensor Converter */
            menu_adc_sensor_converter();
            go_back_to_main();
            break;

        case 3:
            /* 3. Circuit Calculator */
            menu_circuit_calculator();
            go_back_to_main();
            break;

        case 4:
            /* 4. Resistor Colour Decoder */
            menu_resistor_colour_decoder();
            go_back_to_main();
            break;

        case 5:
            /* 5. Unit Converter */
            menu_unit_converter();
            go_back_to_main();
            break;

        case 6:
            /* 6. Engineering Calculator */
            menu_engineering_calculator();
            go_back_to_main();
            break;

        case 7:
            /* 7. Tools (Auto Tests & File Ops) */
            menu_tools();
            go_back_to_main();
            break;

        default:
            /* 8. Exit */
            printf("Bye!\n");
            exit(0);
    }
}

/* Prints the main menu text */
static void print_main_menu(void)
{
    printf("\n----------- Main Menu -----------\n");
    printf(
           "\t1. Signal Analyzer\n"
           "\t2. ADC / Sensor Converter\n"
           "\t3. Circuit Calculator\n"
           "\t4. Resistor Colour Decoder\n"
           "\t5. Unit Converter\n"
           "\t6. Engineering Calculator\n"
           "\t7. Tools (Auto Tests & File Operations for Signal Analyzer)\n"
           "\t8. Exit\n"
    );
    printf("---------------------------------\n");
}

/* Exit only after the user has entered “b” or "B" */
static void go_back_to_main(void)
{
    char buf[64];
    do {
        printf("\nEnter 'b' or 'B' to go back to main menu: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            puts("\nInput error. Exiting.");
            exit(1);
        }
        buf[strcspn(buf, "\r\n")] = '\0'; /* strip newline */
    } while (!(buf[0] == 'b' || buf[0] == 'B') || buf[1] != '\0');
}

/* Checks whether the given C-string represents a valid integer */
static int is_integer(const char *s)
{
    if (!s || !*s) return 0;

    /* optional sign */
    if (*s == '+' || *s == '-') s++;

    /* must have at least one digit */
    if (!isdigit((unsigned char)*s)) return 0;

    /* Remaining User that characters must all be digits. */
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}
