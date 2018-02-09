//Set Params

#include "params/params.h"


#define TDelta 8
#define HDelta 20

//WiFi Network Credentials

//Network Params
int OTA_PORT=3232;
const char* OTA_pass = "iotsharing";
///Firmware/MTS_v0.5/components/arduino/tools$
//sudo python espota.py -i MTS_1.local -I 192.168.43.106 -p 3232 -P 3232 -a iotsharing -f ../../../build/app-template.bin

#define mqtt_retries_reconnect_ 1
#define mqtt_retries_delay_ms_ 200

int limit_Wifi=100;
int cont_Wifi=0;

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
#include  <math.h>

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
float adjT=0;
float adjH=0;
float adjT_1=0;
float adjH_1=0;
//Data Buffers
#define BUFF_SIZE 1024
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
bool calibration_active = CALIBRATION_ACTIVE_;
bool adjusting_T=false;
bool adjusting_H=false;

long general_cont=0;

#define LED_GPIO 5
//****-------------- PROTOTYPE FUNCTIONS -------------*****//

void Screen_Normal_Operation(float temp_c, float humidity, float ADCT_Val,float ADCH_Val );


//****-------------- IOT FUNCTIONS -------------*****//

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

//ADC Normalizado
float getADC_T_Val(){
	adc1_config_width(ADC_WIDTH_10Bit);
	adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_6db);
	return (float) adc1_get_voltage(ADC1_CHANNEL_4)/1024;
}

//ADC Normalizado
float getADC_H_Val(){
	adc1_config_width(ADC_WIDTH_10Bit);
	adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_6db);
	return (float) adc1_get_voltage(ADC1_CHANNEL_6)/1024;
}

void leersensor(){

	float ADCT_Val,ADCH_Val;
	ADCT_Val=getADC_T_Val();
	ADCH_Val=getADC_H_Val();

	adjT=(ADCT_Val)*TDelta-TDelta/2;
	adjH=(ADCH_Val)*HDelta-HDelta/2;

	temp_c = sht1x.readTemperatureC();
	humidity = sht1x.readHumidity();
	if(calibration_active){
		temp_c+=adjT;
		humidity+=adjH;
	}
	if (humidity>100){
		humidity=100;
	}else{
		if(humidity<0){
			humidity=0;
		}
	}
	Screen_Normal_Operation(temp_c, humidity, ADCT_Val,ADCH_Val );


	if(DEBUG){
		ESP_LOGI("Measure","Temp: %.1f %cC", temp_c, 176);
		ESP_LOGI("Measure","Hum: %.1f %%", humidity);
		ESP_LOGI("Measure","ADJ1[T]: %.1f %cC", adjT, 176);
		ESP_LOGI("Measure","ADJ2[T]: %.1f %%", adjH);
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
	//Standard Deviation
	float temp_sd=0.0;
	for (int i=0;i<size_Temp;i++){
		temp_sd += pow(readn(&TempRing,i) - temp_mean, 2);
	}
	temp_sd=sqrt(temp_sd/size_Temp);
	float hum_sd=0.0;
	for (int i=0;i<size_Hum;i++){
		hum_sd += pow(readn(&HumRing,i) - hum_mean, 2);
	}
	hum_sd=sqrt(hum_sd/size_Hum);
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
		esp_wifi_disconnect();
		delay(200);
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
		esp.addToJson("temp", String(temp_c,2));
		esp.addToJson("temp_mean", String(temp_mean,2));
		esp.addToJson("temp_min", String(temp_min,2));
		esp.addToJson("temp_max", String(temp_max,2));
		esp.addToJson("temp_sd", String(temp_sd,2));
		esp.addToJson("hum", String(humidity,1));
		esp.addToJson("hum_mean", String(hum_mean,1));
		esp.addToJson("hum_min", String(hum_min,1));
		esp.addToJson("hum_max", String(hum_max,1));
		esp.addToJson("hum_sd", String(hum_sd,2));
		esp.addToJson("nT",String(size_Temp));
		esp.addToJson("adjT", String(adjT,2));
		esp.addToJson("adjH", String(adjH,2));
		esp.addToJson("heap",String(esp_get_free_heap_size()));
		esp.addToJson("rssi", String(esp.getRSSI()));
		esp.addToJson("version","0.5.2");
		esp.addToJson("label","WiFi disconnect period");
		esp.addToJson("cont",String(general_cont));
		esp_task_wdt_feed();
		esp.MQTTPublish(mqtt_topic_);
		general_cont++;
		//Disconnect from WiFi preventing false positives
		cont_Wifi++;
		if(cont_Wifi>limit_Wifi){
			cont_Wifi=0;
			delay(200);
			esp_wifi_disconnect();
			delay(200);
		}

	}else{
		if(DEBUG){
				Serial.println("No Conectado");
		}
	}

	digitalWrite(5,LOW);
}

void Screen_Initial(){
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 5);
	display.println("iMA6iNEXYZ");
	display.setTextSize(1);
	display.println("");
	display.println("www.imaginexyz.com");
	display.println("");
	display.print(getMacAddress());
	display.print(" ");
	display.println(ID_);
	display.display();
}

void Screen_Normal_Operation(float temp_c, float humidity, float ADCT_Val,float ADCH_Val ){
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 5);
	display.println("iMA6iNEXYZ");
	display.setTextSize(1);
	display.println("www.imaginexyz.com");
	display.println("");
	display.print("T:");
	display.print(temp_c);
	display.print(" C");
	display.print("  HR:");
	display.print(String(humidity,1));
	display.println("%");
	int nchars=17;
	char fillCharUp=30;
	char fillCharDown=31;
	char centerChar=111;
	int position;
	int center=(int)(nchars/2+1);
	if(adjusting_T){
		display.print("T ");
		position = (int)((ADCT_Val)*nchars);
		display.print("[");
		for(int i=0;i<nchars;i++){
			if (i<center-1){
				if(i<position){
					display.print(' ');
				}else{
					display.print(fillCharDown);
				}
			}
			if(i==center-1){
				display.print(centerChar);
			}
			if (i>center-1){
				if(i>position){
					display.print(' ');
				}else{
					display.print(fillCharUp);
				}
			}
		}
		display.print("]");
	}else{ //Uno de los ajustes a la vez
		if(adjusting_H){
			display.print("H ");
			position = (int)((ADCH_Val)*nchars);
			display.print("[");
			for(int i=0;i<nchars;i++){
				if (i<center-1){
					if(i<position){
						display.print(' ');
					}else{
						display.print(fillCharDown);
					}
				}
				if(i==center-1){
					display.print(centerChar);
				}
				if (i>center-1){
					if(i>position){
						display.print(' ');
					}else{
						display.print(fillCharUp);
					}
				}
			}
			display.print("]");
		}
	}
	display.println("");
	display.print(" ");
	display.print(ID_);
	display.print("     ");
	display.print("RSSI: ");
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
			if (cont>2){
				break;
			}
		}
	}
	if (connected_wifi){
		display.println(String(esp.getRSSI()));
	}else{
		if(cont_Wifi!=0){
			display.println("No WiFi");
		}else{
			cont_Wifi++;
		}
	}
	display.display();
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
	if(DEBUG){
		Serial.println(" Wifi");
	}
	//Connect to WiFi
	esp.connectAP(wifi_ssid, wifi_pass);
	//Initialize Screen
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	Screen_Initial();
	delay(500);


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
	uint32_t timerAdjPers = millis();
	uint32_t timerAdjPeriod = millis();

	cont_Temp=0;
	cont_Hum=0;
	adjT_1=getADC_T_Val();
	adjH_1=getADC_H_Val();

	float temp_val;

	//Adjustment vars
	float ADC_T_Val;
	float ADC_H_Val;
	float newAdjT;
	float newAdjH;

	while(1){

		ArduinoOTA.handle();
		esp.MQTTLoop();
		if(!(adjusting_T || adjusting_H)){
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
		}


		if(((millis() - timerAdjPers) >= adj_persistence) && calibration_active){
			temp_val=getADC_T_Val();
			if(abs(adjT_1-temp_val)>0.01){
				adjusting_T=true;
				adjusting_H=false;
				timerAdjPeriod=millis();
			}else{
				temp_val=getADC_H_Val();
				if(abs(adjH_1-temp_val)>0.01){
					adjusting_T=false;
					adjusting_H=true;
					timerAdjPeriod=millis();
				}
			}
			adjT_1=getADC_T_Val();
			adjH_1=getADC_H_Val();
			timerAdjPers=millis();
		}

		if(adjusting_T || adjusting_H){
			if((millis()-timerAdjPeriod)>=adj_period){
				adjusting_T=false;
				adjusting_H=false;
			}
			//Update Screen
			ADC_T_Val=getADC_T_Val();
			ADC_H_Val=getADC_H_Val();
			newAdjT = (ADC_T_Val)*TDelta-TDelta/2;
			newAdjH = (ADC_H_Val)*HDelta-HDelta/2;
			Screen_Normal_Operation(readn(&TempRing,0)-adjT+newAdjT, readn(&HumRing,0)-adjH+newAdjH, getADC_T_Val(),getADC_H_Val() );
			//Screen Persistence Delay
			delay(50);
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

