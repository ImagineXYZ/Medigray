#ifndef ESP8266_XYZ_H
#define ESP8266_XYZ_H

#include <Arduino.h>
#include <functional>
#include "PubSubClient.h"
#if defined(ESP32)
#include "WiFi.h"
#else
#include <ESP8266WiFi.h>
#endif



//#include <string>

class ESP8266_XYZ 
{
	public:
		ESP8266_XYZ();
		bool connectAP(const char* ssid, const char* pass);
		int getRSSI();
		void softReset();
		int readResponse(String* response);
		int httpPost(const char* server, String path, int port, String *response);
		int httpGet(const char* server, String path, int port, String *response);
		bool getJsonAttribute(String Input, String Attribute, String *value);
		void addToJson(String id, String value);
		void addToJson(String id, int value);
		void addToJson(String id, float value);
		void setTimeout(uint32_t timeout);
		void MQTTTSetServer(String server, int port);
		void MQTTTSetServer(String server, int port, String user, String pass);
		void MQTTConfig(String id, int retries, int delay_ms);
#if defined(ESP32)
		void MQTTSetCallback(MQTT_CALLBACK_SIGNATURE);
#else
		void MQTTSetCallback(std::function<void(char*, uint8_t*, unsigned int)> callback);
#endif
		bool MQTTPublish(const char* topic);
		bool MQTTPublish(const char* topic, boolean retained);
		bool MQTTSubscribe(const char* topic);
		void MQTTLoop();
		bool MQTTConnected();
		bool MQTTReconnect(const char* id, int retries, int delay_ms);
	private:
		bool connectServer(const char* server, int port);
		void addToJsonAux(String id, String value);
		String json = "{";
		uint global_timeout = 5000;
		String mqtt_id="";
		int mqtt_retries=1, mqtt_delay_ms=100;
		WiFiClient clientESP;
		PubSubClient clientMQTT;
		String mqtt_server="";
		String mqtt_user="";
		String mqtt_pass="";
		int mqtt_port=1883;
};

#endif
