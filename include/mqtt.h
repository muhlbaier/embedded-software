#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>
#include "aws_iot_mqtt_client.h"
#include "aws_iot_error.h"

#define MQTT_CONNECT_MAX_ATTEMPT_COUNT 3

// every 100 ms yield 10ms to the MQTT client service
#define MQTT_YIELD_TIME 10
#define MQTT_YIELD_PERIOD 100

/**
* Initalize the MQTT Device
*/
IoT_Error_t MQTT_Init(void);

/**
* Subscribe to a topic
* 
* @param topic - Topic to subscribe to
* @param qos - Quality of Service of the MQTT connection
* @param callback - Handles receiving data from the broker 
* @param pData - Pointer to data that gets sent to the callback handler (make it NULL / 0 if not used)
*/
IoT_Error_t MQTT_Subscribe(char* topic, QoS qos, pApplicationHandler_t callback, char* pData);

/**
* Publish to a topic
*
* @param topic - Topic to publish to
* @param qos - Quality of service of the MQTT connection
* @param pData - Data being published
* @param dLen - Length of the data
*/
IoT_Error_t MQTT_Publish(char* topic, QoS qos, char* pData, size_t dLen);

/**
* Unsubscribe from a topic
* 
* @param topic - The topic to unsubscribe from
*/
IoT_Error_t MQTT_Unsubscribe(char* topic);


#endif
