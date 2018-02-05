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
#define mqtt_server_ "10.0.0.2"
#define mqtt_port_ 1883
#define mqtt_user_ "123"
#define mqtt_pass_ "345"
#define mqtt_topic_ "imagine/medigray"

#define measure_period 3000
#define broadcast_period 15000
#define adj_persistence 1000
#define adj_period 10000


#endif /* MAIN_MEDIGRAY_H_ */
