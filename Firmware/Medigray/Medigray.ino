#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SHT1x.h>
#include "Arduino.h"
#include "ESP8266_XYZ_StandAlone.h"

#define mqtt_server "m10.cloudmqtt.com"
#define mqtt_port 19586

#define mqtt_id "Nodo Prueba"
#define user "azvhtoyy"
#define pass "lFd4B29vyXqj"
#define test_topic "imagine/medigray"

#define mqtt_retries_reconnect 5
#define mqtt_retries_delay_ms 500

#define retardo_ms 0
#define DebugUSB 1

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Specify data and clock connections and instantiate SHT1x object
#define dataPin  17
#define clockPin 15

ESP8266_XYZ esp;

SHT1x sht1x(dataPin, clockPin);

const char* wifi_ssid = "ImagineXYZ";
const char* wifi_pass = "delunoalnueve";

String tempC, tempF, Hum;
String ID = "1";

void setup()   {
  if (DebugUSB) {
    Serial.begin(115200);
    Serial.println("Iniciando...");
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

   while (!esp.connectAP(wifi_ssid, wifi_pass)) {
    delay(200);
    if (DebugUSB) {
      Serial.println("Conectando Wifi...");
    }
  }

  if (DebugUSB) {
    Serial.println("MQTT Config");
  }
  esp.MQTTTSetServer(mqtt_server, mqtt_port, user, pass);
  //esp.MQTTTSetServer(mqtt_server, mqtt_port);
  esp.MQTTConfig(mqtt_id, mqtt_retries_reconnect, mqtt_retries_delay_ms);

  esp.MQTTSetCallback(callbackTest);

  esp.MQTTSubscribe(test_topic);

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
  delay(5000);
}

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

void enviarMensaje() {
  if (DebugUSB) {
    Serial.println("Enviando Mensaje");
  }
  esp.addToJson("sensor", ID);
  esp.addToJson("hum", Hum);
  esp.addToJson("temp", tempC);
  //esp.addToJson("tempF", tempF);
  esp.addToJson("IP", String(WiFi.localIP()));//No se entiende cuando se envía la dirección
  esp.MQTTPublish(test_topic);
}

void printOled() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Temperatura: ");
  display.print(tempC);
  display.println(" C");
  display.println("");
  display.print("Temperatura: ");
  display.print(tempF);
  display.println(" F");
  display.println("");
  display.print("Humedad: ");
  display.print(Hum);
  display.println(" %");
  display.println("");
  display.println("");
  display.println(ID + "      by iMA6iNEXYZ");
  display.display();
}


void loop() {

  float temp_c, temp_f, humidity;

  temp_c = sht1x.readTemperatureC();
  temp_f = sht1x.readTemperatureF();
  humidity = sht1x.readHumidity();

  if (DebugUSB) {
    Serial.print("Temperature: ");
    Serial.print(temp_c, 2);
    Serial.print("C / ");
    Serial.print(temp_f, 2);
    Serial.print("F. Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.println(WiFi.status());
  }

  tempC = String(temp_c, 2);
  tempF = String(temp_f, 2);
  Hum = String(humidity, 2);

  printOled();  
  esp.MQTTLoop();
  enviarMensaje();
  delay(retardo_ms);
}

void callbackTest(char* topic, unsigned char* payload, unsigned int length) {
  if (DebugUSB) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (uint i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
  }
}

