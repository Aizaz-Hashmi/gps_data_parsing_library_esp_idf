/**
 * @file test_gps_data_parser.c
 * @brief This file contains test cases for the GPS data parser functions.
 *
 * These tests verify the correctness of the `gps_data_parser` and `validate_checksum`
 * functions by providing various input scenarios and comparing the results against
 * expected values.
 *
 * @created 28-Apr-2024
 * @author Aizaz Ubaid Hashmi
 */

#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "gps_data_parser.h"



/**
 * @brief Test case for verifying incorrect sentence identifier.
 *
 * This test case checks whether the parser correctly handles a GPS packet
 * with an invalid sentence identifier (not GPGGA).
 */
TEST_CASE("  Wrong Sentence Identifier  ", "  Other Identifiers  ") {
   char packet[] = "$GPGSA,123456.00,1234.56,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,,ABC*47";

   gps_data_parse_t result= gps_data_parser(packet);
   	   TEST_ASSERT_EQUAL_STRING("123456.00", result.time); // time in hh:mm:ss.sss
       TEST_ASSERT_EQUAL_STRING("1234.56", result.latitude); // Latitude in degrees and minutes
       TEST_ASSERT_EQUAL_STRING("12345.67", result.longitude); // Longitude in degrees and minutes
       TEST_ASSERT_EQUAL_STRING("12345.67", result.altitude); //altitude in meters
    printf("TEST1   %s    %s      %s     %s  \n",result.time,result.latitude,result.longitude,result.altitude);

}

/**
 * @brief Test case for verifying a valid GPGGA sentence.
 *
 * This test case checks whether the parser correctly handles a valid GPGGA packet
 * and extracts the expected GPS data fields.
 */
TEST_CASE("Valid GPGGA Sentence", " correct format ") {
    const char packet[] = "$GPGGA,123456.00,1234.56,N,12345.67,E,1,08,1.0,10.0,M,0.0,M,18,ABC*47";

    gps_data_parse_t result = gps_data_parser(packet);
    TEST_ASSERT_EQUAL_STRING("123456.00", result.time); // time in hh:mm:ss.sss
        TEST_ASSERT_EQUAL_STRING("1234.56", result.latitude); // Latitude in degrees and minutes
        TEST_ASSERT_EQUAL_STRING("12345.67", result.longitude); // Longitude in degrees and minutes
        TEST_ASSERT_EQUAL_STRING("12345.67", result.altitude); //altitude in meters
        printf("TEST2   %s    %s      %s     %s  \n",result.time,result.latitude,result.longitude,result.altitude);

}

/**
 * @brief Test case for verifying an invalid sentence.
 *
 * This test case checks whether the parser correctly handles a GPS packet
 * with invalid data (e.g., missing fields or noise).
 */
TEST_CASE("Invalid Sentence", " Packet Loss or Noise ") {
    const char packet[] = "$GPGSA,123456.00,1234.56,N,,E,,08,1.0,10.0,,0.0,M,,ABC*47";
    gps_data_parse_t  result = gps_data_parser(packet);

    TEST_ASSERT_EQUAL_STRING("123456.00", result.time); // time in hh:mm:ss.sss
     TEST_ASSERT_EQUAL_STRING("1234.56", result.latitude); // Latitude in degrees and minutes
     TEST_ASSERT_EQUAL_STRING("12345.67", result.longitude); // Longitude in degrees and minutes
     TEST_ASSERT_EQUAL_STRING("12345.67", result.altitude); //altitude in meters
     printf("TEST3   %s    %s      %s     %s  \n",result.time,result.latitude,result.longitude,result.altitude);

}


