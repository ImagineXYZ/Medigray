#define device	13
//DEBUG 1 -> Debug mode active
#define DEBUG 1
#define measure_period 1000
#define broadcast_period 10000
#define TDelta 8
#define HDelta 20

//WiFi Network Credentials
const char* wifi_ssid = "ImagineXYZ";
//const char* wifi_ssid = "Linksys26313";
const char* wifi_pass = "delunoalnueve";
//const char* wifi_ssid = "Medigray";
//const char* wifi_pass = "22707906AA";

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

#define mqtt_retries_reconnect_ 1
#define mqtt_retries_delay_ms_ 200

//****----------------- INCLUDES ---------------*****//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/heap_regions.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_heap_caps.h"
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
#include "driver/adc.h"

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
//A4-> 32
//A6-> 34
#define potTempPin 32
#define potHumPin 34
float adj1=0;
float adj2=0;

//Data Buffers
#define BUFF_SIZE 64
#define BUFF_SIZE_MASK (BUFF_SIZE-1)

typedef struct buffer{
    float buff[BUFF_SIZE];
    int writeIndex;
}buffer;

void write(buffer* buffer,float value){
    buffer->buff[(buffer->writeIndex++) & BUFF_SIZE_MASK] = value;
}

float readn(buffer* buffer, int Xn){
    return buffer->buff[(buffer->writeIndex + (~Xn)) & BUFF_SIZE_MASK];
}

buffer TempRing;
buffer HumRing;
int cont_Temp;
int cont_Hum;



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
			  esp_task_wdt_feed();
			  esp_task_wdt_delete();
		  }
	  });
	  /* this callback function will be invoked when updating end */
	  ArduinoOTA.onEnd([]() {
		  if(DEBUG){
			  Serial.println("\nEnd updating");
			  //esp_task_wdt_feed();
		  }
	  });
	  /* this callback function will be invoked when a number of chunks of software was flashed
	  so we can use it to calculate the progress of flashing */
	  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		  if(DEBUG){
			  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
			  //esp_task_wdt_feed();
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
	adc1_config_width(ADC_WIDTH_10Bit);
	adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_6db);
	adj1=(float) adc1_get_voltage(ADC1_CHANNEL_4);
	adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_6db);
	adj2=(float) adc1_get_voltage(ADC1_CHANNEL_6);

	adj1=(adj1/(1024))*TDelta-TDelta/2;
	adj2=(adj2/(1024))*HDelta-HDelta/2;

	temp_c = sht1x.readTemperatureC()+adj1;
	humidity = sht1x.readHumidity()+adj2;
	if (humidity>100){
		humidity=100;
	}else{
		if(humidity<0){
			humidity=0;
		}
	}

	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 5);
	display.println("iMA6iNEXYZ");
	display.setTextSize(1);
	display.println("www.imaginexyz.com");
	display.println("");
	display.println("");
	display.print("T:");
	display.print(temp_c);
	display.print(" C");
	display.print(" H:");
	display.print(humidity);
	display.println(" %");
	display.print("rssi: ");
	bool connected_wifi = WiFi.status() == WL_CONNECTED;
	if (connected_wifi){
		display.println(String(esp.getRSSI()));
	}else{
		display.println("No conectado");
	}
	display.print(getMacAddress());
	display.print(" ");
	display.println(ID_);
//	display.print("AjT:");
//	display.print(adj1);
//	display.print(" ");
//	display.print("AjH:");
//	display.println(adj2);
	display.display();

	if(DEBUG){
		ESP_LOGI("Measure","Temp: %.1f %cC", temp_c, 176);
		ESP_LOGI("Measure","Hum: %.1f %%", humidity);
		ESP_LOGI("Measure","ADJ1[T]: %.1f %cC", adj1, 176);
		ESP_LOGI("Measure","ADJ2[T]: %.1f %%", adj2);
		ESP_LOGI("Measure","Free IRAM: %d",  esp_get_free_heap_size());
	}

	write(&TempRing,temp_c);
	cont_Temp++;
	write(&HumRing,humidity);
	cont_Hum++;
}


void enviarMensaje(){

	digitalWrite(5,HIGH);

	//Estadistics
	int size_Temp=cont_Temp;
	int size_Hum=cont_Hum;
	if (size_Temp>BUFF_SIZE){
		size_Temp=BUFF_SIZE;
	}
	if (size_Hum>BUFF_SIZE){
		size_Hum=BUFF_SIZE;
	}
	//Promedio, min, max
	float temp_mean,temp_min,temp_max;
	float temp_val;
	temp_mean=0;
	temp_min=readn(&TempRing,0);
	temp_max=readn(&TempRing,0);
	for (int i=0;i<size_Temp;i++){
		temp_val=readn(&TempRing,i);
		temp_mean+=temp_val;
		if(temp_min>temp_val){
			temp_min=temp_val;
		}
		if(temp_max<temp_val){
			temp_max=temp_val;
		}
	}
	temp_mean=temp_mean/size_Temp;
	float hum_mean,hum_min, hum_max;
	float hum_val;
	hum_mean=0;
	hum_min=readn(&HumRing,0);
	hum_max=readn(&HumRing,0);
	for (int i=0;i<size_Hum;i++){
		hum_val=readn(&HumRing,i);
		hum_mean+=hum_val;
		if(hum_min>hum_val){
			hum_min=hum_val;
		}
		if(hum_max<hum_val){
			hum_max=hum_val;
		}
	}
	hum_mean=hum_mean/size_Hum;
	//Reset buffer conts
	cont_Temp=0;
	cont_Hum=0;

	if(DEBUG){
		Serial.println("Check Wifi");
	}
	bool connected_wifi = WiFi.status() == WL_CONNECTED;
	if (!connected_wifi){
		int cont=0;
		esp_task_wdt_feed();
		if(DEBUG){
			Serial.println("Try Wifi...");
		}
		while(!esp.connectAP(wifi_ssid, wifi_pass)){
			esp_task_wdt_feed();
			delay(200);
			if(DEBUG){
				Serial.println("Conectando Wifi...");
			}
			cont++;
			if (cont>5){
				break;
			}
		}
	}else{
		if(DEBUG){
			Serial.println("Wifi connected");
		}
	}

	connected_wifi = WiFi.status() == WL_CONNECTED;
	if (connected_wifi){
		if(DEBUG){
				Serial.println("Enviando Mensaje");
		}
		esp_task_wdt_feed();
		esp.addToJson("sensor", ID);
		esp.addToJson("hum", String(humidity,2));
		esp.addToJson("hum_mean", String(hum_mean,2));
		esp.addToJson("hum_min", String(hum_min,2));
		esp.addToJson("hum_max", String(hum_max,2));
		esp.addToJson("temp", String(temp_c,2));
		esp.addToJson("temp_mean", String(temp_mean,2));
		esp.addToJson("temp_min", String(temp_min,2));
		esp.addToJson("temp_max", String(temp_max,2));
		esp.addToJson("nT",String(size_Temp));
		esp.addToJson("adjT", String(adj1,2));
		esp.addToJson("adjH", String(adj2,2));
		esp.addToJson("heap",String(esp_get_free_heap_size()));
		esp.addToJson("rssi", String(esp.getRSSI()));
		esp.addToJson("version","0.5.1");
		esp.addToJson("label","StatisticsCalRSSI");
		esp_task_wdt_feed();
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

	/* Set the GPIO as output */
	pinMode(LED_GPIO, OUTPUT);

	//Initialize Screen
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

	if(DEBUG){
		Serial.println("Initial configuration completed");
	}
	uint32_t timerMeasure = millis();
	uint32_t timerBroadcast = millis();

	cont_Temp=0;
	cont_Hum=0;

	while(1){

		ArduinoOTA.handle();
		esp.MQTTLoop();
		if( (millis() - timerMeasure) >= measure_period){
			if(DEBUG){
				Serial.println("Leer Sensor");
			}
			leersensor();
			timerMeasure=millis();
		}

		if( (millis() - timerBroadcast) >= broadcast_period){
			if(DEBUG){
				Serial.println("Enviar Mensaje");
			}
			esp_task_wdt_feed();
			enviarMensaje();
			timerBroadcast=millis();
		}

		esp_task_wdt_feed();
		delay(10);
	}
}

extern "C" void app_main(void)
{
	initArduino();
	xTaskCreate(&MTS_task, "MTS_task", 4096, NULL, 5, NULL);

}

