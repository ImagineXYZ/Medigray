/*
 * Medigray.h
 *
 *  Created on: Sep 29, 2017
 *      Author: bryanmtdt
 */

#ifndef MAIN_MEDIGRAY_H_
#define MAIN_MEDIGRAY_H_

const char* wifi_ssid = "Medigray";
const char* wifi_pass = "22707906AA";

//MQTT Server Information and params
#define mqtt_server_ "m11.cloudmqtt.com"
#define mqtt_port_ 17984
#define mqtt_user_ "prueba"
#define mqtt_pass_ "prueba"
#define mqtt_topic_ "imagine/medigray"

#define measure_period 3200
#define broadcast_period 20000
#define adj_persistence 1000
#define adj_period 10000


#endif /* MAIN_MEDIGRAY_H_ */
