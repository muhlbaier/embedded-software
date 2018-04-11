#include "main.h"
#include "mqtt.h"
#include "aws_iot_mqtt_client.h"
#include "aws_iot_error.h"

static AWS_IoT_Client client;
static IoT_Client_Init_Params mqttInitParams;
static IoT_Client_Connect_Params connectParams;


static void Disconnect_Handler(AWS_IoT_Client* theClient, void (*callback)(ClientState clientState));


IoT_Error_t MQTT_Init(void){
	const char *serverAddress = NULL;
	static const char *pDeviceName;
	const char *pCaCert;
	const char *pClientCert;
	const char *pClientPrivateKey;
	static IoT_Error_t rc = FAILURE;

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
	mqttInitParams.disconnectHandler = NULL;
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
		return rc;
	}

	return rc;
}

IoT_Error_t MQTT_Subscribe(char* topic, uint16_t topicLen, QoS qos, void (*callback)(char* data, char* message), char* pData){


}






IoT_Error_t MQTT_Publish(char* topic, uint16_t topicLen, QoS qos, char* pData, size_t dLen){

    IoT_Error_t rc = FAILURE;
    IoT_Publish_Message_Params publishParams = {qos, 0, 0, 0, pData, dLen};
    memset(&client, 0, sizeof(AWS_IoT_Client));
        
    rc = aws_iot_mqtt_publish(&client, topic, topicLen, &publishParams);
	
	return rc;
}	


IoT_Error_t MQTT_Unsubscribe(char* topic, uint16_t topicLen){
    aws_iot_mqtt_unsubscribe(&client, topic, topicLen);
}


//We register Disconnect_Handler with the internal AWS function
//So that we get the current state of the client when disconnect occurs
IoT_Error_t MQTT_StatusHandler(void (*callback)(ClientState clientState)){
    return aws_iot_mqtt_set_disconnect_handler(&client, Disconnect_Handler, callback);
}


void Disconnect_Handler(AWS_IoT_Client* theClient, void (*callback)(ClientState clientState)){
    ClientState currState = aws_iot_mqtt_get_client_state(theClient);
    callback(currState);
}
