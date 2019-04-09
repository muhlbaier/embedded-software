#include <strings.h>
#include "project_settings.h"
#include "esp8266.h"
#include "uart.h"
#include "task.h"

#ifndef ESP8266_UART
#error "You must define what UART number the terminal is using in project_settings.h: #define ESP8266_UART num"
#endif // because esp8266 is external device, this is usually 0 (not backchannel)

/** documentation for struct */
struct network_settings {
	char * ssid;
    char * pass;
    char * flash;
    uint8_t mode;
};
static struct network_settings network;

void ESP8266_ConnectNetwork(char * network_ssid, char * network_pass, uint8_t flash_settings) {
    network.ssid = network_ssid;
    network.pass = network_pass;
    network.flash = flash_settings;
    if(flash_settings) {
        UART_printf(ESP8266_UART, "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", network_ssid, network_pass);
    }
    else UART_printf(ESP8266_UART, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", network_ssid, network_pass);
}

void ESP8266_HTTP_GET(char * hostname, int port, char * uri, char * payload) {
    // send the TCP request
    UART_printf(ESP8266_UART, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", hostname, port);
    // sum the length of uri, payload, hostname and add 25 for edditional characters in message
    int length = ESP8266_CalcLength(uri) + ESP8266_CalcLength(payload) + ESP8266_CalcLength(hostname) + 25;
    UART_printf(ESP8266_UART, "AT+CIPSEND=%d\r\n", length); // send the length
    // send the actual GET request
    UART_printf(ESP8266_UART, "GET %s%s HTTP/1.0\r\nHost: %s\r\n\r\n", uri, payload, hostname);
}

void ESP8266_Reset() {
    UART_printf(ESP8266_UART, "AT+RST\r\n");
}

void ESP8266_SetMode(uint8_t mode, uint8_t flash_settings) {
    network.mode = mode;
    if(flash_settings) { // flashing to esp8266
        UART_printf(ESP8266_UART, "AT+CWMODE_DEF=%d\r\n", flash_settings);
    }
    else UART_printf(ESP8266_UART, "AT+CWMODE_CUR=%d\r\n", flash_settings);
}

int ESP8266_CalcLength(const char *s) {
    int resultLen = 0;
    while(*s) {
        resultLen++;
        *s++;
    }
    return resultLen;
}