/**
 * @file gps_data_parser.h
 * @brief Provides function for parsing GPS GGA sentences and data parameters struct.
 *
 * Created on: 28-Apr-2024
 * Author: Aizaz Ubaid Hashmi
 *
 *
 * This header file defines the structures and functions for parsing GPS GGA sentences
 * and extracting useful information from them.
 *
 */
#ifndef GPS_DATA_PARSER_H
#define GPS_DATA_PARSER_H


/**
 * @brief Structure to store GPS data.
 *
 * This structure contains parsed GPS data from a UART stream, including time,
 * latitude, longitude, and altitude. The time is in the HHMMSS.SSS format,
 * latitude and longitude are in degrees and minutes (DDMM.MMMM format for latitude
 * and DDDMM.MMMM format for longitude), and altitude is above sea level in meters.
 */
typedef struct {
    // Time in HHMMSS.SSS format
    char time[13];

    // Latitude in DDMM.MMMM format
    char latitude[15];

    // Longitude in DDDMM.MMMM format
    char longitude[15];

    // Altitude above sea level in meters
    char altitude[12];
} gps_data_parse_t;

/**
 * @brief  Declaration of function to Parse GPS data from a UART stream.
 *
 * This function takes a UART stream containing GPS data in a specific format
 * and parses it to populate the gps_data_parse_t structure. It returns the parsed
 * data structure.
 *
 * @param uart_stream The input UART stream from GPS module as NMEA sentences.
 *
 * @return Parsed GPS data in a gps_data_parse_t structure.
 */
gps_data_parse_t gps_data_parser(const char * uart_stream);

#endif  // GPS_DATA_PARSER_H
