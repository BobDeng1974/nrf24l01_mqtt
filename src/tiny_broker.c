
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



static bool can_broker_accept_next_client(broker_t * broker){
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




static inline void read_conn_header(conn_header_t *header, uint8_t * frame){
	uint8_t pos = 0;
	header->fix_head =  (conn_fixed_header_t *) &frame[pos];
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





static inline void read_conn_payload(conn_pld_t *payload, conn_header_t* header, uint8_t* frame ){
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
		payload->usr_name_len = (uint16_t*)  &frame[pos];
		*payload->usr_name_len = X_HTONS(* payload->usr_name_len);
		pos += 2;
		payload->usr_name= (char*) &frame[pos];
		pos += *payload->usr_name_len;
	}
	if (header->conn_flags->pswd){
		payload->pswd_len = (uint16_t*)  &frame[pos];
		*payload->pswd_len = X_HTONS(* payload->pswd_len);
		pos += 2;
		payload->pswd= (char*) &frame[pos];
		pos += *payload->pswd_len;
	}

}


bool is_client_authorised(char* usr_name, char* pswd){
	return true;
}


uint8_t * format_conn_ack(header_conn_ack_t * header_ack, bool session_pres, uint8_t code){
	memset(header_ack, 0, sizeof (header_conn_ack_t));
	header_ack->control_type = (CONTR_TYPE_CONNACK << 4);
	header_ack->remainin_len = CONN_ACK_PLD_LEN;
	header_ack->ack_flags.session_pres = session_pres;
	header_ack->conn_code = code;
	return (uint8_t *)header_ack;
}



void broker_fill_new_client(conn_client_t *new_client, const conn_pck_t * conn_pck){
	conn_header_t * header = conn_pck->head;
	conn_flags_t * flags = header->conn_flags;
	conn_pld_t * payload = conn_pck->pld;

	// all len should be extracted to separate  variable!!
	new_client->id = X_MALLOC((*payload->client_id_len)+1);
	strncpy(new_client->id,  payload->client_id, *payload->client_id_len);

	new_client->keepalive = *header->keep_alive;

	if (flags->will_retain){
		new_client->will_retain = 1;
	}

	if (flags->last_will){
		new_client->will_retain = 1;
		new_client->will_topic = X_MALLOC((*payload->will_topic_len)+1);
		strncpy(new_client->will_topic,  payload->will_topic, *payload->will_topic_len );

		new_client->will_msg = X_MALLOC((*payload->will_msg_len)+1);
		strncpy(new_client->will_msg,  payload->will_msg, *payload->will_msg_len);

		new_client->will_qos = flags->will_qos;

	}

	if (flags->user_name){
		new_client->username = X_MALLOC((*payload->usr_name_len)+1);
		strncpy(new_client->username,  payload->usr_name, *payload->usr_name_len);
	}

	if (flags->pswd){
		new_client->password = X_MALLOC((*payload->pswd_len)+1);
		strncpy(new_client->password,  payload->pswd, *payload->pswd_len);
	}
}

// https://www.bevywise.com/developing-mqtt-clients/
// https://morphuslabs.com/hacking-the-iot-with-mqtt-8edaf0d07b9b ack codes



void broker_decode_connect (broker_t * broker, uint8_t * frame, conn_pck_t * conn_pck ){
	read_header(conn_pck->head, frame);
	read_conn_payload(conn_pck->pld, conn_pck->head, frame);

}

void broker_mantain_new_connect (broker_t *broker, conn_pck_t *conn_pck, conn_ack_stat_t * stat){



	if  (*conn_pck->head->proto_level != PROTO_LEVEL_MQTT311){
		stat->session_present = false;
		stat->code = CONN_ACK_BAD_PROTO;
		return;
	}

	if (conn_pck->head->conn_flags->cleans_session){
		if (broker_remove_client(broker, conn_pck->pld->client_id)){
		}
	}

	if (is_client_connected(broker, conn_pck->pld->client_id)){
		stat->session_present = true;
		stat->code = CONN_ACK_OK;
		return;
	}

	if (can_broker_accept_next_client(broker))
	{
		conn_client_t new_client;
		broker_fill_new_client(&new_client, conn_pck);

		if (is_client_authorised(new_client.username, new_client.password)){
			add_client(broker, &new_client);
			stat->session_present = false;
			stat->code = CONN_ACK_OK;
			return;

		}else{
			stat->session_present = false;
			stat->code = CONN_ACK_BAD_AUTH;
			return;
		}
	} else {
		stat->session_present = false;
		stat->code = CONN_ACK_NOT_AVBL;
		return;
	}
}


void broker_send_conn_ack(broker_t * broker,  conn_ack_stat_t * stat){
	header_conn_ack_t header_ack;
	format_conn_ack(&header_ack, stat->session_present, stat->code);
	uint8_t * buf = (uint8_t *) &header_ack;
	uint8_t buf_len = sizeof(header_conn_ack_t);
	broker->net->write(NULL, buf, buf_len, DEFAULT_BROKER_TIMEOUT);

}





static inline void broker_decode_publish(uint8_t* frame, pub_msg_t * pub_msg){
	uint8_t pos = 0;

	pub_msg->head = (pub_header_t *) frame;
	pos += sizeof (pub_header_t);

	pub_msg->pld->topic_name_len  = (uint16_t*) &frame[pos];
	*pub_msg->pld->topic_name_len = X_HTONS(*pub_msg->pld->topic_name_len);
	pos += 2;


	if (pub_msg->head->QoS > 0){
	}
}




void publish_msg_to_subscribers(broker_t * broker, pub_msg_t * pub_msg){
	for (uint8_t i =0; i < MAX_CONN_CLIENTS; i++){
		if ((broker->clients->id)){
			for (uint8_t j =0; j < MAX_SUBS_TOPIC; j++){
				uint16_t len = *pub_msg->pld->topic_name_len;
				unsigned char* topic = pub_msg->pld->topic_name;
				if (memcmp (broker->clients[i].subs_topic[j], topic, len)){
					broker->net->write(broker->clients[i].net_address, topic, len, BROKER_TIMEOUT);
					break;
				}
			}
		}
	}
}







static inline void read_subscribe_payload(pub_pld_t* payload, pub_header_t *header, uint8_t* frame){
	uint8_t pos = sizeof (pub_header_t);

	payload->topic_name_len  = (uint16_t*) &frame[pos];
	*payload->topic_name_len = X_HTONS(*payload->topic_name_len);
	pos += 2;
	payload->topic_name = (char*) &frame[pos];
	pos += *payload->topic_name_len;


	if (header->QoS > 0){
	}
}


/* alternatively  decode publish msg (wich contains head&pld) */
void broker_decode_subscribe (uint8_t * frame, pub_msg_t * pub_msg, conn_ack_stat_t * stat){
	pub_msg->head = (pub_header_t *) frame;
	read_publish_payload(pub_msg->pld, pub_msg->head, frame);
}






//broker->net->write(context, buf, buf_len, timeout_ms);

//broker->net->write(void *context, const byte* buf, int buf_len, int timeout_ms);


 void add_subscribtion(broker_t * broker, uint8_t * client_addr, uint8_t *frame){
	 for (uint8_t i =0; i < MAX_CONN_CLIENTS; i++){
		 if (memcmp(&broker->clients[i].net_address, client_addr, ADDR_SIZE)){
			 for (uint8_t j =0; j < MAX_SUBS_TOPIC; j++){
				 if (!(broker->clients[i].subs_topic[j])){
					 unsigned char * topic_to_subs = &frame[5];
					 uint8_t topic_len = frame[3] + (frame[4]<<8);
					 broker->clients[i].subs_topic[j] =  X_MALLOC(topic_len);
					 memcpy(broker->clients[i].subs_topic[j], topic_to_subs, topic_len);
				 }
			 }
		 }
	 }
 }







