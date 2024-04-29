/**
 * @file gps_data_parser.c
 * @brief Parses GPS data packets, extracting relevant information (time, latitude, longitude, altitude, etc.).
 *
 * Processes, validates, and extracts data from NMEA 0183 sentences sent via UART from GPS module to ESP32 MCU.
 *
 * Created on: 28-Apr-2024
 * Author: Aizaz Ubaid Hashmi
 */

#include "gps_data_parser.h"
