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

The `gps_data_parser` function is responsible for parsing GPS data packets received via UART, specifically focusing on GGA sentences. This function processes the input stream and extracts relevant GPS data such as time, latitude, longitude, and altitude.

#### Step 1: Input Validation

Upon receiving an input string (`uart_stream`), the function first validates the input using the `check_stream_validity` function. This function checks whether the input is either `NULL` or contains ASCII data.

- If the input stream is invalid (i.e., `NULL`), the function will return default values for `time`, `longitude`, `latitude`, and `altitude` (such as "N/A" or zero), depending on your preference.
- By verifying the input early and returning default values if necessary, the function handles invalid input gracefully, ensuring predictable behavior even in the presence of invalid input.

#### Step 2: GPGGA Sentence Validation

Here is the information formatted as requested:

Checking Stream Validity and Extracting GGA Sentence
Upon receiving an input UART stream, the function checks if the stream is valid and contains a valid GGA sentence using the function gga_sentence_format_validity_check.

# Extracting GGA Sentence:
- If a valid GGA sentence is found, the function extracts the GGA sentence from the UART stream and stores it in a temporary buffer.
Processing and Handling Default Values

# Processing Extracted GGA Sentence:
- Parsing logic can be added to process the extracted GGA sentence from the temporary buffer.
# Handling Invalid Stream:
- If no valid GGA sentence is found or the UART stream is invalid, the function initializes the gps_data variable with default values such as "N/A".