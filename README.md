## GPS Data Parsing Library for ESP-IDF

A lightweight external library for ESP32, written in C/C++, using ESP-IDF v5.0 or later for parsing GPS data in NMEA format (e.g., NMEA 0183). This library parses the GGA sentence, providing information such as time, longitude, latitude, and other data. The project is compatible with Espressif's Eclipse-based IDE and can be imported directly.

### GPS Data Parser Component

The `gps_data_parser` component processes GPS data packets received via UART, extracting all data elements like time, latitude, longitude, and altitude,hdop,fix quality, number of satellites, geiod height, age of DGPS Its structure includes:

- **`gps_data_parser.h`**: Declares function to parse GPS data and defines a structure for data parameters.
- **`gps_data_parser.c`**: Implements functions to parse, validate, and process the NMEA sentence through UART stream.

### CMake Configuration

- The `gps_data_parser.c` source file is registered in the component folder's `CMakeLists.txt`.
- The main folder's `CMakeLists.txt` specifies the requirement for the `gps_data_parser` component for proper integration.




### Declaration of Structure and Function in `gps_data_parser.h` Header File

- The `gps_data_parse_t` structure has been declared to store GGA sentence data, such as time, latitude, longitude, and altitude and all data members. There is another structure  `gps_time_t` for time formatting and is a member of `gps_data_parse_t` This structure serves as a comprehensive data container for parsed GPS information, facilitating easy access and processing in your application.

- Additionally, the `gps_data_parser` function has been declared. This function takes an NMEA packet (UART stream) as input and returns a pointer to the `gps_data_parse_t` structure populated with parsed GPS data. It can be called from the main function or any other part of your code, providing a straightforward way to process GPS data and integrate it into your application.



### `gps_data_parser` Function Definition

The `gps_data_parser` function is responsible for parsing GPS data packets received via UART, focusing on GGA sentences. This function processes the input stream and extracts vall  GPGGA  data fields such as time, latitude, longitude, and altitude etc.

This function returns , a handle of type `gps_data_parse_t *` is. When a pointer object of this struct type is instantiated, it is assigned to a dynamically allocated memory location using the `malloc` function. It is crucial in application code to manage this dynamically allocated memory properly by freeing it using `free()` when it is no longer needed. Failure to do so can lead to memory leaks, where memory is allocated but never released, consuming system resources unnecessarily.
#### Step 1: Input Validation

Upon receiving an input string (`uart_stream`), the function first validates the input using the `check_stream_NULL_Empty` function. This function checks whether the input is either `NULL` or contains ASCII data.

- If the input stream is invalid (i.e., `NULL`), the function will return default values for `time`, `longitude`, `latitude`, and `altitude` and all other elements (such as -1, any character symbol or zero), depending on your preference.Default values can be changed from `gps_data_parser.h` file.
- By verifying the input early and returning default values if necessary, the function handles invalid input gracefully, ensuring predictable behavior even in the presence of invalid input.

#### Step 2: GPGGA Sentence Validation

Checking Stream Validity and Extracting GGA Sentence
Upon receiving an input UART stream, the function checks if the stream is valid and contains a valid GGA sentence using the function `gga_sentence_format_validity_check`.

##### 1. Extracting GGA Sentence:
- If a valid GGA sentence is found, the function extracts the GGA sentence from the UART stream and stores it in a temporary buffer.
Processing and Handling Default Values

##### 2. Processing Extracted GGA Sentence:
- Parsing logic can be added to process the extracted GGA sentence from the temporary buffer.

##### 3. Handling Invalid Stream:
- If no valid GGA sentence is found or the UART stream is invalid, the function initializes the gps_data variable with default values such as -1 or 0 etc.

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
  - Extracts and formats the time from `fields[1]` into `gps_data->time` using UTC time parser. If invalid, defaults to `255` for hour and other default values for minute, second, and millisecond.

- **Latitude**:
  - Processes latitude from `fields[2]` using `longitude_latitude_parser`. If invalid, sets `gps_data->latitude` to `-9999`.

- **Latitude Direction**:
  - Validates and sets latitude direction (`N` or `S`) from `fields[3]`. Defaults to a specified character if invalid.

- **Longitude**:
  - Processes longitude from `fields[4]` using `longitude_latitude_parser`. If invalid, sets `gps_data->longitude` to `0`.

- **Longitude Direction**:
  - Validates and sets longitude direction (`E` or `W`) from `fields[5]`. Defaults to `-` if invalid.

- **Fix Quality**:
  - Parses and sets fix quality from `fields[6]`. If invalid, defaults to `-1`.

- **Number of Satellites**:
  - Parses and sets the number of satellites from `fields[7]`. If invalid, defaults to `-1`.

- **Horizontal Dilution of Precision (HDOP)**:
  - Parses and sets HDOP from `fields[8]`. If invalid, defaults to `-1`.

- **Altitude**:
  - Parses and sets mean sea level altitude from `fields[9]`. If invalid, defaults to `-999999`.

- **Altitude Units**:
  - Validates and sets altitude units (`M` for meters) from `fields[10]`. Defaults to `-` if invalid. Converts altitude to feet if `USE_FEET_UNIT` is enabled.

- **Geoid Height**:
  - Parses and sets geoid height from `fields[11]`. If invalid, defaults to `0`.

- **Geoid Height Units**:
  - Validates and sets geoid height units (`M` for meters) from `fields[12]`. Defaults to `-` if invalid. Converts geoid height to feet if `USE_FEET_UNIT` is enabled.

- **Differential GPS Age**:
  - Parses and sets differential GPS age from `fields[13]`. If invalid, defaults to a specified default value.

- **Differential GPS Station ID**:
  - Parses and sets differential GPS station ID from `fields[14]`. If invalid, defaults to a specified default value.


### Function Definitions

1. **`static int check_stream_NULL_Empty(const char * uart_stream)`**
   - Checks if `uart_stream` is `NULL` or empty.
   - Returns `1` if `uart_stream` is `NULL` or empty, otherwise `0`.

2. **`static int gga_sentence_format_validity_check (const char *uart_stream)`**
   - Checks the validity of the GGA sentence format in `uart_stream`.
   - Returns `1` if the format is valid, otherwise `0`.

3. **`static int check_sum_evaluation (const char *sentence)`**
   - Evaluates the checksum of the sentence `sentence`.
   - Returns `1` if the checksum is valid, otherwise `0`.

4. **`static int is_valid_time (const char *time)`**
   - Checks if `time` is a valid UTC time format.
   - Returns `1` if `time` is valid, otherwise `0`.

5. **`static int is_valid_numeric (const char *str, int expected_length)`**
   - Checks if `str` is a valid numeric string of `expected_length`.
   - Returns `1` if `str` is valid, otherwise `0`.

6. **`static int is_valid_number (const char *str)`**
   - Checks if `str` represents a valid numeric value.
   - Returns `1` if `str` is a valid number, otherwise `0`.

7. **`static void print_default_value (gps_data_parse_t * data)`**
   - Prints default values for `data` in case of issues with the UART stream.

8. **`static void utc_time_parser (gps_data_parse_t * gps_time)`**
   - Parses UTC time format and sets it in `gps_time`.

9. **`static float longitude_latitude_parser (const char *str)`**
   - Parses `str` into latitude or longitude in degrees.
   - Returns the parsed value as a `float`.

10. **`void gps_fix_quality_description (int gps_quality_fix)`**
    - Provides a description of GPS fix quality based on `gps_quality_fix`.


### Calling `gps_data_parser` library function to test the library in main.c file
- Running multiple types of strings packets from invalid strings to valid NMEA 0183 GPGGA Sentences
- Checking how library function handles input packets corrupted due to noise, interference, hardware, and firmware issues
- If string is NULL or empty, to save power and processing , it is discarded at first phase
- If String has some data, it is checked to see if it is a valid NMEA Sentence
- If it is NMEA sentence then it is evaluated for checksum and is discarded if checksum is invalid
- If Checksum is valid, then it is parsed where actual processing occurs, where required data parameter is extracted as raw and processed to get actual values.
- If Checksum is valid when there is corruption in data before Checksum is performed by GPS module,or GPS module calcultes checksum after fetching data from Satellite and satellite 
  may not send all data parameters data due to some reasons ,the library function will assume it is valid data but to avoid getting wrong values as discussed earlier, the function 
  first counts number of fields which are 15 for GGA and then parses each field to check if any of the required field is empty, or has invalid characters, it will simply fill the 
  struct data paramters with the preferred string. I chose default but it can be set to any value to let the user know that it is not actual value.

### Addition of  Unit Testing
To incorporate target-based tests for testing this library , we can create a central unit test application running on the ESP32. 
  These tests use the  **`Unity unit test framework`** and can be integrated into an  the `gps_data_parser` library component by adding them to the component's `test` subdirectory.
  It verifies if the developed library remains robust and well-tested as you develop and integrate new features.
  
- Creation of `test` subdirectory in `components/gps_data_parser`. The `gps_data_parser/test`has
` test_gps_data_parser.c` file, `CMakeLists.txt`file in which type `gps_data_parser` in front of  **`REQUIRES`** section and `untiy` in front of **`PRIV_REQUIRES`** part. Third file is `component.mk` file which is Makefile to instruct linker to achieve unit testing.
- `test_gps_data_parser.c` has all test cases ranging from NORMAL TEST CASES to challenging edge cases to test this library's reliabilty.
- To make use of these test cases written in the library subdirectory test folder, it is better to run them alone without merging the application code in `main.c` file in `main` folder in root directory.So, a new program entry point si required which is achieved by creating a test folder in the root directory.
- `test` folder has two subfolders `main` and `build`. 
- `test` folder has `CMakeLists.txt` file and `sdkconfig`file.
-  In `ESP-IDF Termianl`goto test root directory by typing the `cd project/test` use `idf.py build` to build all the files that are to be built. use `idf.py -p COMx flash monitor` to see test results as PASS or FAIL and in `test/main/gps_data_parsing_library_test.c` it has `unity.h`  header file which is necessary to use run our tests. UNIT TEST is a frame wrork provided by UNITY. Most of its files are in `assembly` because they directly communicate with target hardware.
-  use `unity_run_menu` to run the test cases in the form of a menu and it is upto you which test you want to run and see results.
-  If all tests results are PASS then library can be considerd reliable with no loop holes.


### Test Cases

The project includes the following test cases:

1. **Valid GPGGA Sentence**: Tests the parser's ability to handle a valid GPGGA packet and extract the expected GPS data fields.

2. **Incorrect Sentence Identifier**: Verifies that the parser discards packets with invalid sentence identifiers and returns defaults for all fields.
   
3. **Empty Sentence**: Tests the parser's handling of an empty input sentence, expecting all fields to be assigned defaults.

4. **Only Identifier**: Tests the parser's handling of input with only the sentence identifier (no data fields) and expects all fields to be defaults.

5. **Missing Fields**: Verifies the parser's ability to handle input with missing fields and expects all fields to be defaults.

6. **Incorrect Data Format**: Ensures the parser handles input with unexpected data format by discarding the sentence and setting all fields to defaults.

7. **Incorrect Checksum**: Verifies the parser discards sentences with incorrect checksums and sets all fields to defaults.

8. **Out of Range Latitude and Longitude Values**: Tests how the parser handles input with latitude and longitude values out of valid range, expecting default for those fields.

9. **Invalid Packet Format**: Tests the parser's handling of invalid packet format, expecting all fields to be defaults.

10. **Corrupted Data**: Verifies the parser discards sentences with corrupted data and sets all fields to default.

11. **Sentence with Excessive Length**: Tests the parser's handling of input sentences that exceed the expected length, expecting all fields to be default.
12. **GPGGA Stream Empty or NULL**: Verifies handling of NULL or empty streams, expecting function to return 1, and 0 for non-empty streams.

13. **GGA Sentence Format Validity Check**: Tests the validation of GGA sentence format, expecting proper handling of valid and invalid formats, including proper index returns for valid formats.

14. **Checksum Evaluation**: Validates that the calculated checksum matches the expected checksum, returning 1 for matches.

15. **Time Field Validity**: Checks the validity of time fields in the format HHMMSS.sss, expecting 1 for valid times and 0 for invalid times.

16. **Numeric String Validation**: Verifies that strings consist only of digits, a decimal point, and optionally a negative sign, returning 1 for valid numeric strings and 0 for invalid ones.

17. **Latitude and Longitude Format Check**: Tests the validity of latitude and longitude fields, expecting 1 for valid formats (4 digits for latitude, 5 digits for longitude) and 0 for invalid ones.

18. **Latitude and Longitude Processor**: Ensures correct processing of latitude and longitude fields into degrees, expecting accurate conversion or 0.0 for empty or NULL fields.

### Project Layout:

Below is a diagram that illustrates the structure of the project:

```plaintext
gps_data_parsing_library/
├── Binaries/
├── Archives/
├── build/
├── components/
│   └── gps_data_parser_folder/
│       ├── include/
│       │   └── gps_data_parser.h
│       ├── src/
│       │   └── gps_data_parser.c
│       ├── test/
│       │   └── test_gps_data_parser.c
│       └── CMakeLists.txt
├── main/
│   ├── main.c
│   └── CMakeLists.txt
│   
├── test/
│   ├── build/
│   ├── main/
│   │   └── test.c
│   └── CMakeLists.txt
├── sdkconfig
├── README.md
├── LICENSE
└── CMakeLists.txt
```

### Root Directory

- **Binaries/**: Contains firmware and other binary files.
- **Archives/**: Contains archived files.
- **build/**: Build output files (object files, binaries).
- **components/**: Contains various components of the project, including:
    - **gps_data_parser_folder/**
        - **include/**: Header files (.h).
        - **src/**: Source files (.c).
        - **test/**: Test cases and test scripts.
        - **CMakeLists.txt**: CMake build script for the `gps_data_parser_folder` component.
- **main/**: Contains the main source code and build script:
    - **main.c**: Main source code file.
    - **CMakeLists.txt**: CMake build script for the `main` folder.
    - **kconfigproj**: Kconfig project configuration file.
- **test/**: Contains test-related directories and files:
    - **build/**: Build output for tests.
    - **main/**: Main test directory containing test scripts.
    - **test.c**: Test cases file.
    - **CMakeLists.txt**: CMake build script for the `test` folder.
- **sdkconfig**: SDK configuration file.
- **README.md**: This README file.
- **LICENSE**: License file for the project.
- **CMakeLists.txt**: Root-level CMake build script.

