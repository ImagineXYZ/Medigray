#define device	18
//DEBUG 1 -> Debug mode active
#define DEBUG 1

//WiFi Network Credentials
const char* wifi_ssid = "ImagineXYZ";
const char* wifi_pass = "delunoalnueve";
//const char* wifi_ssid = "Medigray";
//const char* wifi_pass = "22707906AA";
//const char* wifi_ssid = "Salazar Ramirez";
//const char* wifi_pass = "17bsantiagodavid130212";

//Network Params
int OTA_PORT=3232;
const char* OTA_pass = "iotsharing";

//MQTT Server Information and params
#define mqtt_server_ "31.22.7.217"
#define mqtt_port_ 1883
#define mqtt_user_ "prueba"
#define mqtt_pass_ "prueba"
#define mqtt_topic_ "imagine/test"

//#define mqtt_server "10.0.0.2"
//#define mqtt_port 1883
//#define user "123"
//#define pass "345"
//#define test_topic "imagine/medigray"

#define mqtt_retries_reconnect_ 5
#define mqtt_retries_delay_ms_ 500

//****----------------- INCLUDES ---------------*****//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "Arduino.h"

//Network Includes
#include "ArduinoOTA.h"
#include "WiFiUdp.h"
#include "ESPmDNS.h"
#include "ESP8266_XYZ_StandAlone.h"

//Interface Includes
#include "Wire.h"
#include "SPI.h"

//Sensor Includes
#include "SHT1x.h"

//Display Includes
#include "Adafruit_SSD1306.h"

//****----------------- PREPROCESSOR ---------------*****//
//Preprocessor functions and definitions
#define xstr(x) str(x)
#define str(x) #x
#define ID_ xstr (device)
#define hostname_(x) "MTS_" str(x)
#define mqtt_id_(x) "Nodo " str(x)
//JSON ID for MQTT Publish, <device>
String ID = ID_;
//Hostname MTS_<device> constant definition
const char* hostnameMTS = hostname_(device);
#define mqtt_id mqtt_id_(device)


//****-------------- GLOBAL DEFINITIONS -------------*****//
//Network communication
ESP8266_XYZ esp;

//Sensor
#define dataPin 17
#define clockPin 15
SHT1x sht1x(dataPin, clockPin);
String tempC,Hum;
float temp_c_1,humidity_1;
float temp_c ;
float humidity;

//Display and LEDS
#define LED_PIN 5
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


//Potentiometers
#define potTempPin A4
#define potHumPin A6
float adj1=0;
float adj2=0;

#define LED_GPIO 5

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

//****-------------- OTA FUNCTIONS -------------*****//
void SetupOTA(){
	/* create a connection at port 3232 */
	  ArduinoOTA.setPort(OTA_PORT);

	  /* we use mDNS instead of IP of ESP32 directly */
	  if(DEBUG){
		  Serial.print("Hostname: ");
		  Serial.println(hostnameMTS);
	  }

	  ArduinoOTA.setHostname(hostnameMTS);

	  /* we set password for updating */
	  ArduinoOTA.setPassword(OTA_pass);

	  /* this callback function will be invoked when updating start */
	  ArduinoOTA.onStart([]() {
		  if(DEBUG){
			  Serial.println("Start updating");
		  }
	  });
	  /* this callback function will be invoked when updating end */
	  ArduinoOTA.onEnd([]() {
		  if(DEBUG){
			  Serial.println("\nEnd updating");
		  }
	  });
	  /* this callback function will be invoked when a number of chunks of software was flashed
	  so we can use it to calculate the progress of flashing */
	  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		  if(DEBUG){
			  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
		  }
	  });

	  /* Callback Function for updating error */
	  ArduinoOTA.onError([](ota_error_t error) {
		  if(DEBUG){Serial.printf("Error[%u]: ", error);}
	    if (error == OTA_AUTH_ERROR){
	    	if(DEBUG){
	    		Serial.println("Auth Failed");
	    	}
	    }
	    else if (error == OTA_BEGIN_ERROR){
	    	if(DEBUG){
	    		Serial.println("Begin Failed");
	    	}
	    }
	    else if (error == OTA_CONNECT_ERROR){
	    	if(DEBUG){
	    		Serial.println("Connect Failed");
	    	}
	    }
	    else if (error == OTA_RECEIVE_ERROR){
	    	if(DEBUG){
	    		Serial.println("Receive Failed");
	    	}
	    }
	    else if (error == OTA_END_ERROR) {
	    	if(DEBUG){
	    		Serial.println("End Failed");
	    	}
	    }
	  });
	  /* start updating */
	  ArduinoOTA.begin();
}


void leersensor(){
	//adj1=(analogRead(potTempPin)/(1024))*8-4;
	//adj2=(analogRead(potHumPin)/(1024))*8-4;
	adj1=0;
	adj2=0;
	temp_c = sht1x.readTemperatureC()+adj1;
	humidity = sht1x.readHumidity()+adj2;
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
	display.print(getMacAddress());
	display.print(" ");
	display.println(ID_);
	display.display();

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

	tempC = String(temp_c, 2);
	Hum = String(humidity, 2);

}


void enviarMensaje(){

	digitalWrite(5,HIGH);
	bool connected_wifi = WiFi.status() == WL_CONNECTED;
	if (!connected_wifi){
		int cont=0;
		while(!esp.connectAP(wifi_ssid, wifi_pass)){
			delay(200);
			if(DEBUG){
				Serial.println("Conectando Wifi...");
			}
			cont++;
			if (cont>5){
				break;
			}
		}
	}

	connected_wifi = WiFi.status() == WL_CONNECTED;
	if (connected_wifi){
		if(DEBUG){
				Serial.println("Enviando Mensaje");
		}
		//tempC = String(temp_c_1/60, 2);
		//Hum = String(humidity_1/60, 2);
		esp.addToJson("sensor", ID);
		esp.addToJson("hum", Hum);
		esp.addToJson("temp", tempC);
		//esp.addToJson("hum", String(humidity,2));
		//esp.addToJson("temp", String(temp_c,2));
		esp.addToJson("exp","IDF WDT");
		esp.MQTTPublish(mqtt_topic_);

	}else{
		if(DEBUG){
				Serial.println("No Conectado");
		}
	}

	digitalWrite(5,LOW);
}

void MTS_task(void *pvParameter)
{
	//Parece que hay un conflicto con esta función
	//Revisar
	//esp_task_wdt_init();

	//El puerto se configuró en 115200 en el sdkconfig
	Serial.begin(115200);

	/* Set the GPIO as a push/pull output */
	pinMode(LED_GPIO, OUTPUT);
	//gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	if(DEBUG){
		Serial.println(" Wifi");
	}
	//Connect to WiFi
	esp.connectAP(wifi_ssid, wifi_pass);
	bool connected_wifi = WiFi.status() == WL_CONNECTED;
	if(connected_wifi){
		//delay(1000); //
		if(DEBUG){
			Serial.println("--Conectado a WiFi--");
		}
	}else{
		if(DEBUG){
			Serial.println("--NO conectado a WiFi--");
		}
	}

	if(DEBUG){
		Serial.println("OTA Setup");
	}

	SetupOTA();

	if(DEBUG){
		Serial.println("MQTT Config");
	}
	esp.MQTTTSetServer(mqtt_server_, mqtt_port_, mqtt_user_, mqtt_pass_);
	esp.MQTTConfig(mqtt_id, mqtt_retries_reconnect_, mqtt_retries_delay_ms_);
	esp_task_wdt_feed();
	int i=0,div=20;
	while(1){
		i=0;
		while(i<div){
			delay(8000/div);
			ArduinoOTA.handle();
			esp.MQTTLoop();
			esp_task_wdt_feed();
			i++;
		}
		ArduinoOTA.handle();
		esp.MQTTLoop();
		leersensor();
		enviarMensaje();
		//delay(8000);
		Serial.println("Feed");
		esp_task_wdt_feed();
	}
//	//LOOP TEST WDT
//	while(1) {
//		/* Blink off (output low) */
//		//gpio_set_level(BLINK_GPIO, 0);
//		digitalWrite(LED_GPIO,LOW);
//		vTaskDelay(500 / portTICK_PERIOD_MS);
//		/* Blink on (output high) */
//		//gpio_set_level(BLINK_GPIO, 1);
//		digitalWrite(LED_GPIO,HIGH);
//		vTaskDelay(500 / portTICK_PERIOD_MS);
//		leersensor();
//		enviarMensaje();
//		printf("%d\n",i);
//		if(i<10){
//			esp_task_wdt_feed();
//		}else{
//			ESP_LOGI("Funct","No Feed");
//		}
//
//		i++;
//
//	}
}

extern "C" void app_main(void)
{
	initArduino();
	xTaskCreate(&MTS_task, "MTS_task", 4096, NULL, 5, NULL);

}

