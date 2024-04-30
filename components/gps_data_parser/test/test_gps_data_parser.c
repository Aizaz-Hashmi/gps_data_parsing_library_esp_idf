#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "gps_data_parser.h"

/**
 * @brief Test case 1: Valid GPGGA Sentence
 *
 * This test case verifies that the parser correctly handles a valid GPGGA packet
 * and extracts the expected GPS data fields such as time, latitude, longitude, and altitude.
 */
TEST_CASE("Valid GPGGA Sentence", "[gps_parser]") {
    const char packet[] = "$GPGGA,123456.257,2358.5623,N,12345.6719,E,1,08,1.0,120.83,M,0.0,M,18,ABC*15\r\n";
    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 1: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);
    // Validate parsed data against expected values
    TEST_ASSERT_EQUAL_STRING("12:34:56.257", result.time);
    TEST_ASSERT_EQUAL_STRING("23 58.5623 N", result.latitude);
    TEST_ASSERT_EQUAL_STRING("123 45.6719 E", result.longitude);
    TEST_ASSERT_EQUAL_STRING("120.83 M", result.altitude);
}

/**
 * @brief Test case 2: Incorrect Sentence Identifier
 *
 * This test case verifies that the parser correctly handles a GPS packet
 * with an invalid sentence identifier (not GPGGA). It expects the parser to discard the packet.
 */
TEST_CASE("Incorrect Sentence Identifier", "[gps_parser]") {
    const char packet[] = "$GPGSA,123456.00,1234.56,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,,ABC*2D\r\n";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 2: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" since the packet should be discarded
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.altitude);
}

/**
 * @brief Test case 3: GGA Sentence with Missing Parameters
 *
 * This test case verifies that the parser correctly handles a GPS packet with missing fields.
 * It expects the parser to handle missing fields gracefully and assign "N/A" values.
 */
TEST_CASE("GGA Sentence with Missing Parameters", "[gps_parser]") {
    const char packet[] = "$GPGGA,093423.453,1234.56,N,,E,,08,1.0,10.0,M,0.0,M,,ABC*31\r\n";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 3: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify the time field and expect missing fields to be "N/A"
    TEST_ASSERT_EQUAL_STRING("09:34:23.453", result.time);
    TEST_ASSERT_EQUAL_STRING("12 34.5600 N", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("10.00 M", result.altitude);
}

/**
 * @brief Test case 4: Empty Sentence
 *
 * This test case verifies that the parser handles an empty input gracefully,
 * expecting all parsed fields to be assigned "N/A".
 */
TEST_CASE("Empty Sentence", "[gps_parser]") {
    const char packet[] = "";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 4: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" for an empty sentence
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.altitude);
}

/**
 * @brief Test case 5: Only Identifier
 *
 * This test case verifies that the parser handles input with only the identifier (no data fields)
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Only Identifier", "[gps_parser]") {
    const char packet[] = "$GPGGA\r\n";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 5: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" for an input with only the identifier
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.altitude);
}

/**
 * @brief Test case 6: Missing Fields
 *
 * This test case verifies that the parser handles input with missing fields
 * by assigning "N/A" to fields with missing values.
 */
TEST_CASE("Missing Fields", "[gps_parser]") {
    const char packet[] = "$GPGGA,123456.00,,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,18,ABC*2D\r\n";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 6: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" when input has missing fields and checksum is incorrect
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.altitude);
}

/**
 * @brief Test case 7: Incorrect Data Format
 *
 * This test case verifies that the parser handles input with data fields in an unexpected format
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Corrupted Data field", "[gps_parser]") {
    const char packet[] = "$GPGGA,abcdef.00,1234.56,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,18,ABC*2D\r\n";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 7: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" for an input with incorrect data format
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("12 34.5600 N", result.latitude);
    TEST_ASSERT_EQUAL_STRING("123 45.6700 E", result.longitude);
    TEST_ASSERT_EQUAL_STRING("10.00 M", result.altitude);
}

/**
 * @brief Test case 8: Incorrect Checksum
 *
 * This test case verifies that the parser handles input with an incorrect checksum
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Incorrect Checksum", "[gps_parser]") {
    const char packet[] = "$GPGGA,123456.00,1234.56,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,18,ABC*3E\r\n";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 8: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" for an input with an incorrect checksum
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.altitude);
}

/**
 * @brief Test case 9: Out of Range Latitude and Longitude Values
 *
 * This test case verifies that the parser handles input with latitude and longitude values
 * that are out of valid range by assigning "N/A" to those fields.
 */
TEST_CASE("Out of Noraml Range Latitude and Longitude Values", "[gps_parser]") {
    const char packet[] = "$GPGGA,123456.000,9934.234,N,19034.040,E,1,08,1.0,10.0,M,0.0,M,18,ABC*13\r\n";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 9: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify time and check that latitude and longitude fields are "N/A" for out of range values latitude greater than 90,longitude greater than 180
    TEST_ASSERT_EQUAL_STRING("12:34:56.000", result.time); // data is only parsed based on checksum and number od fields
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("10.00 M", result.altitude);
}

/**
 * @brief Test case 10: Invalid Packet Format
 *
 * This test case verifies that the parser handles input with invalid packet format
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Invalid Packet Format", "[gps_parser]") {
    const char packet[] = "invalid packet";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 10: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" for an input with invalid packet format
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.altitude);
}

/**
 * @brief Test case 11: Corrupted Data
 *
 * This test case verifies that the parser handles input with corrupted data fields
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Corrupted Data", "[gps_parser]") {
    const char packet[] = "$GPGGA,abcdefghij,k,lmn,o,p,q,r,s,tuv,wxyz\r\n";

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 11: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" for an input with corrupted data
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.altitude);
}

/**
 * @brief Test case 12: Sentence with Excessive Length
 *
 * This test case verifies that the parser handles an input sentence that exceeds the expected length
 * by discarding the sentence and setting all fields to "N/A".
 */
TEST_CASE("Sentence with Excessive Length", "[gps_parser]") {
    char packet[1000];
    memset(packet, 'A', sizeof(packet));
    packet[999] = '\0';

    gps_data_parse_t result = gps_data_parser(packet);
    printf("\nTest 12: Time: %s, Latitude: %s, Longitude: %s, Altitude: %s\n", result.time, result.latitude, result.longitude, result.altitude);

    // Verify that all parsed fields are "N/A" for an excessively long sentence
    TEST_ASSERT_EQUAL_STRING("N/A", result.time);
    TEST_ASSERT_EQUAL_STRING("N/A", result.latitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.longitude);
    TEST_ASSERT_EQUAL_STRING("N/A", result.altitude);
}
