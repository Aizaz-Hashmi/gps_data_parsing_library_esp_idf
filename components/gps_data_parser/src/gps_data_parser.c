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
#define TIME_ZONE 5			 //Pakistan Time UTC +5
#define Buffer_size 1024


static int s_crfl = 0;			// static variable to hold index of \r\n in valid GPGGA sentence
static char *fields[15];		// Array of pointers to each field of GGA sentence

static int check_stream_NULL_Empty(const char * uart_stream);
static int gga_sentence_format_validity_check (const char *uart_stream);
static int check_sum_evaluation (const char *sentence);
static int is_valid_time (const char *time);
static int is_valid_numeric (const char *str, int expected_length);
static int is_valid_number (const char *str);
static void print_default_value (gps_data_parse_t * data);	// function to print default values in case there are issues in uart stream
static void utc_time_parser (gps_data_parse_t * gps_time);	// function to parse time in utc format 
static float longitude_latitude_parser (const char *str);	// function to parse latitude and longitude in degrees
void gps_fix_quality_description (int gps_quality_fix);	//public function to tell GPS fix quality
/**
 * @brief Parses a UART stream to extract GPS data.
 *
 * @param uart_stream The input string containing GPS data.
 * @return A structure containing parsed GPS data.
 */ 
gps_data_parse_t * gps_data_parser (const char *uart_stream)
{ 
    gps_data_parse_t * gps_data = (gps_data_parse_t *) malloc (sizeof (gps_data_parse_t));	//dynamic memory allocation for structure members
	// Check if the UART stream is valid
	if (!(check_stream_NULL_Empty (uart_stream))){
	    
		// Allocate memory for the UART stream buffer
	    char *temp_buffer = (char *) malloc (Buffer_size * sizeof (char));
	  
 
        if (temp_buffer != NULL){
            // if(strlen(temp_buffer) > strlen(uart_stream) + 300)
            // Copy the UART stream to the temporary buffer
			strcpy (temp_buffer, uart_stream); 
			//else: print insufficient memory please increase buffer size return NULL
            
        }
	  
	    else{
		    fprintf (stderr, "Memory allocation failed for buffer\n");
		    print_default_value(gps_data);
		    return gps_data;
		}
	  
	   	// process stream if it is not null or empty
	     int index = gga_sentence_format_validity_check (temp_buffer);
	  
 
        if (index != -1)
		{   // if NMEA sentence is a valid GPGGA sentence then execute this if block code
		    unsigned int length = s_crfl - index;	// Calculate the length of the substring
		    char temp[length + 1];// Allocate space for the substring plus null terminator
		  
		    // Copy the substring (GGA Sentence) from temp_buffer starting from index
		    strncpy (temp, temp_buffer + index, length);
		    temp[length] = '\0';	// Null-terminate the string
		  
    	    // calling checksum function to check integrity of data in GPGGA sentence
    			
    		if (check_sum_evaluation (temp)){
    			  
                int field_count = 0; // Counter for number of fields found
    			  
    	    	// Initialize pointers for the start and end of each field
    	    	char *start = temp;
    			  
                char *end = temp;
    			  
            	// Iterate through the string and find each field
    	    	while (*end != '\0')
    				{
    				  if (*end == ',' || *end == '*'){
    					  
    					    // Terminate the current field with a null character
    						*end = '\0';
    					    // Store the start of the field in the fields array
    						fields[field_count] = start;
    					    field_count++;
    					  	// Move the start pointer to the character after the comma or asterisk
    						start = end + 1;
    				}   
    				  
                     end++;		// Move to the next character
    				}
    			  
                	//check if total fields in GGA sentence are 15 either empty or populated
    				if (field_count == 15){
    				  
        				  // Extract and format the time
        			    if (!(is_valid_time (fields[1]))){
        				      
        					    // If the time field is invalid,empty set time to 255 or any default value
        						gps_data->time.hour = DEFAULT_GPS_TIME_HR;
        					  
                                gps_data->time.minute = DEFAULT_GPS_TIME_MIN;
        					  
                                gps_data->time.second = DEFAULT_GPS_TIME_SEC;
        					  
                                gps_data->time.millisecond = DEFAULT_GPS_TIME_MS;
        					
                        }
        				  
        			    else{
        					  // Format the time as HH:MM:SS.SSS using UTC time parser
        					  utc_time_parser (gps_data);
        				}
    				  
     
                        if (!(is_valid_numeric (fields[2], 4)))	{
    					  
    						// If the latitude field is invalid,empty set latitude to -9999 or any default
    						gps_data->latitude = DEFAULT_LATITUDE;
    					}
    					
    				    else{
    					  
    					    // Process the altitude into degrees
    						gps_data->latitude = longitude_latitude_parser (fields[2]);
    					}
    				  
     
                        if (fields[3] == NULL || strlen (fields[3]) == 0
    						 || !(fields[3][0] == 'N' || fields[3][0] == 'n'
    							  || fields[3][0] == 'S' || fields[3][0] == 's')){
    					    // If the latitude direction field is invalid, set direction to any default character
    						gps_data->lat_direction = DEFAULT_LAT_DIRECTION;
    					}
    				  
                        else{
    					  
                            if (fields[3][0] == 'S' || fields[3][0] == 's'){
    						  
     
                                gps_data->lat_direction = fields[3][0];
    						  
                                gps_data->latitude *= -1;
    						}
    					  
     
    					    else{
    						      gps_data->lat_direction = fields[3][0];
    						}
                       }
    				  
                        if (!(is_valid_numeric (fields[4], 5))){
    					  
    						// If the longtude field is invalid,empty set longitude  to zero or any default
    						gps_data->longitude = DEFAULT_LONGITUDE;
    					}
    				    else{
    					    // Process the altitude into degrees
    						gps_data->longitude = longitude_latitude_parser (fields[4]);
    					}
    				  
     
                        if (fields[5] == NULL || strlen (fields[5]) == 0
    						 || !(fields[5][0] == 'E' || fields[5][0] == 'e'
    							  || fields[5][0] == 'W' || fields[5][0] == 'w'))
    					{
    					  
    						// If the longitude direction field is invalid,empty set direction to '-'  or any default character
    						gps_data->lon_direction = DEFAULT_LON_DIRECTION;
    					
                        }
    				  
    				    else
    					{
    					    if (fields[5][0] == 'W' || fields[5][0] == 'w')
    						{
    						  gps_data->lon_direction = fields[5][0];
    						  
                              gps_data->longitude *= -1;
    						}
    					    else{
    						  gps_data->lon_direction = fields[5][0];
    						}
    					
                        }
    				  
     
                        if (fields[6] == NULL || strlen (fields[6]) == 0
    						 || !(is_valid_number (fields[6]))){
    					  
                            gps_data->fix_quality = DEFAULT_FIX_QUALITY;
    					
                        }
    				  
    				    else{
    					   gps_data->fix_quality = atoi (fields[6]);
    					
                        }
                        
    				    if (fields[7] == NULL || strlen (fields[7]) == 0
    						 || !(is_valid_number (fields[7]))){
    					  
    						// If the number of satellites field is invalid,empty set to -1  or any default character
    						
                            gps_data->num_satellites = DEFAULT_NUM_SATELLITES;
    					}
    				  
                        else{
    					    gps_data->num_satellites = atoi (fields[7]);
    					}
    				  
                        if (fields[8] == NULL || strlen (fields[8]) == 0
    						 || !(is_valid_number (fields[8])))
    					{
    					  
    						// If the Horizontal Dilution of Precision field is invalid,empty then set to -1  or any default value
    						gps_data->hdop = DEFAULT_HDOP;
    					}
    				  
                        else{
    					
    					  gps_data->hdop = strtof (fields[8], NULL);
    					
                        }
    				  
     
                        if (fields[9] == NULL || strlen (fields[9]) == 0
    						 || !(is_valid_number (fields[9])))
    					{
    					  
    						// If the Mean Sea Level Altitude field is invalid,empty then set to -999999  or any default value
    						gps_data->altitude = DEFAULT_ALTITUDE;
    					}
    				  
     
    				    else{
    					
    					    gps_data->altitude = strtof (fields[9], NULL);
    					
                        }
    				  
     
                        if (fields[10] == NULL || strlen (fields[10]) == 0
    						 || (fields[10][0] != 'M'))
    					{
    					    // If the Mean Sea Level Altitude unit  field is invalid,empty then set to -  or any default character
    						gps_data->altitude_units = DEFAULT_ALTITUDE_UNITS;
    					}
    				  
     
    				    else{
    					    gps_data->altitude_units = fields[10][0];	// meter 
    					  
                            #if USE_FEET_UNIT
    						gps_data->altitude_units = 'F';	//set feet as unit
    					    float data = gps_data->altitude;
    					  
                            gps_data->altitude = data * (3.28084);	// convert altitude from meters to feet
                            #else
    						gps_data->altitude *= 1;
    					    #endif
    					}
    				  
     
                        if (fields[11] == NULL || strlen (fields[11]) == 0
    						 || !(is_valid_number (fields[11])))
    					{
    					    // If geoid height  field is invalid,empty then set any default value
    						gps_data->geoid_height = DEFAULT_GEOID_HEIGHT;
    						printf("GEOIDAL SEPARATION field is invalid ,setting to default value of 0\n");
    					}
    				  
                        else{
    					  
                            gps_data->geoid_height = strtof (fields[11], NULL);
    					}
    				  
     
                        if (fields[12] == NULL || strlen (fields[12]) == 0
    						 || (fields[12][0] != 'M'))
    					{
    					    // If the Mean Sea Level Altitude unit  field is invalid,empty then set to -  or any default character
    						gps_data->geoid_height_units = DEFAULT_GEOID_HEIGHT_UNITS;
     
    						
    					}
    				  
                        else{
    					  
                            gps_data->geoid_height_units = fields[12][0];	// meter 
    					  
                            #if USE_FEET_UNIT
    						gps_data->geoid_height_units = 'F';	// set feet as unit
    					    float data = gps_data->geoid_height;
    					    gps_data->geoid_height = data * (3.28084);	// convert geoid height from meters to feet
                            #else 
    						gps_data->geoid_height *= 1;
    					    #endif 
     
    					}
    				  
     
                        if (fields[13] == NULL || strlen (fields[13]) == 0
    						 || !(is_valid_number (fields[13])))
    					{
    					  
     
    						// If the differential gps age  field is invalid,empty then set to any default value
    						gps_data->dgps_age = DEFAULT_DGPS_AGE;
    					}
    				  
    				    else{
    					  
                            gps_data->dgps_age = strtof (fields[13], NULL);	// age in seconds
    					}
    				  
                        if (fields[14] == NULL || strlen (fields[14]) == 0
    						 || !(is_valid_number (fields[14])))
    					{
    					    // If the Station ID   field is invalid,empty then set to any default value
    						gps_data->dgps_station_id = DEFAULT_DGPS_STATION_ID;
    						
    					}
    				  
    				    else{
    					  
                            gps_data->dgps_station_id = atoi(fields[14]);	// ID in numbers from 0 to 1023
    				        
    				   
    				    } 
     
                    }
    			  
                	// if field count is invalid then print default values
    		    	else{
    				     printf("GGA sentence has an invalid number of fields. Resetting to default values.\n");
    					 print_default_value (gps_data);
    					 
    		    	}
			
            }
		  
            else{
			        ESP_LOGE (TAG, "Invalid CheckSum");
			    	// The checksum is invalid, so return default GPS data
				    print_default_value (gps_data);
				    
			}
		
        }
	  
        else{
    		  ESP_LOGE (TAG, "Invalid NMEA 0183 Sentence");
    		  // The sentence format is not according to GPGGA sentence, so return default GPS data
    		  print_default_value (gps_data);
    		  
    		}
	free(temp_buffer); // release allocated dynamic memory to avoid memory leaks 
	temp_buffer = NULL;
	
	
	}
  
    else{
	       ESP_LOGE (TAG, "Invalid Input String");
	  
		// The stream is invalid (either NULL or empty), so return default GPS data
		print_default_value (gps_data);
		
	}
  
    // Return the GPS data structure (either populated or default)
	return gps_data;

 
}

//====================================================================================================================================================================================================================================================================
//                         Library Functions Definitions
//====================================================================================================================================================================================================================================================================

/**
 * @brief Checks the validity of a UART stream.
 *
 * @param uart_stream The UART stream to check.
 * @return Returns 1 if the stream is invalid (NULL or empty), otherwise 0.
 */ 
int check_stream_NULL_Empty (const char *uart_stream) 
{ 
    return ((uart_stream == NULL) || (uart_stream[0] == '\0')) ? 1 : 0;	// Return 1 if the stream is NULL,empty  otherwise return 0
}

/**
 * @brief Checks the validity of format of NMEA string.
 *
 * @param uart_stream The UART stream to check.
 * @return Returns starting index of $GPGGA sentence if it finds GGA sentence and also it finds CRLF at end of GGA sentence and no $ in between which can occurs if there is power instability to GPS module,otherwise it returns -1 if not valid GGA sentence format.
 */ 
  
 
int gga_sentence_format_validity_check (const char *uart_stream)
{
  const char *substring_gga = strstr (uart_stream, "$GPGGA,");	// Check if the substring "$GPGGA," is found
  if (substring_gga == NULL)
	{
	  printf ("Error: GGA sentence not found in uart_stream.\n");
	  return -1;
	}
  
  // Move past "$GPGGA," and check for "\r\n"
  const char *rn_string = strstr (substring_gga + 14, "\r\n");	// move 14 characters(commas)  such a case where all fields are empty
  if (rn_string == NULL)
	{
	  printf ("Error: Expected \\r\\n not found after GGA sentence.\n");
	  return -1;
	}
  
    int gga_pos = (substring_gga - uart_stream);
    s_crfl = (rn_string - uart_stream);	//position at which \r\n starts
    // Print the GGA sentence
    printf ("GGA sentence found: %.*s\n", (int) (rn_string - substring_gga + 2),substring_gga);
    
    // printf ("length of gga sentence is: %d\n", (rn_string - substring_gga));
    
    // free(temp); // Clean up allocated memory
	return gga_pos;	// Return index at which $GPGGA starts
}

/**
 * @brief Evaluates the checksum of an NMEA sentence.
 *
 * @param sentence The input NMEA sentence to evaluate.
 * @return Returns 1 if the calculated checksum matches the expected checksum (valid), otherwise returns 0 (invalid).
 */ 
int check_sum_evaluation (const char *sentence)
{
    // Initialize the calculated checksum variable
    unsigned char calculated_checksum = 0;
    int index = 0;
  
    // Iterate over the sentence starting after '$' until '*' or end of string
    // Calculate the checksum using XOR operation
    for (index = 1; sentence[index] != '*' && sentence[index] != '\0';
		 index++)
	{
	  calculated_checksum ^= sentence[index];
	}
  
    // Check if we reached the end of the sentence without finding '*'
	// Return 0 if '*' was not found, indicating invalid checksum
	if (sentence[index] != '*'){
	  return 0;
	}
  
    // Move past the asterisk to the expected checksum part of the sentence
    const char *checksum_ptr = sentence + index + 1;
  
    // Parse the expected checksum from the sentence (2 hex digits)
    unsigned int expected_checksum;
    
    if (sscanf (checksum_ptr, "%2X", &expected_checksum) != 1){
	  
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
int is_valid_numeric (const char *str, int expected_length)
{
    if (str == NULL || str[0] == '\0')
	    return 0;
    // Initialize variables to track the number of dots and the length of the string
    int dot_count = 0;
  
    int length = 0;
    int d_length = 0;
    char output1[4] = { 0 };
    char output2[3] = { 0 };
    int value = 0;
  
    if (expected_length == 5){
	  
        sprintf (output1, "%.3s", str);
	    value = atoi (output1);
	}
  
    if (expected_length == 4){
	    sprintf (output2, "%.2s", str);
	    value = atoi (output2);
	}
  
    // Iterate through the string
	while (*str)
	{
	  
		// Check if the current character is not a digit
		if (!isdigit ((int) *str))
		{
		  
			// Check if the character is a dot and if no dot has been found yet
			if (*str == '.' && dot_count == 0)
			{
			    d_length = length;
			    dot_count++;
			}
		    else{
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
	if (d_length != expected_length){
	 
	    return 0; // Invalid length
	}
  
    if ((value > 180) && (expected_length == 5)){
	  
	    return 0;
	}
  
    if ((value > 90) && (expected_length == 4)){
	  
        return 0;
	}
  
 
	// Return 1 if all characters are valid and length is correct
	return 1;
}

// Function to validate a time field in HHMMSS.SSS format
  
int is_valid_time (const char *time)
{
    if (time == NULL)
	    return 0;
    // First, check if the time format is valid and all characters are digits
	for (int i = 0; i < 7; i++)
	{
	    if (i == 6)
		{
		    if (time[i] != '.')
		    {
			  
                return 0;// Time should have a decimal point at position 6
			}
		
        }
	    else if (!isdigit ((int) time[i])){
		    
		    return 0;	// Invalid character found
		}
	
    }
  
    // Parse hours, minutes, seconds from the time string
    char hr_str[3], min_str[3], sec_str[3];
  
    strncpy (hr_str, time, 2);
    strncpy (min_str, time + 2, 2);
    strncpy (sec_str, time + 4, 2);
      
    hr_str[2] = min_str[2] = sec_str[2] = '\0';
      
    int hr = atoi (hr_str);
    int min = atoi (min_str);
    int sec = atoi (sec_str);
  
    // Validate the parsed values
	if (hr > 23 || min > 59 || sec > 59 )
	    
	    return 0;
    // All checks passed, return 1 indicating the time is valid
	return 1;

}

// To check if given string is a number 
 
int is_valid_number(const char *str)
{
    int decimal_point_count = 0; // To count the number of decimal points
    int is_negative_allowed = (str == fields[9] || str == fields[11]); // Check if the field is altitude or geoid height

    // Check for a negative sign at the beginning of the string if allowed
    if (is_negative_allowed && *str == '-')
    {
        str++;
    }

    // Iterate through each character in the string
    while (*str)
    {
        // If the character is a decimal point, increment the count
        if (*str == '.')
        {
            decimal_point_count++;
            // If more than one decimal point is found, return 0 (false)
            if (decimal_point_count > 1)
            {
                return 0;
            }
        }
        // If any character is not a digit or a decimal point, return 0 (false)
        else if (!isdigit((unsigned char)*str))
        {
            return 0;
        }
        str++;
    }

    // If all characters are digits or there is a single decimal point, return 1 (true)
    return 1;
}
// Function to set default values for gps_data_parse_t structure
void print_default_value (gps_data_parse_t * data)
{
    printf("\n_____INVALID DATA STREAM______\n setting all parameters to their default values\n");
	
    data->time.hour = DEFAULT_GPS_TIME_HR;
      
    data->time.minute = DEFAULT_GPS_TIME_MIN;
      
    data->time.second = DEFAULT_GPS_TIME_SEC;
      
    data->time.millisecond = DEFAULT_GPS_TIME_MS;
      
    data->latitude = DEFAULT_LATITUDE;
      
    data->longitude = DEFAULT_LONGITUDE;
      
    data->lat_direction = DEFAULT_LAT_DIRECTION;
      
    data->lon_direction = DEFAULT_LON_DIRECTION;
      
    data->fix_quality = DEFAULT_FIX_QUALITY;
      
    data->num_satellites = DEFAULT_NUM_SATELLITES;
      
    data->hdop = DEFAULT_HDOP;
      
    data->altitude = DEFAULT_ALTITUDE;
      
    data->altitude_units = DEFAULT_ALTITUDE_UNITS;
      
    data->geoid_height = DEFAULT_GEOID_HEIGHT;
      
    data->geoid_height_units = DEFAULT_GEOID_HEIGHT_UNITS;
      
    data->dgps_age = DEFAULT_DGPS_AGE;
      
    data->dgps_station_id = DEFAULT_DGPS_STATION_ID;
} 
 
void utc_time_parser (gps_data_parse_t * gps_time) 
{ 
    const char *time_str = fields[1];
  
    // Extract and convert hour
	gps_time->time.hour = TIME_ZONE + (10 * (time_str[0] - '0') + (time_str[1] - '0'));
  
    // Extract and convert minute
	gps_time->time.minute = 10 * (time_str[2] - '0') + (time_str[3] - '0');
  
    // Extract and convert second
	gps_time->time.second = 10 * (time_str[4] - '0') + (time_str[5] - '0');
    
    // Extract and convert milliseconds
     gps_time->time.millisecond = 0; // Initialize to 0
     int factor = 100; // Start with the highest place value
     for (int i = 7; i <= 9 && time_str[i] != '\0'; i++) {
            gps_time->time.millisecond += (time_str[i] - '0') * factor;
            factor /= 10;
        }
    
    
    
} 

//Function to convert longitude and latitude into degrees 
 
float longitude_latitude_parser (const char *str) 
{ 
    float x = strtof (str, NULL);
    int deg = ((int) x) / 100;
    float min = x - (deg * 100);
    x = deg + min / 60.0f;
  
    return x;
    
}

//Function to describe fix quality value
void gps_fix_quality_description (int gps_quality_fix)
{
  
    switch (gps_quality_fix){
	
        case 0:
        	  printf("0: Fix not available - The GPS receiver does not have a valid fix.\n");
        	  break;
        case 1:
        	  printf("1: GPS fix - The GPS receiver has a valid fix using standard GPS satellites.\n");
        	  break;
        case 2:
        	  printf("2: DGPS fix - The GPS receiver has a valid fix using Differential GPS correction.\n");
        	  break;
        case 3:
        	  printf("3: PPS fix - The GPS receiver has a valid fix using Precise Positioning Service.\n");
        	  break;
        case 4:
        	  printf("4: Real Time Kinematic - The GPS receiver has a valid fix using Real Time Kinematic positioning.\n");
        	  break;
        case 5:
        	  printf("5: Float RTK - The GPS receiver has a float RTK fix, which is less accurate than a full RTK fix.\n");
        	  break;
        case 6:
        	  printf("6: Estimated (dead reckoning) - The GPS receiver is using dead reckoning to estimate position.\n");
        	  break;
        case 7:
        	  printf("7: Manual input mode - The GPS receiver's position is being manually inputted.\n");
        	  break;
        case 8:
        	  printf("8: Simulation mode - The GPS receiver is operating in simulation mode.\n");
        	  break;
        default:
        	  printf("%d: Unknown Quality Indicator - The provided quality indicator is not recognized.\n",gps_quality_fix);
        	  break;
    }

}







//====================================================================================================================================================================================================================================================================
//                         Test Functions Definitions
//====================================================================================================================================================================================================================================================================


// these are wrapper around functions for local function in order to access them indirectly for unit tests

 
 
 
 
 
