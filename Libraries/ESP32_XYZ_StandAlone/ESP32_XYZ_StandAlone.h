#ifndef ESP32_XYZ_H
#define ESP32_XYZ_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <functional>
//#include <string>

class ESP32_XYZ 
{
	public:
		ESP32_XYZ();
		bool connectAP(const char* ssid, const char* pass);
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
		void MQTTSetCallback(std::function<void(char*, uint8_t*, unsigned int)> callback);
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
