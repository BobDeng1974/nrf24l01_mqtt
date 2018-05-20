
/*
 * tiny_broker.c
 *
 *  Created on: 07.05.2018
 *      Author: tomek
 */

#include "tiny_broker.h"
#include <string.h>

#define BROKER_TIMEOUT		60
#define TOPIC_POS 			6
#define M_HEAP_SIZE			512

#define XMALLOC				m_malloc

extern local_host_t local_lost;


 void broker_init (broker_t * broker, MqttNet* net){
 	memset(broker, 0, sizeof(broker_t));
 	broker->net = net;
 }


void * m_malloc(size_t size){
	static uint8_t m_heap[M_HEAP_SIZE];
	static uint16_t prev_used_bytes_nb;
	static uint16_t new_used_bytes_nb;
	prev_used_bytes_nb = new_used_bytes_nb;
	new_used_bytes_nb = prev_used_bytes_nb + size;
	if (new_used_bytes_nb < M_HEAP_SIZE){
		return &m_heap[prev_used_bytes_nb];
	}
	else{
		return NULL;
	}
}

//
//
//
//
//9 bajt z ramki to bajt z flagami: (tu 192, czy 11000000)
//User Name Flag (1)
//
//Password Flag (1)
//
//Will Retain (0)
//
//Will QoS (01)
//
//Will Flag (1)
//
//Clean Session (1)
//
//Reserved (0)
//mozna go sparsowac, np
//
//uint8_t byte = frame[9];
//broker->clients[i].last_will = byte_flag & 1<<LAST_WILL_FLAG;
//
//
//broker->clients[i].



bool is_client_connected(broker_t * broker, char* client_id){
	for (uint8_t i =0; i < MAX_CONN_CLIENTS; i++){
		if (strstr(broker->clients[i].username, client_id)){
			return true;
		}
	}
	return false;
}

static inline void read_connection_flags(conn_flags_t ** conn_flags, uint8_t * frame){
	uint8_t * flag_byte = &frame[9];
	*conn_flags =  (conn_flags_t*) flag_byte;
	//zwracac wskaznik
}

static inline uint16_t get_id_size(uint8_t *frame){
	return (frame[CLNT_ID_SIZE_LSB_POS] +  (frame[CLNT_ID_SIZE_MSB_POS]<<8));
}



static bool has_broker_space_for_next_client(broker_t * broker){
	for (uint8_t i = 0; i < MAX_CONN_CLIENTS; i++){
		if (!(broker->clients[i].active)){
			return true;
		}
	}
	return false;
}

static inline uint8_t broker_find_client_pos(broker_t * broker, char* client_id){
	for (uint8_t i = 0; i < MAX_CONN_CLIENTS; i++){
		if (strcmp(broker->clients[i].id, client_id) ==0 ) {
			return i;
		}
	}

	return NOT_FOUND;

}

void broker_remove_client(broker_t * broker, char* client_id){
	uint8_t pos = broker_find_client_pos(broker, client_id);
	if (pos != NOT_FOUND){
		broker->clients[pos].active = false;
	}

}


//static conn_client_t * get_free_slot_for_client(broker_t * broker){
//	for (uint8_t i = 0; i < MAX_CONN_CLIENTS; i++){
//		if (!(broker->clients[i].active)){
//			return &broker->clients[i];
//		}
//	}
//	return NULL;
//}
//

static inline void init_payload_container(payload_t * payload){
	memset (payload, 0, sizeof (payload_t));
}



static inline void read_conn_payload(payload_t* payload, conn_flags_t* flags, uint8_t* frame ){
	init_payload_container(payload);
	uint8_t pos = PLD_START;

	payload->client_id  = (string_in_frame_t*) &frame[pos];
	pos += payload->client_id->len;
	if (flags->last_will){
		payload->will_topic = &frame[pos];
		pos += payload->will_topic->len;
		payload->will_msg   = &frame[pos];
		pos += payload->will_topic->len;
	}
	if (flags->user_name){
		payload->usr_name= &frame[pos];
		pos += payload->usr_name->len;
	}
	if (flags->pswd){
		payload->pswd= &frame[pos];
		pos += payload->pswd->len;
	}
}




//
//
////conn flags odczytywac tu
//static inline void process_payload(uint8_t* frame ){
//	conn_flags_t * flags;
//	read_connection_flags(&flags, frame);
//	payload_t payload;
//	init_payload_container(&payload);
//
//
//	uint8_t pos = PLD_START;
//	payload.client_id  = (string_in_frame_t*) &frame[pos];
//	pos += payload.client_id->len;
//	if (flags->last_will){
//		payload.will_topic = &frame[pos];
//		pos += payload.will_topic->len;
//		payload.will_msg   = &frame[pos];
//		pos += payload.will_topic->len;
//	}
//	if (flags->user_name){
//		payload.usr_name= &frame[pos];
//		pos += payload->usr_name->len;
//	}
//	if (flags->psswd){
//		payload.pswd= &frame[pos];
//		pos += payload.pswd->len;
//	}
//}
//

//
//
//
//static inline void read_client_id(char* id, vhead_offsets_t * offsets, uint8_t * frame ){
//	offsets. = &frame[CLNT_ID_POS];
//	*id_ptr =  (client_id_ptr_t*) client_id_len_start;
//}
//
//
//
//static inline void assign_id_to_client(conn_client_t * client, char* id) {
//	client->id = XMALLOC(strlen(id));
//	strcpy(client->id, id);
//}
//
//
//
//
//static inline void read_will_topic(will_topic_ptr_t ** will_topic_ptr, uint8_t * frame){
//	uint8_t * will_topic_len_start = &frame[10];
//	*will_topic_ptr = (will_topic_ptr_t*) will_topic_len_start;
//}
//
//
//static inline void assign_will_topic(conn_client_t * client, will_topic_ptr_t * will_topic_ptr ) {
//	client->will_topic = XMALLOC(will_topic_ptr->len+1);
//	strcpy(client->will_topic, (char*) will_topic_ptr->data);
//}
//
//
//
//
//
//static inline void read_will_msg(will_msg_ptr_t ** will_msg_ptr, uint8_t * frame){
//	uint8_t * will_msg_len_start = &frame[9];
//	*will_msg_ptr =  (will_msg_ptr_t*) will_msg_len_start;
//}
//
//
//static inline void assign_will_msg(conn_client_t * client, will_msg_ptr_t * will_msg_ptr ) {
//	client->will_msg = XMALLOC(will_msg_ptr->len+1);
//	strcpy(client->will_msg, (char*) will_msg_ptr->data);
//}
//
//



void acccept_connection (broker_t * broker, uint8_t * frame){

	conn_flags_t * conn_flags;
	read_connection_flags(&conn_flags, frame);

	payload_t payload;
	read_conn_payload(&payload, conn_flags, frame);



	if (conn_flags->cleans_session){
		broker_remove_client(broker, payload.client_id->data);
	}

	if (is_client_connected(broker, payload.client_id->data)){
		return;
	}

	if (has_broker_space_for_next_client(broker))
	{

		conn_client_t new_client;
		new_client.id = XMALLOC(strlen(payload.client_id->data));
		strcpy(new_client.id,  payload.client_id->data);

		if (conn_flags->will_retain){
			new_client.will_retain = 1;
		}

		if (conn_flags->last_will){
			new_client.will_retain = 1;
			new_client.will_topic = XMALLOC(strlen(payload.will_topic->data));
			strcpy(new_client.will_topic,  payload.will_topic->data);

			new_client.will_msg = XMALLOC(strlen(payload.client_id->data));
			strcpy(new_client.will_topic,  payload.will_topic->data);

			memcpy(new_client.will_qos, conn_flags->will_qos, sizeof(new_client.will_qos));

		}

		if (conn_flags->user_name){
			new_client.username = XMALLOC(strlen(payload.usr_name->data));
			strcpy(new_client.username,  payload.usr_name->data);
		}

		if (conn_flags->pswd){
			new_client.password = XMALLOC(strlen(payload.pswd->data));
			strcpy(new_client.password,  payload.pswd->data);
		}


		return;
	}

}





 void publish_msg_to_subscribers(broker_t * broker, uint8_t * frame, uint8_t len){
	 for (uint8_t i =0; i < MAX_CONN_CLIENTS; i++){
		 if ((broker->clients->id)){
			 for (uint8_t j =0; j < MAX_CONN_CLIENTS; j++){
				 char * topic_name = (char *) &frame[5];
				 uint8_t topic_len = frame[3] + (frame[4]<<8);
				 if (memcmp (broker->clients[i].subs_topic[j], topic_name, topic_len)){
					 broker->net->write(broker->clients[i].net_address, frame, len, BROKER_TIMEOUT);
					 break;
				 }
			 }
		 }
	 }
 }


 void add_subscribtion(broker_t * broker, uint8_t * client_addr, uint8_t *frame){
	 for (uint8_t i =0; i < MAX_CONN_CLIENTS; i++){
		 if (memcmp(&broker->clients[i].net_address, client_addr, ADDR_SIZE)){
			 for (uint8_t j =0; j < MAX_SUBS_TOPIC; j++){
				 if (!(broker->clients[i].subs_topic[j])){
					 unsigned char * topic_to_subs = &frame[5];
					 uint8_t topic_len = frame[3] + (frame[4]<<8);
					 broker->clients[i].subs_topic[j] = (unsigned char  *) XMALLOC(topic_len);
					 memcpy(broker->clients[i].subs_topic[j], topic_to_subs, topic_len);
				 }
			 }
		 }
	 }
 }



