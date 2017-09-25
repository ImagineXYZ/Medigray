#include "Arduino.h"

#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>  // Include SPI if you're using SPI
#include <Adafruit_SSD1306.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <SHT1x.h>
#include "ESP8266_XYZ_StandAlone.h"
#define dataPin 17
#define clockPin 15
#define potTempPin A4
#define potHumPin A6

#define device 17
#define xstr(x) str(x)
#define str(x) #x

#define ID_ xstr (device)
#define hostname_(x) "MTS_" str(x)
#define mqtt_id_(x) "Nodo " str(x)

String SA = ID_;
//#define ID_ strx(device)
//#define hostnamedef(x) strx(MTS_##x)

//String ID = "14";
//const char* hostnameMTS = "MTS_14";
//#define mqtt_id "Nodo 14"

String ID = ID_;
const char* hostnameMTS = hostname_(device);
#define mqtt_id mqtt_id_(device)

//const char* wifi_ssid = "Javo2";
//const char* wifi_pass = "123456789";
const char* wifi_ssid = "ImagineXYZ";
const char* wifi_pass = "delunoalnueve";
#define mqtt_server "m12.cloudmqtt.com"
#define mqtt_port 17269
#define user "bjakjzet"
#define pass "Ja0pHkgWxr6l"
#define test_topic "imagine/medigray"

//const char* wifi_ssid = "Medigray";
//const char* wifi_pass = "22707906AA";
//#define mqtt_server "10.0.0.2"
//#define mqtt_port 1883
//#define user "123"
//#define pass "345"
//#define test_topic "imagine/medigray"

//OTA_PORT=3232;

float adj1=0;
float adj2=0;

SHT1x sht1x(dataPin, clockPin);
String tempC,Hum;
float temp_c_1,humidity_1;

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


#define mqtt_retries_reconnect 5
#define mqtt_retries_delay_ms 500



ESP8266_XYZ esp;

//#define retardo_ms 2000
#define DEBUG 1

int cont=0;
bool primero=true;

String getMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  String cMac = "";
  for (int i = 0; i < 6; ++i) {
    if (mac[i] < 0x10) {
      cMac += "0";
    }
    cMac += String(mac[i], HEX);
    if (i < 5)
      cMac += ":"; // put : or - if you want byte delimiters
  }
  cMac.toUpperCase();
  return cMac;
}

String getMacAddressSimple() {
  byte mac[6];
  WiFi.macAddress(mac);
  String cMac = "";
  for (int i = 0; i < 6; ++i) {
    if (mac[i] < 0x10) {
      cMac += "0";
    }
    cMac += String(mac[i], HEX);
  }
  cMac.toUpperCase();
  return cMac;
}

void SetupOTA(){
	/* create a connection at port 3232 */
	  ArduinoOTA.setPort(3232);
	  /* we use mDNS instead of IP of ESP32 directly */
	  String hostname = "MTS_"+getMacAddressSimple();
	  if(DEBUG){
		  Serial.print("Hostname: ");
		  Serial.println(hostname);
	  }
	  //char hostnamechar[50];

	  //ArduinoOTA.setHostname(hostname.toCharArray(hostnamechar, 50));
	  ArduinoOTA.setHostname(hostnameMTS);

	  /* we set password for updating */
	  ArduinoOTA.setPassword("iotsharing");

	  /* this callback function will be invoked when updating start */
	  ArduinoOTA.onStart([]() {
	    Serial.println("Start updating");
	  });
	  /* this callback function will be invoked when updating end */
	  ArduinoOTA.onEnd([]() {
	    Serial.println("\nEnd updating");
	  });
	  /* this callback function will be invoked when a number of chunks of software was flashed
	  so we can use it to calculate the progress of flashing */
	  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
	    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	  });

	  /* this callback function will be invoked when updating error */
	  ArduinoOTA.onError([](ota_error_t error) {
	    Serial.printf("Error[%u]: ", error);
	    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
	    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
	    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
	    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
	    else if (error == OTA_END_ERROR) Serial.println("End Failed");
	  });
	  /* start updating */
	  ArduinoOTA.begin();

}

void setup()
{
	pinMode(5,OUTPUT);

	if(DEBUG){
		Serial.begin(115200);

	}

	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 5);
	display.println("iMA6iNEXYZ");
	display.setTextSize(1);
	display.println("");
	display.println("www.imaginexyz.com");
	display.println("");
	display.println("");
	display.display();
	delay(500);

	while(!esp.connectAP(wifi_ssid, wifi_pass)){
		delay(200);
		if(DEBUG){
			Serial.println("Conectando Wifi...");
		}
	}

	if(DEBUG){
		Serial.println("OTA Setup");
	}

	SetupOTA();

	if(DEBUG){
			Serial.println("MQTT Config");
	}

	esp.MQTTTSetServer(mqtt_server, mqtt_port, user, pass);
	esp.MQTTConfig(mqtt_id, mqtt_retries_reconnect, mqtt_retries_delay_ms);

	//esp.MQTTSetCallback(callbackTest);

	//esp.MQTTSubscribe(test_topic);

	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 5);
	display.println("iMA6iNEXYZ");
	display.setTextSize(1);
	display.println("");
	display.println("www.imaginexyz.com");
	display.println("");
	display.println("");
	display.println(getMacAddress());
	display.display();
	delay(3000);


}

void enviarMensaje(){

	digitalWrite(5,HIGH);
	while(!esp.connectAP(wifi_ssid, wifi_pass)){
		delay(200);
		if(DEBUG){
			Serial.println("Conectando Wifi...");
		}
	}
	if(DEBUG){
		Serial.println("Enviando Mensaje");
	}
	tempC = String(temp_c_1/60, 2);
	Hum = String(humidity_1/60, 2);
	esp.addToJson("sensor", ID);
	esp.addToJson("hum", Hum);
	esp.addToJson("temp", tempC);
	esp.MQTTPublish(test_topic);
	digitalWrite(5,LOW);
}

void leersensor(){
	//adj1=(analogRead(potTempPin)/(1024))*8-4;
	//adj2=(analogRead(potHumPin)/(1024))*8-4;
	adj1=0;
	adj2=0;
	float temp_c = sht1x.readTemperatureC()+adj1;
	float humidity = sht1x.readHumidity()+adj2;
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 5);
	display.println("iMA6iNEXYZ");
	display.setTextSize(1);
	display.println("");
	display.println("www.imaginexyz.com");
	display.print("T: ");
	display.print(temp_c);
	display.println(" C");
	display.print("H: ");
	display.print(humidity);
	display.println(" %");
	display.println(getMacAddress());
	display.display();

	if(primero){
		temp_c_1=temp_c;
		humidity_1=humidity;
		primero=false;
	}else{
		temp_c_1+=temp_c;
		humidity_1+=humidity;
	}


	if(DEBUG){
		Serial.print("ADJ1(T): ");
		Serial.print(adj1);
		Serial.print("\tADJ2(H): ");
		Serial.println(adj2);
		Serial.print("T: ");
		Serial.print(temp_c);
		Serial.print("  \tH: ");
		Serial.println(humidity);
	}

	//tempC = String(temp_c, 2);
	//Hum = String(humidity, 2);

}

void loop()
{
	delay(100);
	ArduinoOTA.handle();
	esp.MQTTLoop();
	leersensor();
	if(cont>=59){
		enviarMensaje();
		cont=0;
		primero=true;
	}
	cont++;
	Serial.println(SA);
//	Serial.print (hostnamedef(device));

}
//
//void callbackTest(char* topic, unsigned char* payload, unsigned int length) {
//	if(DEBUG){
//		Serial.print("Message arrived [");
//		Serial.print(topic);
//		Serial.print("] ");
//		for (uint i = 0; i < length; i++) {
//			Serial.print((char)payload[i]);
//		}
//		Serial.println();
//	}
//}

