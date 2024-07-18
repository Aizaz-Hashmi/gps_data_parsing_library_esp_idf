#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>
#include "unity.h"
#include "gps_data_parser.h"



//====================================================================================================================================================================================================================================================================
//                         Test of gps_result_parser function 
//====================================================================================================================================================================================================================================================================
/**
 * @brief Test case 1: Valid GPGGA Sentence
 *
 * This test case verifies that the parser correctly handles a valid GPGGA packet
 * and extracts the expected GPS result fields such as time, latitude, longitude, and altitude.
 */
TEST_CASE("Valid GPGGA Sentence", "[gps_parser]") {
    const char packet[] = "$GPGGA,123456.257,2358.5623,N,12345.6719,E,1,08,1.0,120.83,M,0.0,M,18,934*6B\r\n";
    gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);

    // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8((12+5), result->time.hour); // hours, 5 added for Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(34, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(56, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(257, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(23.97603, result->latitude); // latitude value
    TEST_ASSERT_EQUAL('N', result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(123.76119, result->longitude); //longitude value
    TEST_ASSERT_EQUAL('E', result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(1, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(8, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(1.0, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(120.83, result->altitude); // Altitude
    TEST_ASSERT_EQUAL('M', result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(0.0, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL('M', result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(18, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(934, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
}
/**
 * @brief Test case 2: Incorrect Sentence Identifier
 *
 * This test case verifies that the parser correctly handles a GPS packet
 * with an invalid sentence identifier (not GPGGA). It expects the parser to discard the packet.
 */
TEST_CASE("Incorrect Sentence Identifier", "[gps_parser]") {
    const char packet[] = "$GPGSA,123456.00,1234.56,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,,ABC*2D\r\n";

    gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(DEFAULT_FIX_QUALITY, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(DEFAULT_NUM_SATELLITES, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_HDOP, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_ALTITUDE, result->altitude); // Altitude
    TEST_ASSERT_EQUAL(DEFAULT_ALTITUDE_UNITS, result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_GEOID_HEIGHT, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL(DEFAULT_GEOID_HEIGHT_UNITS, result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DGPS_AGE, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory

}

/**
 * @brief Test case 3: GGA Sentence with Missing Parameters
 *
 * This test case verifies that the parser correctly handles a GPS packet with missing fields.
 * It expects the parser to handle missing fields gracefully and assign them default values.
 */
TEST_CASE("GPGGA Sentence with missing fields", "[gps_parser]")
{
    gps_gga_handle_t result = gps_data_parser("$GPGGA,,,,,W,,8,1.03,,M,,M,,*25\r\n");
    
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, 5 added for Pakistan Standard Time
    TEST_ASSERT_EQUAL_INT(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_INT(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_INT(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value should be negative because its direction is west in given sentence
    TEST_ASSERT_EQUAL('W', result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(DEFAULT_FIX_QUALITY, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(8, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(1.03, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_ALTITUDE, result->altitude); // Altitude
    TEST_ASSERT_EQUAL('M', result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_GEOID_HEIGHT, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL('M', result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DGPS_AGE, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
               
}


/**
 * @brief Test case 4: Empty Sentence
 *
 * This test case verifies that the parser handles an empty input gracefully,
 * expecting all parsed fields to be assigned "N/A".
 */
TEST_CASE("Empty Stream", "[gps_parser]") {
    const char packet[] = "";
gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(DEFAULT_FIX_QUALITY, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(DEFAULT_NUM_SATELLITES, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_HDOP, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_ALTITUDE, result->altitude); // Altitude
    TEST_ASSERT_EQUAL(DEFAULT_ALTITUDE_UNITS, result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_GEOID_HEIGHT, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL(DEFAULT_GEOID_HEIGHT_UNITS, result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DGPS_AGE, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
}

/**
 * @brief Test case 5: Only Identifier
 *
 * This test case verifies that the parser handles input with only the identifier (no result fields)
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Only Identifier", "[gps_parser]") {
    const char packet[] = "$GPGGA\r\n";

   gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(DEFAULT_FIX_QUALITY, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(DEFAULT_NUM_SATELLITES, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_HDOP, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_ALTITUDE, result->altitude); // Altitude
    TEST_ASSERT_EQUAL(DEFAULT_ALTITUDE_UNITS, result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_GEOID_HEIGHT, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL(DEFAULT_GEOID_HEIGHT_UNITS, result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DGPS_AGE, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
}


/**
 * @brief Test case 6: Incorrect result Format
 *
 * This test case verifies that the parser handles input with result fields in an unexpected format
 * by discarding the sentence and setting all fields to Defaults".
 */
TEST_CASE("Corrupted result field", "[gps_parser]") {
    const char packet[] = "$GPGGA,abcdef.00,1234.56,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,18,ABC*2D\r\n";

    gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(12.576, result->latitude); // latitude value
    TEST_ASSERT_EQUAL('N', result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(123.761, result->longitude); //longitude value
    TEST_ASSERT_EQUAL('E', result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(1, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(8, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(1.0, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(10.0, result->altitude); // Altitude
    TEST_ASSERT_EQUAL('M', result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(0, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL('M', result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(18, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
}

/**
 * @brief Test case 7: Incorrect Checksum
 *
 * This test case verifies that the parser handles input with an incorrect checksum
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Incorrect Checksum", "[gps_parser]") {
    const char packet[] = "$GPGGA,123456.00,1234.56,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,18,ABC*3E\r\n";

    gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(DEFAULT_FIX_QUALITY, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(DEFAULT_NUM_SATELLITES, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_HDOP, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_ALTITUDE, result->altitude); // Altitude
    TEST_ASSERT_EQUAL(DEFAULT_ALTITUDE_UNITS, result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_GEOID_HEIGHT, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL(DEFAULT_GEOID_HEIGHT_UNITS, result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DGPS_AGE, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
}

/**
 * @brief Test case 8: Out of Range Latitude and Longitude Values
 *
 * This test case verifies that the parser handles input with latitude and longitude values
 * that are out of valid range by assigning "N/A" to those fields.
 */
TEST_CASE("Out of Noraml Range Latitude and Longitude Values", "[gps_parser]") {
    const char packet[] = "$GPGGA,123456.000,9934.234,N,19034.040,E,1,08,1.0,10.0,M,0.0,M,18,*53\r\n";

    gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(12+5, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(34, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(56, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(000, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL('N', result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value
    TEST_ASSERT_EQUAL('E', result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(1, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(8, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(1.0, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(10.0, result->altitude); // Altitude
    TEST_ASSERT_EQUAL('M', result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(0.0, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL('M', result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(18, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
}

/**
 * @brief Test case 9: Invalid Packet Format
 *
 * This test case verifies that the parser handles input with invalid packet format
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Invalid Packet Format", "[gps_parser]") 
{
    const char packet[] = "invalid packet";
    gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(DEFAULT_FIX_QUALITY, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(DEFAULT_NUM_SATELLITES, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_HDOP, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_ALTITUDE, result->altitude); // Altitude
    TEST_ASSERT_EQUAL(DEFAULT_ALTITUDE_UNITS, result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_GEOID_HEIGHT, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL(DEFAULT_GEOID_HEIGHT_UNITS, result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DGPS_AGE, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory

    
}

/**
 * @brief Test case 10: Corrupted result
 *
 * This test case verifies that the parser handles input with corrupted result fields
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Corrupted result", "[gps_parser]") 
{
    const char packet[] = "$GPGGA,abcdefghij,k,lmn,o,p,q,r,s,tuv,wxyz\r\n";

    gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(DEFAULT_FIX_QUALITY, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(DEFAULT_NUM_SATELLITES, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_HDOP, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_ALTITUDE, result->altitude); // Altitude
    TEST_ASSERT_EQUAL(DEFAULT_ALTITUDE_UNITS, result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_GEOID_HEIGHT, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL(DEFAULT_GEOID_HEIGHT_UNITS, result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DGPS_AGE, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
}


/**
 * @brief Test case 11: Sentence with Excessive Length
 *
 * This test case verifies that the parser handles an input sentence that exceeds the expected length
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Sentence with Excessive Length", "[gps_parser]")
{
    char packet[1000];
    memset(packet, 'A', sizeof(packet));
    packet[999] = '\0';

    gps_gga_handle_t result = gps_data_parser(packet);
    // Using ESP-IDF logging function for each output
    printf("  TIME: %02d:%02d:%02d.%d", result->time.hour, result->time.minute, result->time.second, result->time.millisecond);
    printf("  LATITUDE: %f %c",result->latitude, result->lat_direction);
    printf("  LONGITUDE: %f %c",result->longitude, result->lon_direction);
    printf("  FIX QUALITY: %d",result->fix_quality);
    printf("  NUMBER OF SATELLITES: %d",result->num_satellites);
    printf("  HORIZONTAL DILUTION: %0.3f",result->hdop);
    printf("  ALTITUDE: %0.3f %c",result->altitude, result->altitude_units);
    printf("  GEOIDAL SEPARATION: %0.3f %c", result->geoid_height, result->geoid_height_units);
    printf("  AGE OF DIFFERENTIAL GPS result: %f", result->dgps_age);
    printf("  DIFFERENTIAL REFERENCE STATION ID: %d", result->dgps_station_id);
     // Validate parsed result against expected values
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_HR, result->time.hour); // hours, Pakistan Standard Time
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MIN, result->time.minute);// minutes
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_SEC, result->time.second); // seconds 
    TEST_ASSERT_EQUAL_UINT8(DEFAULT_GPS_TIME_MS, result->time.millisecond); // milliseconds
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LATITUDE, result->latitude); // latitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lat_direction); // latitude direction
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_LONGITUDE, result->longitude); //longitude value
    TEST_ASSERT_EQUAL(DEFAULT_LAT_DIRECTION, result->lon_direction); // longitude direction
    TEST_ASSERT_EQUAL_INT(DEFAULT_FIX_QUALITY, result->fix_quality); // Fix quality: 1 = GPS fix
    TEST_ASSERT_EQUAL_INT(DEFAULT_NUM_SATELLITES, result->num_satellites); // Number of satellites
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_HDOP, result->hdop); // Horizontal dilution of precision
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_ALTITUDE, result->altitude); // Altitude
    TEST_ASSERT_EQUAL(DEFAULT_ALTITUDE_UNITS, result->altitude_units); // Altitude units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_GEOID_HEIGHT, result->geoid_height); // Height of geoid
    TEST_ASSERT_EQUAL(DEFAULT_GEOID_HEIGHT_UNITS, result->geoid_height_units); // Geoid separation units: Meters
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DGPS_AGE, result->dgps_age); // DGPS age (empty in this case)
    TEST_ASSERT_EQUAL_INT(DEFAULT_DGPS_STATION_ID, result->dgps_station_id); // DGPS station ID 
    free(result);// deallocate memory
}



//====================================================================================================================================================================================================================================================================
//                         Test of private functions of the library 
//====================================================================================================================================================================================================================================================================

TEST_CASE("GPGGA Stream Empty or NULL", "[gps_parser]")
{
    // this function 1 when stream is NULL or empty
    TEST_ASSERT_EQUAL(1, check_stream_NULL_Empty_public(NULL));
    TEST_ASSERT_EQUAL(1, check_stream_NULL_Empty_public(""));
    TEST_ASSERT_EQUAL(0, check_stream_NULL_Empty_public("aizaz"));
    TEST_ASSERT_EQUAL(0, check_stream_NULL_Empty_public("                                                                                                    "));
}

TEST_CASE("GGA_sentence_format_validity_check", "[gps_parser]")
{  // this function is executed only when when stream passes if condition of check_stream_NULL_Empty function, it returns index of $ when string starts with $GPGGA, and ends with \r\n
   //  it returns index of 
    TEST_ASSERT_EQUAL(0, gga_sentence_format_validity_check_public("$GPGGA,\r\n"));
    TEST_ASSERT_EQUAL(-1, gga_sentence_format_validity_check_public("             "));
    TEST_ASSERT_EQUAL(1, gga_sentence_format_validity_check_public("$$GPGGA,\r\n\r\n")); // this function will skim the string from index 1 as soon as it finds \r\n is sent for checksum validity and discards the rest 
    TEST_ASSERT_EQUAL(-1, gga_sentence_format_validity_check_public("$GPGGA,xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
    TEST_ASSERT_EQUAL(-1, gga_sentence_format_validity_check_public("$GPGGA,xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
    TEST_ASSERT_EQUAL(-1, gga_sentence_format_validity_check_public("$GPGGA,,,                                                                                                          ,,hello\n"));
    TEST_ASSERT_EQUAL(-1, gga_sentence_format_validity_check_public("GPGGA,abdsjdhsjkdnjksnd\n\r\r\n"));
}

TEST_CASE("Checksum evaluation: Expected == calculated ","[gps_parser]")
{    // when expected checksum == calculated it returns 1 
    TEST_ASSERT_EQUAL(1,check_sum_evaluation_public("$GPGGA,123204.00,5106.94086,N,01701.51680,E,1,06,3.86,127.9,M,40.5,M,,*51"));
    TEST_ASSERT_EQUAL(1,check_sum_evaluation_public("$GNGBS,170556.00,3.0,2.9,8.3,,,,*5C"));
    TEST_ASSERT_EQUAL(1,check_sum_evaluation_public("$GPGSV,4,2,13,08,51,203,30,09,45,215,28,10,69,197,19,13,47,081,*76"));
    TEST_ASSERT_EQUAL(1,check_sum_evaluation_public("$GPGGA,xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*7A"));
    TEST_ASSERT_EQUAL(1,check_sum_evaluation_public("$GPGGGGGGG______MY_NAME_IS_AIZAZ_UBAID_HASHMI_@@@@@@1928___________________67365---------###########%%%%%%%*1A"));
    
    
    
    
}
TEST_CASE("Time field validity: time : HHMMSS.sss","[gps_parser]")
{
    
    TEST_ASSERT_EQUAL(1,is_valid_time_public("123456.678"));// valid time format
    TEST_ASSERT_EQUAL(0,is_valid_time_public("563456.33"));// out of range hours
    TEST_ASSERT_EQUAL(0,is_valid_time_public("123456"));// invalid time format
    TEST_ASSERT_EQUAL(0,is_valid_time_public("abcdef.ghk"));// corrupted
    TEST_ASSERT_EQUAL(0,is_valid_time_public(" "));// empty time field
    TEST_ASSERT_EQUAL(0,is_valid_time_public(NULL));// null time field
    TEST_ASSERT_EQUAL(0,is_valid_time_public("246060.1000"));// 0ut of range field
    
    
}

TEST_CASE("string consisting of real numbers only","[gps_parser]")
{  //function used to check if a string is consisting of digits from 0 to 9 and one decimal point and a negative sign 
    TEST_ASSERT_EQUAL(1,is_valid_number_public("123456"));// valid time format
    TEST_ASSERT_EQUAL(0,is_valid_number_public("56nuy456.33"));// string with non numeric mixed characters
    TEST_ASSERT_EQUAL(1,is_valid_number_public("12345687987987980980980980980989088009808098098098"));// very large string
    TEST_ASSERT_EQUAL(0,is_valid_number_public("abcdef.ghk"));// corrupted
    TEST_ASSERT_EQUAL(0,is_valid_number_public(" "));// empty number field
    TEST_ASSERT_EQUAL(0,is_valid_number_public(NULL));// null number field
    TEST_ASSERT_EQUAL(1,is_valid_number_public("246060.1000"));// decimal field
    
}

TEST_CASE("string to test latitude and longitude format","[gps_parser]")
{   //4 value as argument for latitude and 5 for longitude , this funcion is only when executed when there are complete fields , stream is valid and it is GGA sentence
    TEST_ASSERT_EQUAL(1,is_valid_numeric_public("1234.3455",4));// valid latitude format
    TEST_ASSERT_EQUAL(1,is_valid_numeric_public("16258.3334",5));// valid longitude format
    TEST_ASSERT_EQUAL(0,is_valid_numeric_public("12.800",4));// 
    TEST_ASSERT_EQUAL(0,is_valid_numeric_public("abcdef.ghk",3));// corrupted
    TEST_ASSERT_EQUAL(0,is_valid_numeric_public(" ",5));// empty number field
    TEST_ASSERT_EQUAL(0,is_valid_numeric_public(NULL,4));// null number field
    TEST_ASSERT_EQUAL(0,is_valid_numeric_public("9634.1004",4));// out of range latitude
    TEST_ASSERT_EQUAL(0,is_valid_numeric_public("19634.1525",5));// out of range longitude
   
    
} 

TEST_CASE("latitude and longitude processor test","[gps_parser]")
{
    // this function only takes valid inputs after lat , long field is filtered through earlier functions, it only is executed in the library code if there is valid lat, long value.
    
  TEST_ASSERT_EQUAL_FLOAT(59.390968,longitude_latitude_parser_public("5923.4578")); // valid latitude format DDMM.MMMM
  TEST_ASSERT_EQUAL_FLOAT(137.753967,longitude_latitude_parser_public("13745.2378")); // valid longitude format DDDMM.MMM
  TEST_ASSERT_EQUAL_FLOAT(0.0,longitude_latitude_parser_public(""));     // empty field
  TEST_ASSERT_EQUAL_FLOAT(0.0,longitude_latitude_parser_public(NULL));     // NULL input
      
}
