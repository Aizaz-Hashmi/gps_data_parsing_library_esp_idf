## GPS Data Parsing Library for ESP-IDF

A lightweight external library for ESP32, written in C/C++, using ESP-IDF v5.0 or later for parsing GPS data in NMEA format (e.g., NMEA 0183). This library parses the GGA sentence, providing information such as time, longitude, latitude, and other data. The project is compatible with Espressif's Eclipse-based IDE and can be imported directly.

### GPS Data Parser Component

The `gps_data_parser` component processes GPS data packets received via UART, extracting relevant information like time, latitude, longitude, and altitude. Its structure includes:

- **`gps_data_parser.h`**: Declares function to parse GPS data and defines a structure for data parameters.
- **`gps_data_parser.c`**: Implements functions to parse, validate, and process the NMEA sentence through UART stream.

### CMake Configuration

- The `gps_data_parser.c` source file is registered in the component folder's `CMakeLists.txt`.
- The main folder's `CMakeLists.txt` specifies the requirement for the `gps_data_parser` component for proper integration.




### Declaration of Structure and Function in `gps_data_parser.h` Header File

- The `gps_data_parse_t` structure has been declared to store GPS data, such as time, latitude, longitude, and altitude. This structure serves as a comprehensive data container for parsed GPS information, facilitating easy access and processing in your application.

- Additionally, the `gps_data_parser` function has been declared. This function takes an NMEA packet (UART stream) as input and returns a `gps_data_parse_t` structure populated with parsed GPS data. It can be called from the main function or any other part of your code, providing a straightforward way to process GPS data and integrate it into your application.



### `gps_data_parser` Function Definition

The `gps_data_parser`  function is responsible for parsing GPS data packets received via UART, specifically focusing on GGA sentences. This function processes the input stream and extracts relevant GPS data such as time, latitude, longitude, and altitude.

#### Step 1: Input Validation

Upon receiving an input string (`uart_stream`), the function first validates the input using the `check_stream_validity` function. This function checks whether the input is either `NULL` or contains ASCII data.

- If the input stream is invalid (i.e., `NULL`), the function will return default values for `time`, `longitude`, `latitude`, and `altitude` (such as "N/A" or zero), depending on your preference.
- By verifying the input early and returning default values if necessary, the function handles invalid input gracefully, ensuring predictable behavior even in the presence of invalid input.

#### Step 2: GPGGA Sentence Validation

Here is the information formatted as requested:

Checking Stream Validity and Extracting GGA Sentence
Upon receiving an input UART stream, the function checks if the stream is valid and contains a valid GGA sentence using the function `gga_sentence_format_validity_check`.

##### 1. Extracting GGA Sentence:
- If a valid GGA sentence is found, the function extracts the GGA sentence from the UART stream and stores it in a temporary buffer.
Processing and Handling Default Values

##### 2. Processing Extracted GGA Sentence:
- Parsing logic can be added to process the extracted GGA sentence from the temporary buffer.

##### 3. Handling Invalid Stream:
- If no valid GGA sentence is found or the UART stream is invalid, the function initializes the gps_data variable with default values such as "N/A".

#### Step 3: checksum verification for data integrity
This process ensures that the data packet has not been corrupted during transmission and that the GPS data can be trusted for further processing. Implementing checksum evaluation verification is crucial for maintaining the reliability of GPS data parsing in your application.
##### Initialization:
- Initialize the calculated checksum variable (calculated_checksum) to zero.

##### Iterate Through Sentence:
- Start iteration from the character after $ until the character before * or the end of the sentence ('\0').
- Compute the XOR operation for each character in the range to calculate the checksum.

##### Check for Asterisk:
- Ensure the iteration ends at the asterisk (*).
- If the asterisk is not found, return 0 (invalid checksum).

##### Extract Expected Checksum:
- After finding the asterisk, move past it to extract the expected checksum, which is represented as a hexadecimal string of two characters.

##### Parse Expected Checksum:
- Parse the expected checksum using sscanf to convert the hexadecimal string to an integer.
##### Compare Checksums:
- Compare the calculated checksum with the expected checksum.
Return 1 if they match (valid checksum), otherwise return 0 (invalid checksum).


#### Step 4:  NMEA GPGGA  sentence Data Parsing and Processing

This code provides functions for parsing GPS data packets in NMEA format GGA sentences and extracting key information such as time, latitude, longitude, and altitude and processing this data in presentable form to be displayed.

##### Main Parsing Block (`if (field_count == 15)`)

In the main parsing block, the code processes the parsed fields from a GGA sentence:

- **Time**: 
    - The function starts by checking the time field (`fields[1]`) using `is_valid_time()`.
    - If the time field is valid, the time is formatted as "HH:MM:SS.SSS" using `sprintf()` and stored in `gps_data.time`.
    - If the time field is invalid, the function sets `gps_data.time` to "N/A" to indicate the absence of valid data.

- **Latitude**:
    - The latitude value is obtained from `fields[2]` and its direction from `fields[3]`.
    - The function checks if the latitude is valid and if the direction is a valid compass direction (N or S) using `is_valid_numeric()` and `is_valid_direction()`.
    - If both latitude and direction are valid, the function converts the latitude from degrees/minutes to degrees and fractional minutes, then formats it as "DD MM.MMMM D" (e.g., "12 55.7174 N").
    - The formatted latitude is stored in `gps_data.latitude`.
    - If the latitude or direction is invalid, "N/A" is stored.

- **Longitude**:
    - The longitude value is obtained from `fields[4]` and its direction from `fields[5]`.
    - Similar to latitude, the function validates longitude using `is_valid_numeric()` and `is_valid_direction()`.
    - If both longitude and direction are valid, the function converts the longitude from degrees/minutes to degrees and fractional minutes, then formats it as "DDD MM.MMMM D" (e.g., "077 37.2052 E").
    - The formatted longitude is stored in `gps_data.longitude`.
    - If the longitude or direction is invalid, "N/A" is stored.

- **Altitude**:
    - The altitude value is obtained from `fields[9]` and its unit (e.g., meters) from `fields[10]`.
    - The function checks if the altitude is valid using `is_valid_altitude()` and if the unit field contains a valid character.
    - If the altitude and unit are valid, the function formats the altitude as "###.# U" (e.g., "333.2 M").
    - The formatted altitude is stored in `gps_data.altitude`.
    - If the altitude or unit is invalid, "N/A" is stored.

### Function Definitions

#### `is_valid_numeric(const char *str, int expected_length)`
- This function checks if a given string consists only of digits and at most one dot (.), and whether its length matches the expected length.
- The function iterates through each character in the input string (`str`).
- If a dot is encountered, it checks if it is the first dot encountered. If not, the function returns `0` (invalid).
- The function counts the length of the string, and if it doesn't match `expected_length`, it returns `0` (invalid).
- If all checks pass, it returns `1` (valid).

#### `is_valid_altitude(const char *str)`
- This function validates an altitude string, checking that it contains only digits and at most one dot (.).
- Similar to `is_valid_numeric()`, it iterates through the string and verifies the content.
- It returns `1` if the string is valid and `0` if not.

#### `is_valid_time(const char *time)`
- This function validates a time string in the "HHMMSS.SSS" format.
- It checks each character of the string, ensuring that all but the sixth character (which should be a decimal point) are digits.
- If the format is valid, it returns `1`, otherwise, it returns `0`.

#### `is_valid_direction(char direction)`
- This function checks if the input character is a valid compass direction (N, S, E, or W).
- It returns `1` if the direction is valid, otherwise it returns `0`.

### Calling `gps_data_parser` library function to test library in main.c file
- Running multiple types of strings packets from invalid strings to valid NMEA 0183 GPGGA Sentences
- Checking how library function handles input packets corrupted due to noise, interference, hardware, and firmware issues
- If string is NULL or empty, to save power and processing , it is discarded at first phase
- If String has some data, it is checked to see if it is a valid NMEA Sentence
- If it is NMEA sentence then it is evaluated for checksum and is discarded if checksum is invalid
- If Checksum is valid, then it is parsed where actual processing occurs, where required data parameter is extracted as raw and processed to get actual values.
- If Checksum is valid when there is corruption in data before Checksum is performed by GPS module,or GPS module calcultes checksum after fetching data from Satellite and satellite 
  may not send all data parameters data due to some reasons ,the library function will assume it is valid data but to avoid getting wrong values as discussed earlier, the function 
  first counts number of fields which are 15 for GGA and then parses each field to check if any of the required field is empty, or has invalid characters, it will simply fill the 
  struct data paramters with the preferred string. I chose "N/A" but it can be set to any value to let the user know that it is not actual value.
