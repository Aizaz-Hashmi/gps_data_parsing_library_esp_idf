## GPS Data Parsing Library for ESP-IDF

A lightweight external library for ESP32, written in C/C++, using ESP-IDF v5.0 or later for parsing GPS data in NMEA format (e.g., NMEA 0183). This library parses the GGA sentence, providing information such as time, longitude, latitude, and other data. The project is compatible with Espressif's Eclipse-based IDE and can be imported directly.

### GPS Data Parser Component

The `gps_data_parser` component processes GPS data packets received via UART, extracting relevant information like time, latitude, longitude, and altitude. Its structure includes:

- **`gps_data_parser.h`**: Declares function to parse GPS data and defines a structure for data parameters.
- **`gps_data_parser.c`**: Implements functions to parse, validate, and process the NMEA sentence through UART stream.

### CMake Configuration

- The `gps_data_parser.c` source file is registered in the component folder's `CMakeLists.txt`.
- The main folder's `CMakeLists.txt` specifies the requirement for the `gps_data_parser` component for proper integration.