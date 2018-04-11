#include "main.h"
#include "mqtt.h"
#include "aws_iot_mqtt_client.h"
#include "aws_iot_error.h"
#include "task.h"

static AWS_IoT_Client client;
static IoT_Client_Init_Params mqttInitParams;
static IoT_Client_Connect_Params connectParams;
static IoT_Error_t con_stat = FAILURE;

// internal connection manager
static void MQTT_Tick(void);

// internal helper to update status handlers
static void StatusUpdate(ClientState status);

static void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data);

// list for status callbacks
static mqtt_status_callback_t status_callback[MQTT_MAX_STATUS_CALLBACKS];
static int num_status_callbacks = 0;

IoT_Error_t MQTT_Init(void){
    // use flag so module only gets initialized once
    static uint8_t init_flag = 0;
    if(init_flag) return CLIENT_STATE_INITIALIZED;
    init_flag = 1;
    const char *serverAddress = NULL;
    static const char *pDeviceName;
    const char *pCaCert;
    const char *pClientCert;
    const char *pClientPrivateKey;
    static IoT_Error_t rc = FAILURE;
    int i;

    memset(&client, 0, sizeof(AWS_IoT_Client));
    mqttInitParams = iotClientInitParamsDefault;

    getServerAddress(&serverAddress);
    getTLSKeys(&pCaCert, &pClientCert, &pClientPrivateKey);

    mqttInitParams.enableAutoReconnect = false;
    mqttInitParams.pHostURL = (char *) serverAddress;
    mqttInitParams.port = AWS_IOT_MQTT_PORT;
    mqttInitParams.pRootCALocation = (char *) pCaCert;
    mqttInitParams.pDeviceCertLocation = (char *) pClientCert;
    mqttInitParams.pDevicePrivateKeyLocation = (char *) pClientPrivateKey;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = NULL;
    rc = aws_iot_mqtt_init(&client, &mqttInitParams);

    if(rc != 0){
	msg_error("aws_iot_mqtt_init error");
	return rc;
    }

    getIoTDeviceConfig(&pDeviceName);
    connectParams.keepAliveIntervalInSec = 30;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = (char *) pDeviceName;
    connectParams.clientIDLen = (uint16_t) strlen(pDeviceName);
    connectParams.isWillMsgPresent = false;
    uint8_t connectCounter = 0;
    do{
	rc = aws_iot_mqtt_connect(&client, &connectParams);
	connectCounter++;
    }while(rc != AWS_SUCCESS || (connectCounter > MQTT_CONNECT_MAX_ATTEMPT_COUNT));

    if(AWS_SUCCESS != rc){
	msg_error("MQTT connection failed");
	return rc;
    }

    rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
    if(AWS_SUCCESS != rc) {
	msg_error("Unable to set Auto Reconnect to true - %d\n", rc);
	if (aws_iot_mqtt_is_client_connected(&client)) {
	    aws_iot_mqtt_disconnect(&client);
	}
	return rc;
    }

    if(rc == AWS_SUCCESS) {
	// call list of status callbacks to let them know we are connected
	StatusUpdate(CLIENT_STATE_CONNECTED_IDLE);
    }

    // schedule a task to service the MQTT client every MQTT_YIELD_PERIOD
    Task_Schedule(MQTT_Tick, 0, MQTT_YIELD_PERIOD, MQTT_YIELD_PERIOD);
    con_stat = AWS_SUCCESS;
    return rc;
}


IoT_Error_t MQTT_Subscribe(char* topic, QoS qos, pApplicationHandler_t callback, char* pData){
    return aws_iot_mqtt_subscribe(&client, topic, strlen(topic), qos, callback, pData);
}


IoT_Error_t MQTT_Publish(char* topic, QoS qos, char* pData, size_t dLen){

    IoT_Error_t rc = FAILURE;
    IoT_Publish_Message_Params publishParams = {qos, 0, 0, 0, pData, dLen};
        
    rc = aws_iot_mqtt_publish(&client, topic, strlen(topic), &publishParams);
	
    return rc;
}	


IoT_Error_t MQTT_Unsubscribe(char* topic){
    return aws_iot_mqtt_unsubscribe(&client, topic, strlen(topic));
}


static void MQTT_Tick(void) {
    static int state = 0;
    static uint32_t timestamp = 0;
    if(state == 0) {
	if(NETWORK_ATTEMPTING_RECONNECT == con_stat || NETWORK_RECONNECTED ==  con_stat|| AWS_SUCCESS == con_stat){
	    /* Max time the yield function will wait for read messages */
	    con_stat = aws_iot_mqtt_yield(&client, MQTT_YIELD_TIME);
	    if(NETWORK_ATTEMPTING_RECONNECT == con_stat)
	    {
	      state = 1; // reconnect state where we just wait one second
	      timestamp = TimeNow();
	      StatusUpdate(CLIENT_STATE_PENDING_RECONNECT);
	      return;
	    }
	    if(NETWORK_RECONNECTED == con_stat)
	    {
	        StatusUpdate(CLIENT_STATE_CONNECTED_IDLE);
	    }
	}else {
	    msg_error("Something went wrong - %d\n", con_stat);
	    StatusUpdate(CLIENT_STATE_DISCONNECTING);
	    aws_iot_mqtt_yield(&client, 10);
	    aws_iot_mqtt_disconnect(&client);
	}
    }else {
	if(TimeSince(timestamp) > 1000) {
	    msg_info("Attempting to reconnect\n");
	    state = 0; // go back to the main state
	}
    }
}


static void StatusUpdate(ClientState status) {
    int i;
    for(i = 0; i < num_status_callbacks; i++) {
	status_callback[i](status);
    }
}

static void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data)
{
  // Might need to change stuff here or use this to call StatusUpdate()
  msg_warning("MQTT Disconnect\n");
  IoT_Error_t rc = FAILURE;

  if(NULL == data)
  {
    return;
  }

  AWS_IoT_Client *client = (AWS_IoT_Client *)data;

  if(aws_iot_is_autoreconnect_enabled(client))
  {
    msg_info("Auto Reconnect is enabled, Reconnecting attempt will start now\n");
  }
  else
  {
    msg_warning("Auto Reconnect not enabled. Starting manual reconnect...\n");
    rc = aws_iot_mqtt_attempt_reconnect(client);

    if(NETWORK_RECONNECTED == rc)
    {
      msg_warning("Manual Reconnect Successful\n");
    }
    else
    {
      msg_warning("Manual Reconnect Failed - %d\n", rc);
    }
  }
}

