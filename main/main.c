/**
 * @file gps_data_parser.c
 * @brief Application entry point for processing GPS data packets.
 *
 * This file contains the entry point function that processes GPS data packets using
 * functions from the GPS data parser library.
 *
 * @created 28-Apr-2024
 * @author Aizaz Ubaid Hashmi
 */


#include <unistd.h>
#include <stdio.h>


#include <esp_log.h>
#include "gps_data_parser.h"

#define TAG "GPS"
void test(const char * stream, int stream_num);

/**
 * @brief Entry point for the application that processes GPS data packets.
 *
 * The function continuously processes a GPS data stream by calling the `test` function,
 * which parses the GPS data stream and outputs the parsed time, latitude, longitude, and altitude.
 * The loop runs indefinitely and processes different GPS data streams in each iteration.
 *
 * @return int Returns 0 on successful execution of the application.
 */
int app_main() {


	while(1) {

	// using test cases
    const char *stream1 = "";// empty string, invalid stream
    test(stream1,1);
    sleep(1);//1 second delay using POSIX API

    const char *stream2 = "$GPGGA,002153.000,1585.7149,N,07737.2052,E,1,4,5.57,333.2,M,-88.5,M,,*70\r\n";// pure GPGGA sentence with CRLF
    test(stream2,2);
    sleep(1);

    const char *stream3 = NULL;// NULL string
    test(stream3,3);
    sleep(1);

    const char *stream4 = "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*43\r\n$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75\r\n";// actual packet sample sent by  GPS module to mcu through UART
    test(stream4,4);
    sleep(1);

    const char *stream5 = ",09275,,,,,,,,,,ewrwer,sdfvzx,453453635,41,,,A*43\r\n$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75\r\n	$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A\r\n$GPGSV,AIZAZ UABID HASHMI 	59,290,20,08,54$GPGSV,3,2,11,02,39,223,16,13,28,070,17,26,SDFJDSNFJ23,252,,04,1SDJFNASJDKFSDJHDS4,186,15*77\r\n	$GPGSV,3,3,11,29,09,3JDSAFNJKDASFDSC01,24,16,09,020,,36,,,*76\r\n";// corrupted packet sample but uncorrupted GGA sent by  GPS module to mcu through UART
     test(stream5,5);
     sleep(1);

   // Now testing  when stream is valid , and it is NMEA sentence and even checksum result is true but some data parameters are missing
   // when time parameter is missing due to some issue but checksum attached with is without time field empty so checksum will be true
     const char * stream6 = "$GPGGA,,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*61\r\n"; // time is missing
     test(stream6,6);
     sleep(1);
     const char * stream7 = "$GPGGA,,,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*46\r\n"; // latitude  is also missing
     test(stream7,7);
     sleep(1);
     const char * stream8 = "$GPGGA,,,N,,W,1,8,1.03,61.7,M,55.2,M,,*58\r\n"; // longitude  is also missing
     test(stream8,8);
     sleep(1);
     const char * stream9 = "$GPGGA,,,N,,W,1,8,1.03,,M,55.2,M,,*46\r\n"; // altitude  is also missing
     test(stream9,9);
     sleep(1);

     const char * stream10 = "$GPGGA,123456.235,aaaa1as7,N,,W,1,8,1.03,61.7,M,55.2,M,,*51\r\n"; // latitude longitude data corrupted
     test(stream10,10);
     sleep(1);
     const char * stream11 = ",,,$$%%&^*(*(@,,,,4531313372,W,0.02,31.66,280511,,,A*43\r\n$GPGGA,065551.680,5321.6802,S,00630.3371,E,1,85,,,,554.3,M,,*30\r\n$GPGSA,A,3,10,07,05##(())(,1.38*0A\r%^&*()_+n$GPGSV,,./??3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70\r";//highly corrupted stream
     test(stream11,11);
     sleep(1);
	}
    return 0;
}

/**
 * @brief Tests the parsing of a GPS data stream.
 *
 * The function parses the given GPS data stream using `gps_data_parser`
 * and prints the parsed time, latitude, longitude, and altitude.
 *
 * @param stream The GPS data stream to parse and test.
 */
void test(const char* stream, int stream_num)
{

	    gps_data_parse_t data = gps_data_parser(stream);

	    // Using ESP-IDF logging function for each output
	    ESP_LOGI(TAG, "stream no. %d TIME: %s",stream_num, data.time);
	    ESP_LOGI(TAG, "stream no. %d LATITUDE: %s",stream_num, data.latitude);
	    ESP_LOGI(TAG, "stream no. %d LONGITUDE: %s", stream_num,data.longitude);
	    ESP_LOGI(TAG, "stream no. %d ALTITUDE: %s\n",stream_num, data.altitude);

}
