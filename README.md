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

- The `gps_data_parse_t` structure has been declared to store GGA sentence data, such as time, latitude, longitude, and altitude and all data members. There is another structure  `gps_time_t` for time formatting and is a member of `gps_data_parse_t` This structure serves as a comprehensive data container for parsed GPS information, facilitating easy access and processing in your application.

- Additionally, the `gps_data_parser` function has been declared. This function takes an NMEA packet (UART stream) as input and returns a pointer to the `gps_data_parse_t` structure populated with parsed GPS data. It can be called from the main function or any other part of your code, providing a straightforward way to process GPS data and integrate it into your application.



### `gps_data_parser` Function Definition

The `gps_data_parser`  function is responsible for parsing GPS data packets received via UART, for GGA sentences. This function processes the input stream and extracts all GPS  GGA sentence data fields such as time, latitude, longitude, and altitude and so on.

#### Step 1: Input Validation

Upon receiving an input string (`uart_stream`), the function first validates the input using the `check_stream_NULL_Empty` function. This function checks whether the input is either `NULL` or contains ASCII data.

- If the input stream is invalid (i.e., `NULL`), the function will return default values for `time`, `longitude`, `latitude`, and `altitude` (such as -1, any character symbol or zero), depending on your preference.
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

2. **Incorrect Sentence Identifier**: Verifies that the parser discards packets with invalid sentence identifiers and returns "N/A" for all fields.

3. **GGA Sentence with Missing Parameters**: Ensures the parser can handle packets with missing fields and assigns "N/A" values to missing data.

4. **Empty Sentence**: Tests the parser's handling of an empty input sentence, expecting all fields to be assigned "N/A".

5. **Only Identifier**: Tests the parser's handling of input with only the sentence identifier (no data fields) and expects all fields to be "N/A".

6. **Missing Fields**: Verifies the parser's ability to handle input with missing fields and expects all fields to be "N/A".

7. **Incorrect Data Format**: Ensures the parser handles input with unexpected data format by discarding the sentence and setting all fields to "N/A".

8. **Incorrect Checksum**: Verifies the parser discards sentences with incorrect checksums and sets all fields to "N/A".

9. **Out of Range Latitude and Longitude Values**: Tests how the parser handles input with latitude and longitude values out of valid range, expecting "N/A" for those fields.

10. **Invalid Packet Format**: Tests the parser's handling of invalid packet format, expecting all fields to be "N/A".

11. **Corrupted Data**: Verifies the parser discards sentences with corrupted data and sets all fields to "N/A".

12. **Sentence with Excessive Length**: Tests the parser's handling of input sentences that exceed the expected length, expecting all fields to be "N/A".

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

