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

#define PLD_START					12
#define CLNT_ID_POS					14
#define CLNT_ID_SIZE_MSB_POS		12
#define CLNT_ID_SIZE_LSB_POS		13


#define CLEAN_S_FLAG			(1<<1)
#define WILL_FLAG				(1<<2)
#define WILL_QOS_FLAG			(3<<3)
#define WILL_RETAIN_FLAG		(1<<5)
#define USR_NAME_FLAG			(1<<6)
#define PSWD_FLAG				(1<<7)

#define STRINGS_EQUAL			(0)

typedef struct{
uint8_t data[256];
uint8_t len;
}local_host_t;

typedef struct{
	uint16_t len;
	char * data;
}string_in_frame_t;




typedef struct{
	string_in_frame_t * client_id;
	string_in_frame_t * will_topic;
	string_in_frame_t * will_msg;
	string_in_frame_t * usr_name;
	string_in_frame_t * pswd;
}payload_t;


//przerobic na system offsetow;


typedef struct{
uint16_t * client_id;
uint16_t * will_topic;
uint16_t * will_msg;
uint16_t * user_name;
uint16_t * user_pswd;
}pld_offsets_t;



typedef struct{
	uint8_t reserved 	   :1;
	uint8_t cleans_session :1;
	uint8_t last_will      :1;
	uint8_t will_qos       :2;
	uint8_t will_retain    :1;
	uint8_t psswd          :1;
	uint8_t user_name      :1;
}conn_flags_t;




typedef struct {
	uint8_t net_address[ADDR_SIZE];
	char*  id;
	bool keepalive;
	char*  username;
	char*  password;
	bool last_will;
	char*  will_topic;
	char*  will_msg;
	uint8_t will_qos;
	uint8_t will_retain;
	char* subs_topic[MAX_SUBS_TOPIC];
	bool active;
}  conn_client_t;

typedef struct{
	conn_client_t clients[MAX_CONN_CLIENTS];
	MqttNet * net;
}broker_t;

void * m_malloc(size_t size);

#endif /* INC_TINY_BROKER_H_ */
