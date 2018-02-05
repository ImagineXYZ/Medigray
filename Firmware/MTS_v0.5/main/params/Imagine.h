/*
 * Medigray.h
 *
 *  Created on: Sep 29, 2017
 *      Author: bryanmtdt
 */

#ifndef MAIN_IMAGINE_H_
#define MAIN_IMAGINE_H_

const char* wifi_ssid = "ImagineXYZ";
const char* wifi_pass = "delunoalnueve";

//MQTT Server Information and params
#define mqtt_server_ "31.22.7.217"
#define mqtt_port_ 1883
#define mqtt_user_ "prueba"
#define mqtt_pass_ "prueba"
#define mqtt_topic_ "imagine/test"

#define measure_period 3000
#define broadcast_period 15000
#define adj_persistence 1000
#define adj_period 10000

#endif /* MAIN_MEDIGRAY_H_ */
