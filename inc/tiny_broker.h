#include <sys/_stdint.h>

/*
 * tiny_broker.h
 *
 *  Created on: 10.05.2018
 *      Author: tomek
 */

#ifndef INC_TINY_BROKER_H_
#define INC_TINY_BROKER_H_

#include "mqtt_socket.h"
#include "stdbool.h"
#include "string.h"

#define MAX_SUBS_TOPIC 				8
#define MAX_TOPIC_NAME_SIZE 		32
#define NOT_FOUND					255
#define MAX_CONN_CLIENTS			8






typedef struct {
	uint8_t net_address[4];
	char*  client_id;
	bool keepalive;
	char*  username;
	char*  password;
	bool last_will;
	char*  will_topic;
	char*  will_payload;
	char* subs_topic[MAX_SUBS_TOPIC];
}  conn_client_t;


typedef struct{
	char*  topic_name;
	uint8_t qos_level;
	char*  clients_id[MAX_CONN_CLIENTS];
	uint8_t sub_client_nb;
}topic_subs_t;


typedef struct{
	topic_subs_t topics[MAX_SUBS_TOPIC];
	conn_client_t clients[MAX_CONN_CLIENTS];
	MqttNet * net;
}broker_t;



#endif /* INC_TINY_BROKER_H_ */
