/**
  ******************************************************************************
  * @file    subscribe_publish_sensor_values.c
  * @author  MCD Application Team
  * @brief   Control of the measurement sampling and MQTT reporting loop.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "iot_flash_config.h"
#include "sensors_data.h"
#include "msg.h"
#include "timing.h"
#include "mqtt.h"
#include "task.h"

#ifndef AWS_SUCCESS
#define AWS_SUCCESS 0
#endif

// sensor app related functions
void init_sensor_subscribe(void);
void service_sensor_logic(void);

void MQTTcallbackHandler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData);
int subscribe_publish_sensor_values(void);

/* Private defines ------------------------------------------------------------*/
#define MQTT_CONNECT_MAX_ATTEMPT_COUNT 3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static char const * deviceName;
//static AWS_IoT_Client client;

/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
int cloud_device_enter_credentials(void)
{
  int ret = 0;
  iot_config_t iot_config;

  memset(&iot_config, 0, sizeof(iot_config_t));
    
  printf("\nEnter server address: (example: xxx.iot.region.amazonaws.com) \n");
  getInputString(iot_config.server_name, USER_CONF_SERVER_NAME_LENGTH);
  msg_info("read: --->\n%s\n<---\n", iot_config.server_name);
  
  printf("\nEnter device name: (example: mything1) \n");
  getInputString(iot_config.device_name, USER_CONF_DEVICE_NAME_LENGTH);
  msg_info("read: --->\n%s\n<---\n", iot_config.device_name);

  if(setIoTDeviceConfig(&iot_config) != 0)
  {
    ret = -1;
    msg_error("Failed programming the IoT device configuration to Flash.\n");
  }
  
  return ret;
}

bool app_needs_device_keypair()
{
  return true;
}




int subscribe_publish_sensor_values(void)
{
  IoT_Error_t rc;
  rc = MQTT_Init();
  init_sensor_subscribe();
  return 1;
}

/////////////// SENSOR APPLICATION RELATED STUFF ////////////////////
#define TIMER_COUNT_FOR_SENSOR_PUBLISH 10
/* Private variables ---------------------------------------------------------*/
static char ledstate[] = { "Off" };
static char cPTopicName[MAX_SHADOW_TOPIC_LENGTH_BYTES] = "";
static char cSTopicName[MAX_SHADOW_TOPIC_LENGTH_BYTES] = "";
#define aws_json_pre        "{\"state\":{\"reported\":"
#define aws_json_desired    "{\"state\":{\"desired\":"
#define aws_json_post       "}}"
/**
* @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
*/
static uint32_t publishCount = 60;

static char cPayload[AWS_IOT_MQTT_TX_BUF_LEN];
static IoT_Publish_Message_Params paramsQOS1 = {QOS1, 0, 0, 0, NULL,0};

void init_sensor_subscribe(void) {
  int i = 0;
  IoT_Error_t rc = FAILURE;
  snprintf(cPTopicName, sizeof(cPTopicName), AWS_DEVICE_SHADOW_PRE "%s" AWS_DEVICE_SHADOW_UPDATE_TOPIC, deviceName);
  snprintf(cSTopicName, sizeof(cSTopicName), AWS_DEVICE_SHADOW_PRE "%s" AWS_DEVICE_SHADOW_UPDATE_ACCEPTED_TOPIC, deviceName);
  //rc = aws_iot_mqtt_subscribe(&client, cSTopicName, strlen(cSTopicName), QOS0, MQTTcallbackHandler, NULL);
  rc = MQTT_Subscribe(cSTopicName, QOS0, MQTTcallbackHandler, NULL);
  if(AWS_SUCCESS != rc) {
    msg_error("Error subscribing : %d\n", rc);
  } else {
    msg_info("Subscribed to topic %s\n", cSTopicName);
  }
  sprintf(cPayload, "%s : %d ", "hello from STM", i);
  paramsQOS1.payload = (void *) cPayload;
  printf("Press the User button (Blue) to publish the LED desired value on the %s topic\n", cPTopicName);
  Task_Schedule(service_sensor_logic, 0, 500, 500);
}

void service_sensor_logic(void) {
#ifdef SENSOR
  static uint32_t sensor_publish_time = 0;
#endif
  uint8_t bp_pushed;
  IoT_Error_t rc = FAILURE;

  bp_pushed = Button_WaitForMultiPush(1000); // note this now returns immediately and every 1000ms it will have a value if the button was pressed
  /* exit loop on long push  */
  if (bp_pushed == BP_MULTIPLE_PUSH )
  {
    msg_info("\nPushed button perceived as a *double push*. Terminates the application.\n");
    // remove myself
    Task_Remove(service_sensor_logic, 0);
    publishCount = 0;
    return;
  }
  if (bp_pushed == BP_SINGLE_PUSH)
  {
    if(strstr(ledstate, "Off")!= NULL)
    {
      strcpy(ledstate, "On");
    }
    else
    {
      strcpy(ledstate, "Off");
    }
    printf("Sending the desired LED state to AWS.\n");
    /* create desired message */
    memset(cPayload, 0, sizeof(cPayload));
    strcat(cPayload, aws_json_desired);
    strcat(cPayload, "{\"LED_value\":\"");
    strcat(cPayload, ledstate);
    strcat(cPayload, "\"}");
    strcat(cPayload, aws_json_post);
    paramsQOS1.payloadLen = strlen(cPayload) + 1;
    /// @todo split this into a state machine to avoid the loop
    do
    {
      //rc = aws_iot_mqtt_publish(&client, cPTopicName, strlen(cPTopicName), &paramsQOS1);
	rc = MQTT_Publish(cPTopicName, QOS1, cPayload, paramsQOS1.payloadLen );
      if (rc == AWS_SUCCESS)
      {
	printf("\nPublished to topic %s:", cPTopicName);
	printf("%s\n", cPayload);
      }

      if (publishCount > 0)
      {
	publishCount--;
      }
    } while(MQTT_REQUEST_TIMEOUT_ERROR == rc && (publishCount > 0));
  }
#ifdef  SENSOR
  if (TimeSince(sensor_publish_time) >= TIMER_COUNT_FOR_SENSOR_PUBLISH*1000)
  {
    sensor_publish_time = TimeNow();

    PrepareSensorsData(cPayload, sizeof(cPayload), NULL);

    paramsQOS1.payloadLen = strlen(cPayload) + 1;

    do
    {
      //rc = aws_iot_mqtt_publish(&client, cPTopicName, strlen(cPTopicName), &paramsQOS1);
	rc = MQTT_Publish(cPTopicName, QOS1, cPayload, paramsQOS1.payloadLen);
      if (rc == AWS_SUCCESS)
      {
	printf("\nPublished to topic %s:\n", cPTopicName);
	printf("%s\n", cPayload);
      }

      if (publishCount > 0)
      {
	publishCount--;
      }
    } while((MQTT_REQUEST_TIMEOUT_ERROR == rc) && (publishCount > 0));
  }
#endif
}

/**
* @brief MQTT subscriber callback hander
*
* called when data is received from AWS IoT Thing (message broker)
* @param MQTTCallbackParams type parameter
* @return no return
*/
void MQTTcallbackHandler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData)
{
  const char msg_on[]  = "{\"state\":{\"reported\":{\"LED_value\":\"On\"}}}";
  const char msg_off[] = "{\"state\":{\"reported\":{\"LED_value\":\"Off\"}}}";
  const char *msg = NULL;
  IoT_Publish_Message_Params paramsQOS1 = {QOS1, 0, 0, 0, NULL,0};

  msg_info("\nMQTT subscribe callback......\n");
  msg_info("%.*s\n", (int)params->payloadLen, (char *)params->payload);

  /* If a new desired LED state is received, change the LED state. */
  if (strstr((char *) params->payload, "\"desired\":{\"LED_value\":\"On\"}") != NULL)
  {
    Led_SetState(true);
    strcpy(ledstate, "On");
    msg_info("LED On!\n");
    msg = msg_on;
  }
  else if (strstr((char *) params->payload, "\"desired\":{\"LED_value\":\"Off\"}") != NULL)
  {
    Led_SetState(false);
    strcpy(ledstate, "Off");
    msg_info("LED Off!\n");
    msg = msg_off;
  }

  /* Report the new LED state to the MQTT broker. */
  if (msg != NULL)
  {
    paramsQOS1.payload = (void *) msg;
    paramsQOS1.payloadLen = strlen(msg) + 1;
    IoT_Error_t rc = aws_iot_mqtt_publish(pClient, cPTopicName, strlen(cPTopicName), &paramsQOS1);

    if (rc == AWS_SUCCESS)
    {
      msg_info("\nPublished the new LED status to topic %s:", cPTopicName);
      msg_info("%s\n", msg);
    }
  }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
