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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <esp_log.h>

#include "gps_data_parser.h"

#define TAG "ERROR"
static int s_crfl = 0;// static variable to hold index of \r\n in valid GPGGA sentence

static int check_stream_validity(const char *uart_stream);

static int gga_sentence_format_validity_check(const char *uart_stream);

static int check_sum_evaluation(const char *sentence);

static int is_valid_direction(char direction);

static int is_valid_time(const char *time);

static int is_valid_numeric(const char *str,int expected_length);

static int is_valid_altitude(const char *str);

static void print_default_value(gps_data_parse_t* data);
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
    	 int index = gga_sentence_format_validity_check(uart_stream);

    	      if (index != -1)    // if NMEA sentence is a valid GPGGA sentence then execute this if block code
    	      {

    	    	  unsigned int length = s_crfl - index;
    	    	  char temp[length + 1]; 					   // Added 1 for the null terminator

    	    	  strncpy(temp, uart_stream + index, length);   // Copy portion of uart_stream to temp
    	          temp[length] = '\0';  					   // Ensure null termination
    	          int x = check_sum_evaluation(temp);       // calling checksum function to check integrity of data in GPGGA sentence

    	             	if (x == 1) {


    	             		// Array to hold pointers to each field
    	             		char *fields[15];
    	             		int field_count = 0;

    	             		// Initialize a pointer for the start of each field
    	             		char *start = temp;
    	             		char *end = temp;

    	             		// Iterate through the string and find each field
    	             		while (*end != '\0' && field_count < 15) {
    	             		    if (*end == ',') {
    	             		         // Terminate the current field with a null character
    	             		        *end = '\0';
    	             		        // Store the start of the field in the fields array
    	             		        fields[field_count] = start;
    	             		        field_count++;

    	             		        // Move the start pointer to the character after the comma
    	             		        start = end + 1;
    	             		    }
    	             		    	// Move to the next character
    	             		    end++;
    	             		}

    	             		// Handle the last field if there was no trailing comma
    	             		if (start < end && field_count < 20) {
    	             		    fields[field_count] = start;
    	             		    field_count++;
    	             		}

    	            		if (field_count == 15) {

    	            		    // Extract and format the time
    	            		    if (!is_valid_time(fields[1])) {
    	            		        // If the time field is invalid, print "N/A"
    	            		        sprintf(gps_data.time, "N/A");
    	            		    }
    	            		    else {
    	            		        // Format the time as HH:MM:SS.SSS directly using sprintf
    	            		        sprintf(gps_data.time, "%.2s:%.2s:%.6s", fields[1], fields[1] + 2, fields[1] + 4);
    	            		    }

    	            		    // Extract and format the latitude
    	            		    if (!is_valid_numeric(fields[2],4) || !is_valid_direction(fields[3][0])) {
    	            		        // If the latitude or direction fields are invalid, print "N/A"
    	            		        sprintf(gps_data.latitude, "N/A");
    	            		    }
    	            		    else {
    	            		        double latitude = atof(fields[2]);
    	            		        char direction = fields[3][0]; // N or S
    	            		        // Convert latitude from degrees/minutes to degrees and fractional minutes
    	            		        int degrees = (int)(latitude / 100);
    	            		        double minutes = latitude - (degrees * 100);
    	            		        // Format the latitude as required (e.g., "12 55.7174 N")
    	            		        sprintf(gps_data.latitude, "%02d %.4f %c", degrees, minutes, direction);
    	            		    }

    	            		    // Extract and format the longitude
    	            		    if (!is_valid_numeric(fields[4],5) || !is_valid_direction(fields[5][0])) {
    	            		        // If the longitude or direction fields are invalid, print "N/A"
    	            		        sprintf(gps_data.longitude, "N/A");
    	            		    }
    	            		    else {
    	            		        double longitude = atof(fields[4]);
    	            		        char direction = fields[5][0]; // E or W
    	            		        // Convert longitude from degrees/minutes to degrees and fractional minutes
    	            		        int degrees = (int)(longitude / 100);
    	            		        double minutes = longitude - (degrees * 100);
    	            		        // Format the longitude as required (e.g., "077 37.2052 E")
    	            		        sprintf(gps_data.longitude, "%03d %.4f %c", degrees, minutes, direction);
    	            		    }

    	            		    // Extract and format the altitude
    	            		    if (!is_valid_altitude(fields[9]) || fields[10][0] == '\0') {
    	            		        // If the altitude or unit fields are invalid, print "N/A"
    	            		        sprintf(gps_data.altitude, "N/A");
    	            		    }
    	            		    else {
    	            		        double altitude = atof(fields[9]); // Parse the altitude from the 10th field (index 9)
    	            		        char unit = fields[10][0]; // Get the altitude unit from the 11th field (index 10)
    	            		        // Format the altitude as required (e.g., "333.2 M")
    	            		        sprintf(gps_data.altitude, "%.2f %c", altitude, unit);
    	            		    }
    	            		}
    	            	}
    	             	else
    	             	{
    	             	  ESP_LOGE(TAG, "Invalid CheckSum");
    	             	  // The checksum is invalid, so return default GPS data
    	             	  print_default_value(&gps_data);
    	             	  return gps_data;
    	             	}


    	     }
    	      else
    	      {
    	    	  ESP_LOGE(TAG, "Invalid NMEA 0183 Sentence");
    	     	  // The sentence format is not according to GPGGA sentence, so return default GPS data

    	    	  print_default_value(&gps_data);
    	    	  return gps_data;
    	      }
    }
    else
    {    ESP_LOGE(TAG, "Invalid Input String");
    	 // The stream is invalid (either NULL or empty), so return default GPS data
    	 //use sprintf function to store string to give parameters as string
    	 print_default_value(&gps_data);
    	 return gps_data;
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
     return ((uart_stream == NULL )|| (uart_stream[0] == '\0')) ? 1 : 0; // Return 1 if the stream is NULL or empty  otherwise return 0
}
/**
 * @brief Checks the validity of format of NMEA string.
 *
 * @param uart_stream The UART stream to check.
 * @return Returns starting index of $GPGGA sentence if it finds GGA sentence and also it finds CRLF at end of GGA sentence and no $ in between which can occurs if there is power instability to GPS module,otherwise it returns -1 if not valid GGA sentence format.
 */
int gga_sentence_format_validity_check(const char *uart_stream) {
     
    const char *substring_gga = strstr(uart_stream, "$GPGGA,"); // Check if the substring "$GPGGA," is found
    if (substring_gga == NULL) {
        printf("Error: GGA sentence not found in uart_stream.\n");
        return -1;
    }

    // Move past "$GPGGA," and check for "\r\n"
    const char *rn_string = strstr(substring_gga + 14, "\r\n"); // move 14 characters(commas) in such a case where all fields are empty
    if (rn_string == NULL) {
        printf("Error: Expected \\r\\n not found after GGA sentence.\n");
        return -1;
    }
    int  gga_pos =(substring_gga-uart_stream);
	s_crfl = (rn_string -uart_stream); //position at which \r\n starts
    // optionally, Print the GGA sentence
   // printf("GGA sentence found: %.*s\n", (int)(rn_string - substring_gga + 2), substring_gga); 
   // printf("length of gga sentence is: %d\n",(rn_string-substring_gga));

    return gga_pos; // Return index at which $GPGGA starts
}
/**
 * @brief Evaluates the checksum of an NMEA sentence.
 *
 * @param sentence The input NMEA sentence to evaluate.
 * @return Returns 1 if the calculated checksum matches the expected checksum (valid), otherwise returns 0 (invalid).
 */
int check_sum_evaluation(const char *sentence) {
    // Initialize the calculated checksum variable
    unsigned char calculated_checksum = 0;
    int index = 0;

    // Iterate over the sentence starting after '$' until '*' or end of string
    // Calculate the checksum using XOR operation
    for (index = 1; sentence[index] != '*' && sentence[index] != '\0'; index++) {
        calculated_checksum ^= sentence[index];
    }

    // Check if we reached the end of the sentence without finding '*'
    // Return 0 if '*' was not found, indicating invalid checksum
    if (sentence[index] != '*') {
        return 0;
    }

    // Move past the asterisk to the expected checksum part of the sentence
    const char *checksum_ptr = sentence + index + 1;

    // Parse the expected checksum from the sentence (2 hex digits)
    unsigned int expected_checksum;
    if (sscanf(checksum_ptr, "%2X", &expected_checksum) != 1) {
        // Return 0 if the expected checksum could not be parsed successfully
        return 0;
    }

    // Compare calculated checksum with expected checksum
    // Return 1 if they match (valid checksum), otherwise return 0 (invalid checksum)
    return calculated_checksum == expected_checksum ? 1 : 0;
}

/**
 * @brief Validates a string to ensure it consists of digits and a decimal point.
 *
 * This function checks whether a given string contains only digits and at most one decimal point.
 * It also ensures that the length of the string up to the first decimal point matches the expected length.
 *
 * @param str The string to validate.
 * @param expected_length The expected length of the string up to the first decimal point.
 * @return int Returns 1 if the string is valid, otherwise returns 0.
 */
int is_valid_numeric(const char *str, int expected_length) {
     // Initialize variables to track the number of dots and the length of the string
     int dot_count = 0;
     int length = 0;
     int d_length = 0;
     char output1[4] = {0};
     char output2[3] = {0};
     int value = 0;
     if (expected_length == 5)
     {
        sprintf(output1,"%.3s", str);
        value = atoi(output1);

     }
     if (expected_length == 4)
     {
        sprintf(output2,"%.2s", str);
         value = atoi(output2);

     }

     // Iterate through the string
     while (*str) {
         // Check if the current character is not a digit
         if (!isdigit((int)*str)) {
             // Check if the character is a dot and if no dot has been found yet
             if (*str == '.' && dot_count == 0) {
            	 d_length = length;
                 dot_count++;
             } else {
                 // If the character is neither a digit nor a valid dot, return 0 (invalid)
                 return 0;
             }
         }
         // Increment the length counter
         length++;
         // Move to the next character
         str++;
     }
     // Check if the length of the numeric string matches the expected length
     if (d_length != expected_length) {
         return 0; // Invalid length
     }

     if((value >180) && (expected_length == 5))
       {
           return 0;
       }

     if((value >90) && (expected_length == 4))
       {
           return 0;
       }

     // Return 1 if all characters are valid and length is correct
     return 1;
} /**
  * @brief Validates an altitude string.
  *
  * This function checks whether a given string represents a valid altitude.
  * A valid altitude string contains only numeric characters and at most one dot ('.').
  *
  * @param str The string to validate.
  * @return int Returns 1 if the string is a valid altitude, 0 otherwise.
  */


/**
 * @brief Checks if the given string represents a valid altitude.
 *
 * The function checks if the string contains only one dot (.) and digits.
 * It also returns 0 if the altitude field is empty.
 *
 * @param str The input string representing altitude.
 * @return int Returns 1 if the string is valid, 0 otherwise.
 */
static int is_valid_altitude(const char *str) {
    // Initialize variable to track the number of dots
    int dot_count = 0;

    // Check if the input string is empty
    if (*str == '\0') {
        return 0; // Return 0 if the altitude field is empty
    }

    // Iterate through the string
    while (*str) {
        // Check if the current character is not a digit
        if (!isdigit((int)*str)) {
            // Check if the character is a dot and if no dot has been found yet
            if (*str == '.' && dot_count == 0) {
                dot_count++;
            } else {
                // If the character is neither a digit nor a valid dot, return 0 (invalid)
                return 0;
            }
        }

        // Move to the next character
        str++;
    }

    // Return 1 if all characters are valid and there is at most one dot
    return 1;
}




 // Function to validate a time field in HHMMSS.SSS format

int is_valid_time(const char *time) {
    // First, check if the time format is valid and all characters are digits
    for (int i = 0; i < 7; i++) {
        if (i == 6) {
            if (time[i] != '.') {
                return 0; // Time should have a decimal point at position 6
            }
        } else if (!isdigit((int)time[i])) {
            return 0; // Invalid character found
        }
    }

    // Parse hours, minutes, seconds from the time string
   char hr_str[3], min_str[3], sec_str[3];

    strncpy(hr_str, time, 2);
    strncpy(min_str, time + 2, 2);
    strncpy(sec_str, time + 4, 2);

    hr_str[2] = min_str[2] = sec_str[2]  = '\0';

    int hr = atoi(hr_str);
    int min = atoi(min_str);
    int sec = atoi(sec_str);

    // Validate the parsed values
    if (hr > 23 || min > 59 || sec > 59)
        return 0;
    // All checks passed, return 1 indicating the time is valid
    return 1;
}



 // Function to check if a string is a valid compass direction (N, S, E, W)
 int is_valid_direction(char direction) {
     return (direction == 'N' || direction == 'S' || direction == 'E' || direction == 'W');
}


// Function to set default values for gps_data_parse_t structure
void print_default_value(gps_data_parse_t* data) {
    // Check if data is not NULL
    if (data != NULL) {
        // Use strncpy to avoid buffer overflow
        strncpy(data->time, "N/A", sizeof(data->time) - 1);
        data->time[sizeof(data->time) - 1] = '\0';  // Ensure null-termination

        strncpy(data->latitude, "N/A", sizeof(data->latitude) - 1);
        data->latitude[sizeof(data->latitude) - 1] = '\0';  // Ensure null-termination

        strncpy(data->longitude, "N/A", sizeof(data->longitude) - 1);
        data->longitude[sizeof(data->longitude) - 1] = '\0';  // Ensure null-termination

        strncpy(data->altitude, "N/A", sizeof(data->altitude) - 1);
        data->altitude[sizeof(data->altitude) - 1] = '\0';  // Ensure null-termination
    }
}




