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

#define DEFAULT_BROKER_TIMEOUT		(100)

#define PROTO_LEVEL_MQTT311			(4)

#define MAX_SUBS_TOPIC 				(8)
#define MAX_TOPIC_NAME_SIZE 		(32)
#define NOT_FOUND					(255)
#define MAX_CONN_CLIENTS			(8)
#define ADDR_SIZE					(4)

#define PLD_START					(12)
#define CLNT_ID_POS					(14)
#define CLNT_ID_SIZE_MSB_POS		(12)
#define CLNT_ID_SIZE_LSB_POS		(13)

#define CONN_ACK_PLD_LEN			(2)

#define CLEAN_S_FLAG			(1<<1)
#define WILL_FLAG				(1<<2)
#define WILL_QOS_FLAG			(3<<3)
#define WILL_RETAIN_FLAG		(1<<5)
#define USR_NAME_FLAG			(1<<6)
#define PSWD_FLAG				(1<<7)


#define SESSION_PRESENT			(1<<0)

#define STRINGS_EQUAL			(0)




#define CONN_ACK_OK					(0)
#define CONN_ACK_BAD_PROTO			(1)
#define CONN_ACK_BAD_ID				(2)
#define CONN_ACK_NOT_AVBL			(3)
#define CONN_ACK_AUTH_MALFORM		(4)
#define CONN_ACK_BAD_AUTH			(5)
#define CONN_ACK_OK_SESS_PRESENT	(0)

#define CONTR_TYPE_CONNACK 			(2)

//
//Reserved
//
//
//0
//
//
//Forbidden
//
//
//Reserved
//
//CONNECT
//
//
//1
//
//
//Client to Server
//
//
//Client request to connect to Server
//
//#define CONNACK 	(2)
//
//
//Server to Client
//
//
//Connect acknowledgment
//
//PUBLISH
//
//
//3
//
//
//Client to Server
//
//          or
//
//Server to Client
//
//
//Publish message
//
//PUBACK
//
//
//4
//
//
//Client to Server
//
//          or
//
//Server to Client
//
//
//Publish acknowledgment
//
//PUBREC
//
//
//5
//
//
//Client to Server
//
//          or
//
//Server to Client
//
//
//Publish received (assured delivery part 1)
//
//PUBREL
//
//
//6
//
//
//Client to Server
//
//          or
//
//Server to Client
//
//
//Publish release (assured delivery part 2)
//
//PUBCOMP
//
//
//7
//
//
//Client to Server
//
//          or
//
//Server to Client
//
//
//Publish complete (assured delivery part 3)
//
//SUBSCRIBE
//
//
//8
//
//
//Client to Server
//
//
//Client subscribe request
//
//SUBACK
//
//
//9
//
//
//Server to Client
//
//
//Subscribe acknowledgment
//
//UNSUBSCRIBE
//
//
//10
//
//
//Client to Server
//
//
//Unsubscribe request
//
//UNSUBACK
//
//
//11
//
//
//Server to Client
//
//
//Unsubscribe acknowledgment
//
//PINGREQ
//
//
//12
//
//
//Client to Server
//
//
//PING request
//
//PINGRESP
//
//
//13
//
//
//Server to Client
//
//
//PING response
//
//DISCONNECT
//
//
//14
//
//
//Client to Server
//
//
//Client is disconnecting
//
//Reserved
//
//
//15
//
//


typedef struct{
uint8_t data[256];
uint8_t len;
uint8_t pos;
}local_host_t;

typedef struct{
	uint8_t len_LSB;

	char * data;
}string_in_frame_t;




typedef struct{

	uint16_t * client_id_len;
	char * client_id;
	uint16_t  * will_topic_len;
	char * will_topic;
	uint16_t  * will_msg_len;
	char *  will_msg;
	uint16_t * usr_name_len;
	char* usr_name;
	uint16_t * pswd_len;
	char*  pswd;
}payload_t;



typedef struct{
	uint8_t reserved 	   :1;
	uint8_t cleans_session :1;
	uint8_t last_will      :1;
	uint8_t will_qos       :2;
	uint8_t will_retain    :1;
	uint8_t pswd          :1;
	uint8_t user_name      :1;
}conn_flags_t;





typedef struct{
	uint8_t control_type;
	uint8_t remainin_len;
}fixed_header_t;


typedef struct{
	fixed_header_t * fixed_header;
	uint16_t * len;
	char * proto_name;
	uint8_t * proto_level;
	conn_flags_t * conn_flags;
	uint16_t * keep_alive;
}header_t;


typedef struct{
	uint8_t session_pres   :1;
	uint8_t reserved 	   :7;
}Connect_ack_Flags;


typedef struct{
	uint8_t control_type;
	uint8_t remainin_len;
	Connect_ack_Flags ack_flags;
	uint8_t conn_code;
}header_conn_ack_t;



typedef struct {
	uint8_t retain :1;
	uint8_t QoS :2;
	uint8_t dup :1;
	uint8_t type :4;
	uint8_t len;
}pub_header_t;






typedef struct{
	uint16_t * topic_name_len;
	unsigned char * topic_name;
	uint16_t  * packet_id;
}pub_pld_t;



typedef struct{
	pub_header_t * head;
	pub_pld_t * pld;
}pub_msg_t;





typedef struct{
	bool session_present;
	uint8_t code;
}conn_ack_stat_t;


typedef struct{
	bool session_present;
	uint8_t code;
}pub_ack_stat_t;




typedef struct {
	uint8_t net_address[ADDR_SIZE];
	char*  id;
	uint16_t keepalive;
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

void broker_init (broker_t * broker, MqttNet* net);
void broker_decode_connect (broker_t * broker, uint8_t * frame, conn_ack_stat_t * stat);
void * m_malloc(size_t size);
void broker_fill_new_client(conn_client_t *new_client, header_t *header, payload_t *payload);
void broker_mantain_conn_frame (broker_t * broker, uint8_t * frame, conn_ack_stat_t * stat);
void broker_send_conn_ack(broker_t * broker,  conn_ack_stat_t * stat);

#endif /* INC_TINY_BROKER_H_ */
