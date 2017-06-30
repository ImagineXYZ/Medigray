#include "ESP32_XYZ_StandAlone.h"
//#define DEBUG

#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback


ESP32_XYZ::ESP32_XYZ(void){
	clientESP=WiFiClient();
	clientMQTT=PubSubClient(clientESP);
}

bool ESP32_XYZ::connectAP(const char* ssid, const char* pass){
	uint32_t t = millis();

	WiFi.begin(ssid, pass);

  	while (WiFi.status() != WL_CONNECTED && (millis() - t) < global_timeout) {
    	delay(100);
  	}
#ifdef DEBUG
  	Serial.println(WiFi.localIP());
  	Serial.println(WiFi.status());
  	Serial.println(WiFi.SSID());
  	Serial.println(WiFi.gatewayIP());
#endif
  	return WiFi.status(); 
}

void ESP32_XYZ::softReset(){
	ESP.restart();
}

void ESP32_XYZ::MQTTTSetServer(String server, int port){
	this->mqtt_server=server;
	this->mqtt_port=port;
}

void ESP32_XYZ::MQTTTSetServer(String server, int port, String user, String pass){
	this->mqtt_server=server;
	this->mqtt_port=port;
	this->mqtt_user=user;
	this->mqtt_pass=pass;
}

void ESP32_XYZ::MQTTConfig(String id, int retries, int delay_ms){
	this->mqtt_id=id;
	this->mqtt_retries=retries;
	this->mqtt_delay_ms=delay_ms;
}

void ESP32_XYZ::MQTTSetCallback(std::function<void(char*, uint8_t*, unsigned int)> callback) {
    clientMQTT.setCallback(callback);
}

bool ESP32_XYZ::MQTTPublish(const char* topic){
	if(!MQTTConnected()){
		if (!MQTTReconnect(mqtt_id.c_str(), mqtt_retries, mqtt_delay_ms)){
			return false;
		}
	}
	uint16_t json_len = json.length();
	json.setCharAt(json_len-1, '}');
	clientMQTT.publish(topic,json.c_str());
	json="{";
	return true;
}

bool ESP32_XYZ::MQTTPublish(const char* topic, boolean retained){
	if(!MQTTConnected()){
		if (!MQTTReconnect(mqtt_id.c_str(), mqtt_retries, mqtt_delay_ms)){
			return false;
		}
	}
	uint16_t json_len = json.length();
	json.setCharAt(json_len-1, '}');
	clientMQTT.publish(topic,json.c_str(),retained);
	json="{";
	return true;
}

bool ESP32_XYZ::MQTTSubscribe(const char* topic){
	if(!MQTTConnected()){
		if (!MQTTReconnect(mqtt_id.c_str(), mqtt_retries, mqtt_delay_ms)){
			return false;
		}
	}
	return clientMQTT.subscribe(topic);
}

void ESP32_XYZ::MQTTLoop(){
	clientMQTT.loop();
}

bool ESP32_XYZ::MQTTConnected(){
	return clientMQTT.connected();
}

bool ESP32_XYZ::MQTTReconnect(const char* id, int retries, int delay_ms){
	int i=0;
	clientMQTT.setServer(mqtt_server.c_str(), mqtt_port);
	while (!clientMQTT.connected()) {
#ifdef DEBUG
		Serial.print("Attempting MQTT connection...");
#endif
		if (clientMQTT.connect(id,mqtt_user.c_str(),mqtt_pass.c_str())) {
#ifdef DEBUG
			Serial.println("connected");
#endif
		} else {
#ifdef DEBUG
			Serial.print("failed, rc=");
			Serial.print(clientMQTT.state());
			Serial.println(" try again in 5 seconds");
#endif
			delay(delay_ms);
		}
		if(i<retries){
			i++;
		}else{
			return false;
		}
	}
	return true;
}

int ESP32_XYZ::readResponse(String* response) {

	//Banderas de control del parseo
	bool line_blank = true;
	bool http_body = false;
	bool in_status = false;

	//Variables de soporte
	char status_code[4];
	int i = 0;
	int code = 0;
	uint32_t t = millis();

	//Se lee caracter por caracter de acuerdo con las distintas partes
	//de un response HTTP
	while ((millis() - t) < global_timeout) {

	    if (clientESP.available()) {

	    	char c = clientESP.read();;
	    	

		    //Luego del primer espacio se encuentra el código de estado
		    if(c == ' ' && !in_status){
		        in_status = true;
		    }

		    //Se lee el c�digo de estado de 3 d�gitos
		    if(in_status && i < 3 && c != ' '){////Qu� pasa si el c�digo es de 4 digitos???
		        status_code[i] = c;
		        i++;
		    }

		    //Se parsea el c�digo de char* a int
		    if(i == 3){
		        status_code[i] = '\0';
		        code = atoi(status_code);
		    }

		    if(http_body){
		    	//Si se detecta el cuerpo de la respuesta
		    	//se escribe en response (si existe)
		   		if(response != NULL){
		   			response->concat(c);
		   			if(c=='}'){
		   				break;
		   			}
		   		}else{
		   			break;
		   		}

		    } else {	
		    	//Condiciones para detectar el cuerpo de la respuesta
		        if (c == '\n' && line_blank) {
		            http_body = true;
		        }
		        if (c == '\n') {
		            line_blank = true;
		        } else if (c != '\r') {
		            line_blank = false;
		        }
		    }
	    }
	}
	response->remove(0,response->indexOf('{'));
	response->remove(response->indexOf('}')+1);

	#ifdef DEBUG 
	    	Serial.println(*response);
	#endif
  	return code;
}


bool ESP32_XYZ::connectServer(const char* server, int port){
	uint32_t t = millis();
	bool connected = false;
	do {
    	connected = clientESP.connect(server, port);
    	delay(500);
  	} while (!connected && (millis() - t) < global_timeout);
  	return connected;
}


int ESP32_XYZ::httpPost(const char* server, String path, int port, String *response){
	String msg = "";
	String server_str = server;
	if(connectServer(server, port)){
		#ifdef DEBUG 
			Serial.println("Connected to server");
		#endif

	} else {
		#ifdef DEBUG 
			Serial.println("Connection Failure");
		#endif
		return -1;
	}

	server_str += ":";
	server_str += String(port);

	//C�lculo de lengitud total del mensaje
	uint16_t json_len = json.length();

	json.setCharAt(json_len-1, '}');

	//POST  HTTP/1.1\r\nHost: \r\nConnection: close\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length:\r\n\r\n

	//Solicitud HTTP al servidor
	//Header
	msg += "POST ";
	msg += path;
	msg += " HTTP/1.1\r\nHost: ";
	msg += server_str;
	msg += "\r\nConnection: keep-alive\r\n";
	msg += "Accept: application/json\r\n";
	msg += "Content-Type: application/json\r\nContent-Length:";
	msg += json_len;
	msg += "\r\n\r\n";
	msg += json;

	#ifdef DEBUG 
			Serial.println("Request: ");
			Serial.println(msg);
	#endif
	
	clientESP.print(msg);
	//Reestablece el JSON para el siguiente POST
	json = "{";

	return readResponse(response);
	
}

int ESP32_XYZ::httpGet(const char* server, String path, int port, String *response){
	String msg = "";
	String server_str = server;

	if(connectServer(server, port)){
		#ifdef DEBUG 
			Serial.println("Connected to server");
		#endif

	} else {
		#ifdef DEBUG
			Serial.println("Connection Failure");
		#endif
	}

	server_str += ":";
	server_str += String(port);

	//GET  HTTP/1.1\r\nHost: \r\n\r\n
	//Solicitud HTTP al servidor
	//Header
    msg += "GET ";
    msg += path;
    msg += " HTTP/1.1\r\nHost: ";
   	msg += server_str;
	msg += "\r\n\r\n";
	#ifdef DEBUG
		Serial.println(msg);
	#endif
	//Se env�a el mensaje al servidor
	clientESP.print(msg);

	//Se obtiene el c�digo de estado de la solicitud
	return readResponse(response);
}

void ESP32_XYZ::setTimeout(uint32_t timeout){
	global_timeout = timeout;
}

void ESP32_XYZ::addToJson(String id, String value){
	json += "\"";
	json += id;
	json += "\":\"";
	json += value;
	json += "\",";
}

void ESP32_XYZ::addToJsonAux(String id, String value){
	json += "\"";
	json += id;
	json += "\":";
	json += value;
	json += ",";
}

void ESP32_XYZ::addToJson(String id, int value){
	addToJsonAux(id, String(value));
}

void ESP32_XYZ::addToJson(String id, float value){
	addToJsonAux(id, String(value));
}

bool ESP32_XYZ::getJsonAttribute(String Input, String Attribute, String *value) {

	int start = Input.indexOf(":",Input.indexOf(Attribute));
	int end = Input.indexOf(",",start);
	int end2 = Input.indexOf("}",start);
	if (end == -1){end = end2;}
	for (int n = start+1; n<end; n++) {
		value -> concat(Input[n]);
	}
	return value;               
}
