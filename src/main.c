

#include <stdint.h>
#include "nrf24.h"
#include "stm32f10x_gpio.h"
#include "mqtt_client.h"
#include "tiny_broker.h"
#include "string.h"

volatile static int debug_var;
uint8_t temp;
uint8_t q = 0;
uint8_t data_array[32];
uint8_t big_array[1500];
uint8_t layer3_buff[230];
uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
/* ----------------------------------------------------------- */




local_host_t local_host;

void packet_send_localhost(uint8_t * data, uint8_t size){
	memcpy(local_host.data, data, size);
	local_host.len = size;
}


//int mqtt_send(void* socket_info, const void* buf, unsigned int count){
//	l3_send_packet(tx_address, buf, count);
//}
//

	int mqtt_message_cb(struct _MqttClient *client, MqttMessage *message, byte msg_new, byte msg_done){
		return 1;
	}


	int mqt_net_connect_cb (void *context, const char* host, word16 port, int timeout_ms){
		return 1;
	}

	int mqtt_net_read_cb(void *context, byte* buf, int buf_len, int timeout_ms){
		memcpy(buf, &local_host.data[local_host.pos], buf_len);
		local_host.pos += buf_len;
		return buf_len;
		;
	}


//	int mqtt_net_write_cb(void *context, const byte* buf, int buf_len, int timeout_ms){
//		uint8_t * broker_address = (uint8_t*) context;
//		l3_send_packet(broker_address, buf, buf_len);
//		return buf_len;
//	}


	int mqtt_net_write_cb(void *context, const byte* buf, int buf_len, int timeout_ms){
		packet_send_localhost(buf, buf_len);
		return 0;
	}


	int mqtt_net_disconnect_cb(void *context){
		return 0;
	}

	MqttNet net;
int main()
{


	memset(&local_host, 0, 256);

    nrf24_init();
	nRF24_restore_defaults();


    
    /* Channel #2 , payload length: 4 */
    nrf24_config(2,32);
 
    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);


	GPIO_InitTypeDef PORT;
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;
	PORT.GPIO_Speed = GPIO_Speed_2MHz;
	PORT.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &PORT);
	GPIOC->ODR |= GPIO_Pin_13;
    


	PORT.GPIO_Mode =  GPIO_Mode_IPU;
	PORT.GPIO_Speed = GPIO_Speed_2MHz;
	PORT.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &PORT);
	GPIOC->ODR |= GPIO_Pin_13;


	MqttClient client;

	net.connect = mqt_net_connect_cb;
	net.read = mqtt_net_read_cb;
	net.write = mqtt_net_write_cb;
	net.disconnect = mqtt_net_disconnect_cb;


	uint8_t * tx_buf = local_host.data;
	memset(tx_buf, 0, 64);
	uint8_t tx_buf_len = 64;
	uint8_t * rx_buf = local_host.data;
	int rx_buf_len =64;
	int cmd_timeout_ms =500;
	MqttClient_Init(&client, &net, mqtt_message_cb, tx_buf, tx_buf_len, rx_buf, rx_buf_len, cmd_timeout_ms);

	MqttConnect mqtt_con;
	mqtt_con.clean_session =0;
	mqtt_con.client_id = "rt1";
	mqtt_con.enable_lwt = 0;
	mqtt_con.keep_alive_sec =30;
	mqtt_con.stat = MQTT_MSG_BEGIN;
	mqtt_con.username ="bedroomTMP1";
	mqtt_con.password = "passw0rd";
	MqttClient_Connect(&client, &mqtt_con);

	MqttEncode_Connect(client.tx_buf, 100, &mqtt_con);
	conn_pck_t conn_pck;


	decode_connect(client.tx_buf, &conn_pck);


	MqttPublish publish;

	const char* test_topic1 = "flat/livingroom/temp/1";
	const char* test_topic2 = "flat/bedroom/humidity/2";
	publish.topic_name = test_topic1;
	publish.topic_name_len = strlen(test_topic1);
	uint8_t temp = 25;
	publish.buffer = &temp;
	publish.total_len = sizeof(temp);
	uint16_t pck_id = 1;
	publish.packet_id = pck_id;
	publish.qos = 1;
	publish.retain = (byte) true;
	publish.stat = 0;

	MqttEncode_Publish(client.tx_buf, client.tx_buf_len, &publish);
	pub_pck_t  pub_pck;
	broker_decode_publish(local_host.data, &pub_pck);


    MqttTopic topics[2];
    topics[0].qos =1;
    topics[0].topic_filter = test_topic1;

    topics[1].qos =1;
    topics[1].topic_filter = test_topic2;



	MqttSubscribe subscribe;
	subscribe.packet_id = pck_id;
	uint8_t topic_count = 2;
	subscribe.topic_count = topic_count;
	subscribe.topics = topics;

	memset(client.tx_buf, 0, 100);
	MqttEncode_Subscribe(client.tx_buf, client.tx_buf_len, &subscribe);
	sub_pck_t sub_pck;
	broker_decode_subscribe(client.tx_buf, &sub_pck);
	//MqttClient_Subscribe(&client, &subscribe);



//
    while(1)
    {    

//
//        /* Fill the data buffer */
//         data_array[0] = 'a';
//         data_array[1] = 'n';
//         data_array[2] = 's';
//         data_array[3] = 'w';
//
//        if ((GPIOB->MqttEncode_PublishIDR  & GPIO_IDR_IDR11) == 0){
//        	/* Automatically goes to TX mode */
//        	nrf24_send(data_array);
//
//
//            /* Make analysis on last tranmission attempt */
//            temp = nrf24_lastMessageStatus();
//
//
//
//
//            if(temp == NRF24_TRANSMISSON_OK)
//            {
//            //	 GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//            	debug_var++;
//    //            xprintf("> Tranmission went OK\r\n");
//            }
//            else if(temp == NRF24_MESSAGE_LOST)
//            {
//            	debug_var++;
//    //            xprintf("> Message is lost ...\r\n");
//            }
//
//    		/* Retranmission count indicates the tranmission quality */
//    		temp = nrf24_retransmissionCount();
//    		debug_var+= temp;
//
//    		/* Optionally, go back to RX mode ... */
//    		nrf24_powerUpRx();
//    	    _delay_ms(50);
//
//
//    		/* Or you might want to power down after TX */
//    		// nrf24_powerDown();
//
//       }
//

	   if(nrf24_dataReady())
		{
			//GPIOC->ODR ^=  GPIO_Pin_13;
			nrf24_getData(data_array);
			bool res = l3_receive_packet(data_array,layer3_buff );
			if (res){
				GPIOC->BRR = GPIO_Pin_13;
				nrf24_send(data_array);
			}

//			if (data_array[0] % 2){
//				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//				_delay_ms(100);
//			}
//			else{
//				GPIO_SetBits(GPIOC, GPIO_Pin_13);
//				_delay_ms(100);
//			}

		//	nrf24_send(data_array);
		//	nrf24_powerUpRx();

//			temp = nrf24_lastMessageStatus();
//
//			if(temp == NRF24_TRANSMISSON_OK)
//			{
//				GPIO_SetBits(GPIOC, GPIO_Pin_13);
//
//			}
//			else if(temp == NRF24_MESSAGE_LOST)
//			{
//				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//				debug_var++;
//			}


		}
		_delay_ms(10);
    }
}
