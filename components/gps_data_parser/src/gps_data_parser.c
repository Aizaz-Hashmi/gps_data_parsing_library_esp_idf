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
#include "gps_data_parser.h"

static int s_crfl = 0;// static variable to hold index of \r\n in valid GPGGA sentence

static int check_stream_validity(const char *uart_stream);

static int gga_sentence_format_validity_check(const char *uart_stream);
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

    	      if (index != -1)
    	      {

    	    	 unsigned int length = s_crfl - index;
    	    	 char temp[length + 1];  // Added 1 for the null terminator

    	    	 strncpy(temp, uart_stream + index, length);  // Copy portion of uart_stream to temp
    	          temp[length] = '\0';  // Ensure null termination

    	      }
    	      else
    	      {

    	     	 // The sentence format is not according to GPGGA sentence, so return default GPS data

    	     	  sprintf(gps_data.time, "N/A");
    	     	  sprintf(gps_data.latitude, "N/A");   // here any string can be added as per preference like empty string or zeros
    	     	  sprintf(gps_data.longitude, "N/A");
    	     	  sprintf(gps_data.altitude, "N/A");
    	      }
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
	return (uart_stream == NULL) ? 1 : 0; // Return 1 if the stream is NULL, otherwise return 0
}
/**
 * @brief Checks the validity of format of NMEA string.
 *
 * @param uart_stream The UART stream to check.
 * @return Returns starting index of $GPGGA sentence if it finds GGA sentence and also it finds CRLF at end of GGA sentence and no $ in between which can occurs if there is power instability to GPS module,otherwise it returns -1 if not valid GGA sentence format.
 */
int gga_sentence_format_validity_check (const char *uart_stream)
{

    int size = strlen(uart_stream); // Calculate the length of the stream


    char temp[size+1];             // Allocate a temporary buffer to store the sentence

    int i;                         // Copy the input stream to temp array
    for ( i = 0; i < size; i++) {
        temp[i] = uart_stream[i];
    }
    temp[size] = '\0'; 			   // Null terminate the copied string

    for (i = 0; i <= size - 6; i++) {
    		 // Check if the substring matches "$GPGGA,"
    	if (temp[i] == '$' && temp[i + 1] == 'G' && temp[i + 2] == 'P' &&
					temp[i + 3] == 'G' && temp[i + 4] == 'G' && temp[i + 5] == 'A' &&  temp[i+6] == ',') {

    		 // Once "$GPGGA," is found, search for "\r\n" in the rest of the string
        int j = i + 7;
        int found_rn = 0;
        int has_dollar = 0;
             s_crfl =0;
        while (j < size - 1) {
            // Check for the "\r\n" sequence
            if (temp[j] == '\r' && temp[j + 1] == '\n') {
                found_rn = 1;

                break;
            }
            if (temp[j] == '$') {
                          has_dollar = 1;
                          break;
            }
           // If a dollar sign is found in between, restart the search from the position after the dollar sign
             if (has_dollar) {
                        i = j;  // Update `i` to restart the search after the position where the dollar sign was found
                        continue;
             }

            j++;
        }

        // If "\r\n" is found without a dollar sign between "$GPGGA" and "\r\n"
        if (found_rn && !has_dollar) {
            // Return the index of "$GPGGA"
            s_crfl =j;
            return i;
        }
    }
    }
    return -1; // Return -1 if the GGA sentence is not found
}
