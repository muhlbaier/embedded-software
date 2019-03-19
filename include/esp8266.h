/** @file
 * @addtogroup esp8266 ESP8266 WiFi
 *
 * @ brief Simplex communication with ESP8266 WiFi module using AT commands
 * UART must be initialized and configured to use backchannel before using 
 * functions from this module.
 * 
 * ESP8266_UART must be defined in project_settings.h
 * 
 *  Created on: March 18, 2019
 *      Author: Stephen Glass
 * 
 * 
 * Example psuedo code:
 * (Note the task scheduelr may need to be used to add delays
 * between sending commands to ESP8266 because the device still
 * needs to process it)
 * void main() {
 *      UART_Init();
 *      ESP8266_Reset();
 *      ESP8266_SetMode(MODE_STATION, 0);
 *      ESP8266_ConnectNetwork("Rowan_IoT", "", 0);
 *      ESP8266_HTTP_GET("127.0.0.1", 80, "/api/callback.php", "?data=10");
 * }
 *
 * @{
 * 
 * 
 */

#ifndef _ESP8266_H_
#define _ESP8266_H_

#define MODE_STATION            1
#define MODE_SOFTAP             2
#define MODE_SOFTAP_STATION     3

#include <stdint.h>
#include "project_settings.h"

#ifndef USE_MODULE_TASK
#warning "USE_MODULE_TASK not defined in project_settings.h. Other modules won't be able to utilize this module."
#endif

#ifndef USE_MODULE_BUFFER_PRINTF
#error "This module requires optional UART functionality provided by buffer_printf. Please declare USE_MODULE_BUFFER_PRINTF in project_settings.h"
#endif

/** Configure the WiFi network
 *
 * @param network_ssid name of the network SSID
 * @param network_pass password of the network
 * @param flash_settings flash the settings to the esp8266 (0, no; 1, flash)
 * Setting to flash will automatically connect every time ESP8266 is powered on
 */
void ESP8266_ConnectNetwork(char * network_ssid, char * network_pass, uint8_t flash_settings);

/** Set the mode of the ESP8266
 *
 * @param mode mode to set
 * @param flash_settings flash the setting to esp8266 (0, no; 1, flash)
 * Setting to flash will automatically enable this mode every time ESP8266 is powered on
 */
void ESP8266_SetMode(uint8_t mode, uint8_t flash_settings);

/** Resets the ESP8266
 */
void ESP8266_Reset(void);

/** Performs a HTTP GET request on a specified URL with payload
 *
 * @param hostname hostname in format of IP address or URL (e.g 127.0.0.1, localhost)
 * @param port port of the HTTP server (default 80 on most servers)
 * @param uri uri of the server (e.g: /api/test.php)
 * @param payload payload to send (e.g: ?data=1&id=5)
 */
void ESP8266_HTTP_GET(char * hostname, int port, char * uri, char * payload)

/** Helper function to calculate length of string
 *
 * @param s string to calculate length
 */
int ESP8266_CalcLength(const char * s);

#endif