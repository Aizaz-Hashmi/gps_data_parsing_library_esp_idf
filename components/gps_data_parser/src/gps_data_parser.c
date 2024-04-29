/**
 * @file gps_data_parser.c
 * @brief Parses GPS data packets, extracting relevant information (time, latitude, longitude, altitude, etc.).
 *
 * Processes, validates, and extracts data from NMEA 0183 sentences sent via UART from a GPS module to an ESP32 MCU.
 *
 * Created on: 28-Apr-2024
 * Author: Aizaz Ubaid Hashmi
 */

#include <stdio.h>
#include "gps_data_parser.h"

static int check_stream_validity(const char *uart_stream);

/**
 * @brief Parses a UART stream to extract GPS data.
 *
 * @param uart_stream The input string containing GPS data.
 * @return A structure containing parsed GPS data.
 */
gps_data_parse_t gps_data_parser(const char * uart_stream)
{
    gps_data_parse_t gps_data;

    // Check if the UART stream is valid
    if (!(check_stream_validity(uart_stream)))
    {
        // process stream if it is not null or empty
    }

    else
    {
    	 // The stream is invalid (either NULL or empty), so return default GPS data
    	 //use sprintf function to store string to give parameters as string
    	  sprintf(gps_data.time, "N/A");
    	  sprintf(gps_data.latitude, "N/A");   // here any string can be added as per preference like empty string or zeros
    	  sprintf(gps_data.longitude, "N/A");
    	  sprintf(gps_data.altitude, "N/A");
    }

    // Return the GPS data structure (either populated or default)
    return gps_data;
}

/**
 * @brief Checks the validity of a UART stream.
 *
 * @param uart_stream The UART stream to check.
 * @return Returns 1 if the stream is invalid (NULL or empty), otherwise 0.
 */
int check_stream_validity(const char *uart_stream)
{
    // Return 1 if the stream is NULL, otherwise return 0
    return (uart_stream == NULL) ? 1 : 0;
}
