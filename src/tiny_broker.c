/*
 * tiny_broker.c
 *
 *  Created on: 07.05.2018
 *      Author: tomek
 */

#include "tiny_broker.h"
#include <string.h>
#define BROKER_TIMEOUT		60


 void broker_init (broker_t * broker, MqttNet* net){
 	memset(broker, 0, sizeof(broker_t));
 	broker->net = net;
 }


 uint8_t get_topics_nb (broker_t * broker){
	 for (uint8_t i; i<MAX_SUBS_TOPIC; i++){
		 if (broker->topics[i].topic_name){
			 return i;
		 }
	 }
	 return NOT_FOUND;
 }




void  put_topic_in_list(broker_t * broker, char * topic, uint8_t pos){
	 memcpy(broker->topics[pos].topic_name, topic, strl(topic));
}


void  remove_topic_from_list(broker_t * broker, char * topic, uint8_t pos){
	 memset(broker->topics[pos].topic_name, 0, strl(topic));
}



 void create_new_topic(broker_t * broker, char * topic){
	 uint8_t topic_nb = get_topics_nb(broker);
	 uint8_t pos_to_put = topic_nb + 1;
	 put_topic_in_list(broker, topic, pos_to_put);
 }


 uint8_t find_topic_pos(broker_t * broker, uint8_t * topic, uint8_t topic_len){
 	for (uint8_t i = 0; i < MAX_SUBS_TOPIC; i++){
 		if (broker->topics[i].topic_name){
 			if (memcmp(broker->topics[i].topic_name, topic, strl(topic) == 0)){
 				return i;
 			}
 		}
 	}
 	return NOT_FOUND;
 }

 void publish_msg_to_subsribers(broker_t * broker, uint8_t * topic, uint8_t topic_len, uint8_t * message, uint8_t msg_len){
	 uint8_t pos = find_topic_pos(broker, topic, topic_len);
	 if (pos != NOT_FOUND){
		 for (uint8_t i; i< MAX_CONN_CLIENTS; i++){
			 publish_to_client(broker, broker->topics.client_id)
		 }
	 }
 }

 void publish_to_client(broker_t * broker, uint8_t * client_id){
	 broker->net->write(broker->net->context, topic, topic_len,
						 BROKER_TIMEOUT);
 }

