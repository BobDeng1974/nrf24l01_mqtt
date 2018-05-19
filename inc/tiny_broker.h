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
#define ADDR_SIZE					4

#define CLNT_ID_POS					14
#define CLNT_ID_SIZE_POS			13


#define CLEAN_S_FLAG			(1<<1)
#define WILL_FLAG				(1<<2)
#define WILL_QOS_FLAG			(3<<3)
#define WILL_RETAIN_FLAG		(1<<5)
#define USR_NAME_FLAG			(1<<6)
#define PSWD_FLAG				(1<<7)

typedef struct{
uint8_t data[256];
uint8_t len;
}local_host_t;


typedef struct {
	uint8_t net_address[ADDR_SIZE];
	char*  client_id;
	bool keepalive;
	char*  username;
	char*  password;
	bool last_will;
	char*  will_topic;
	char*  will_payload;
	uint8_t will_qos;
	uint8_t will_retain;
	char* subs_topic[MAX_SUBS_TOPIC];
}  conn_client_t;


typedef struct{
	conn_client_t clients[MAX_CONN_CLIENTS];
	MqttNet * net;
}broker_t;

void * m_malloc(size_t size);

#endif /* INC_TINY_BROKER_H_ */
