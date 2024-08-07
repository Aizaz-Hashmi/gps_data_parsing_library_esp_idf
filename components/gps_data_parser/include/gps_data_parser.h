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

#include <stdint.h>

// Define USE_FEET_UNIT as 1 to convert altitude,Geoid separation to feet, or 0 to use meters
#define USE_FEET_UNIT 0
 

/**
 * @brief Structure to store GPS time.
 */
typedef struct {
    uint8_t hour;      // Hour (00-23) 
    uint8_t minute;    // Minute (00-59) 
    uint8_t second;    // Second (00-59) 
    uint16_t millisecond; // Thousandth's of a second (000-999) 
} gps_time_t;

/**
 * @brief Structure to store parsed GPS data.
 *
 * This structure contains parsed GPS data from a GGA Sentence, including time,
 * latitude, longitude, altitude, fix quality, number of satellites, HDOP, and geoid height.
 * The time is in the HHMMSS.SSS format, latitude and longitude are in degrees and minutes 
 * (DDMM.MMMM format for latitude and DDDMM.MMMM format for longitude), altitude and geoid 
 * height are above sea level in meters, and fix quality, number of satellites, differential age,station ID and HDOP
 * are also provided.
 */
typedef struct {
    // Time in HHMMSS.SSS format from GGA sentence
    gps_time_t time;

    // Latitude in DDMM.MMMM format from GGA sentence 
    float latitude;

    // Latitude direction (N/S)
    char lat_direction;

    // Longitude in DDDMM.MMMM format
    float longitude;

    // Longitude direction (E/W)
    char lon_direction;

    // Fix quality (0 = invalid, 1 = GPS fix, 2 = DGPS fix, 3 = Precise Positioning Service,4 =RTK Integer,5 = RTK Float, 6 = Dead Reckoning)
    int fix_quality;

    // Number of satellites being tracked
    int num_satellites;

    // Horizontal Dilution of Precision
    float hdop;

    // Altitude above sea level in meters
    float altitude;

    // Units of altitude (M) in meters
    char altitude_units;

    // Geoid separation (difference between WGS-84 earth ellipsoid and mean sea level)
    float geoid_height;

    // Units of geoid separation (M) in meters
    char geoid_height_units;
    
    // Age of differential GPS data (seconds), range from 0 to 99
    float dgps_age;

    // Differential reference station ID (numeric), range from 0 to 1023
    int dgps_station_id;

} gps_data_parse_t;

// Set default values when correct data or invalid format or missing
#define DEFAULT_GPS_TIME_HR  255
#define DEFAULT_GPS_TIME_MIN 255
#define DEFAULT_GPS_TIME_SEC 255
#define DEFAULT_GPS_TIME_MS  255
#define DEFAULT_LATITUDE     9999
#define DEFAULT_LAT_DIRECTION '-'
#define DEFAULT_LONGITUDE    9999
#define DEFAULT_LON_DIRECTION '-'
#define DEFAULT_FIX_QUALITY    -1
#define DEFAULT_NUM_SATELLITES -1
#define DEFAULT_HDOP           -1
#define DEFAULT_ALTITUDE    -9999
#define DEFAULT_ALTITUDE_UNITS '-'
#define DEFAULT_GEOID_HEIGHT -9999
#define DEFAULT_GEOID_HEIGHT_UNITS '-'
#define DEFAULT_DGPS_AGE        -1
#define DEFAULT_DGPS_STATION_ID -1

typedef gps_data_parse_t*  gps_gga_handle_t;// create gps handle variable for gga sentence


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
gps_data_parse_t* gps_data_parser(const char * uart_stream);
#define UNIT_TESTING_ENABLED 1  // Set to 1 to enable public functions for unit testing ONLY, 0 to disable

// Conditional compilation based on UNIT_TESTING_ENABLED macro
#if UNIT_TESTING_ENABLED
#define PUBLIC_FOR_TESTING
#else
#define PUBLIC_FOR_TESTING static
#endif


//***************************************************
// Test Functions
//***************************************************

// Public function declarations that must be used for unit tests 
PUBLIC_FOR_TESTING int check_stream_NULL_Empty_public(const char *uart_stream);
PUBLIC_FOR_TESTING int gga_sentence_format_validity_check_public(const char *uart_stream);
PUBLIC_FOR_TESTING int check_sum_evaluation_public(const char *sentence);
PUBLIC_FOR_TESTING int is_valid_time_public(const char *time);
PUBLIC_FOR_TESTING int is_valid_numeric_public(const char *str, int expected_length);
PUBLIC_FOR_TESTING int is_valid_number_public(const char *str);
PUBLIC_FOR_TESTING float longitude_latitude_parser_public(const char *str);

#endif  // GPS_DATA_PARSER_H
