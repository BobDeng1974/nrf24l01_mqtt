
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





9 bajt z ramki to bajt z flagami: (tu 192, czy 11000000)
User Name Flag (1)

Password Flag (1)

Will Retain (0)

Will QoS (01)

Will Flag (1)

Clean Session (1)

Reserved (0)
mozna go sparsowac, np

uint8_t byte = frame[9];
broker->clients[i].last_will = byte_flag & 1<<LAST_WILL_FLAG;


broker->clients[i].


void acccept_connection (broker_t * broker, uint8_t * frame){
	 for (uint8_t i =0; i < MAX_CONN_CLIENTS; i++){
		 if (!(broker->clients[i].client_id)){
			 uint8_t client_id_size = frame[13];
			 broker->clients[i].client_id = XMALLOC(client_id_size+1);
			 char * client_id =  frame[14];
			 memcpy(broker->clients[i].client_id, &frame[14], client_id_size);
			 broker->clients[i].client_id[client_id_size+1] = 0;

		 }
	 }
}




 void publish_msg_to_subscribers(broker_t * broker, uint8_t * frame, uint8_t len){
	 for (uint8_t i =0; i < MAX_CONN_CLIENTS; i++){
		 if ((broker->clients->client_id)){
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


