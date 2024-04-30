/**
 * @file test_gps_data_parser_app.c
 * @brief Example test application for GPS data parsing using the UNITY framework.
 *
 * This file demonstrates how to use the UNITY testing framework to test GPS data
 * parsing functions. It contains functions to execute various test cases and
 * print banners indicating different stages of test execution.
 *
 * @note This code is in the Public Domain (or CC0 licensed, at your option).
 *       Distributed on an "AS IS" basis, without warranties or conditions of any kind.
 */

#include <stdio.h>
#include <string.h>

#include "unity.h"

/**
 * @brief Prints a banner around a given text.
 *
 * @param text The text to be printed within the banner.
 */
static void print_banner(const char* text)
{
    printf("\n#### %s #####\n\n", text);
}

/**
 * @brief The main function where the application logic starts.
 *
 * This function executes different test cases using the UNITY testing framework,
 * demonstrating various ways to run tests based on specific conditions.
 */
void app_main(void)
{


    // Running all registered tests
    print_banner("Running all registered tests for GPS data parsing");
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();

    // Starting the interactive test menu
    print_banner("Starting interactive GPS data parsing test menu");
    unity_run_menu(); // This function will wait for UART input and not return
}
