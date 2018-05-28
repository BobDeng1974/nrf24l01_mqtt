
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
#define NOT_FOUND 			255
#define X_MALLOC				m_malloc

#define X_HTONS(a) ((a>>8) | (a<<8))

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

bool broker_remove_client(broker_t * broker, char* client_id){
	uint8_t pos = broker_find_client_pos(broker, client_id);
	if (pos != NOT_FOUND){
		broker->clients[pos].active = false;
		return true;
	}
	return false;
}


static uint8_t broker_first_free_pos_for_client(broker_t * broker){
	for (uint8_t i = 0; i < MAX_CONN_CLIENTS; i++){
		if (!(broker->clients[i].active)){
			return i;
		}
	}
	return NOT_FOUND;
}


static void add_client (broker_t * broker, conn_client_t * new_client){
	uint8_t pos = broker_first_free_pos_for_client(broker);
		memcpy(&broker->clients[pos], new_client, sizeof (conn_client_t));
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



static inline void init_header_container(header_t * header){
	memset (header, 0, sizeof (header_t));
}


static inline void read_header(header_t* header, uint8_t * frame){
	uint8_t pos = 0;
	header->fixed_header =  (fixed_header_t *) &frame[pos];
	pos += 2;
	header->len = (uint16_t*) &frame[pos];
	*header->len = X_HTONS(*header->len);
	pos += 2;
	header->proto_name = (char*) &frame[pos];
	pos += *header->len;
	header->proto_level = (uint8_t*) &frame[pos];
	pos += 1;
	header->conn_flags = (conn_flags_t*) &frame[pos];
	pos += 1;
	header->keep_alive = (uint16_t*)  &frame[pos];
	*header->keep_alive = X_HTONS(*header->keep_alive);

}



static inline void init_payload_container(payload_t * payload){
	memset (payload, 0, sizeof (payload_t));
}



static inline void read_conn_payload(payload_t* payload, header_t* header, uint8_t* frame ){
	uint8_t pos = PLD_START;

	payload->client_id_len  = (uint16_t*) &frame[pos];
	*payload->client_id_len = X_HTONS(*payload->client_id_len);
	pos += 2;
	payload->client_id = (char*) &frame[pos];
	pos += *payload->client_id_len;


	if (header->conn_flags->last_will){

		payload->will_topic_len = (uint16_t*)  &frame[pos];
		*payload->will_topic_len = X_HTONS(* payload->will_topic_len);
		pos += 2;
		payload->will_topic = (char*)  &frame[pos];
		pos += *payload->will_topic_len;

		payload->will_msg_len = (uint16_t*)  &frame[pos];
		*payload->will_msg_len = X_HTONS(* payload->will_msg_len);
		pos += 2;
		payload->will_msg = (char*)  &frame[pos];
		pos += *payload->will_msg_len;
	}
	if (header->conn_flags->user_name){
		payload->usr_name= (char*) &frame[pos];
		pos += *payload->usr_name_len;
	}
	if (header->conn_flags->pswd){
		payload->pswd= (char*) &frame[pos];
		pos += *payload->pswd_len;
	}

}


bool is_client_authorised(char* usr_name, char* pswd){
	return true;
}

przemyslec i wykorzystac
uint8_t * format_conn_ack(header_conn_ack_t * header_ack, uint8_t conn_ack_code, bool alread_conn){
	header_ack->control_type = CONNACK;
	header_ack->conn_code = conn_ack_code;
	header_ack->ack_flags = SESSION_PRESENT;
	header_ack->remainin_len = CONN_ACK_PLD_LEN;
	return (uint8_t *)header_ack;
}


void broker_send_con_rsp(broker_t * broker, uint8_t * conn_ack_id){

}



// https://www.bevywise.com/developing-mqtt-clients/
// https://morphuslabs.com/hacking-the-iot-with-mqtt-8edaf0d07b9b ack codes


void acccept_connection (broker_t * broker, uint8_t * frame){

	header_t header;
	read_header(&header, frame);

	payload_t payload;
	read_conn_payload(&payload, &header, frame);

	header_conn_ack_t header_ack;
	memset(&header_ack, 0, sizeof (header_conn_ack_t));
	header_ack.control_type = (CONTR_TYPE_CONNACK << 4);
	header_ack.remainin_len = CONN_ACK_LEN;

	if  (*header.proto_level != PROTO_LEVEL_MQTT311){
		header_ack.conn_code = CONN_ACK_BAD_PROTO;
	}

	if (header.conn_flags->cleans_session){
		if (broker_remove_client(broker, payload.client_id)){
			header_ack.ack_flags.session_pres = true;
		}
	}

	if (is_client_connected(broker, payload.client_id)){
		header_ack.ack_flags.session_pres = true;
		//broker_send_con_rsp(CONN_ACK_OK);
	}

	if (has_broker_space_for_next_client(broker))
	{
		conn_client_t new_client;
		new_client.id = X_MALLOC(*payload.client_id_len);
		strcpy(new_client.id,  payload.client_id);

		new_client.keepalive = *header.keep_alive;

		if (header->conn_flags.will_retain){
			new_client.will_retain = 1;
		}

		if (header->conn_flags.last_will){
			new_client.will_retain = 1;
			new_client.will_topic = X_MALLOC(strlen(payload.will_topic->data));
			strcpy(new_client.will_topic,  payload.will_topic->data);

			new_client.will_msg = X_MALLOC(strlen(payload.client_id->data));
			strcpy(new_client.will_topic,  payload.will_topic->data);

			memcpy(new_client.will_qos, header->conn_flags.will_qos, sizeof(uint8_t));

		}

		if (header->conn_flags.user_name){
			new_client.username = X_MALLOC(strlen(payload.usr_name->data));
			strcpy(new_client.username,  payload.usr_name->data);
		}

		if (header->conn_flags.pswd){
			new_client.password = X_MALLOC(strlen(payload.pswd->data));
			strcpy(new_client.password,  payload.pswd->data);
		}

		if (is_client_authorised(new_client.username, new_client.password)){
			add_client(broker, &new_client);

			//broker_send_con_rsp(CONN_ACK_OK);
		}else{
		//	broker_send_con_rsp(CONN_ACK_BAD_AUTH);
		}





		broker->net->write(void *context, const byte* buf, int buf_len, int timeout_ms);)
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
					 broker->clients[i].subs_topic[j] = (unsigned char  *) X_MALLOC(topic_len);
					 memcpy(broker->clients[i].subs_topic[j], topic_to_subs, topic_len);
				 }
			 }
		 }
	 }
 }



