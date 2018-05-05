

#include <stdint.h>
#include "nrf24.h"
#include "stm32f10x_gpio.h"
#include "mqtt_client.h"


volatile static int debug_var;
uint8_t temp;
uint8_t q = 0;
uint8_t data_array[32];
uint8_t big_array[1500];
uint8_t layer3_buff[230];
uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
/* ----------------------------------------------------------- */



//int mqtt_send(void* socket_info, const void* buf, unsigned int count){
//	l3_send_packet(tx_address, buf, count);
//}
//

	int mqtt_message_cb(struct _MqttClient *client, MqttMessage *message, byte msg_new, byte msg_done){
		;
	}


int main()
{
  
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
	MqttNet net;

	uint8_t tx_buf[64];
	uint8_t tx_buf_len = 64;
	byte rx_buf[64];
	int rx_buf_len;
	int cmd_timeout_ms =500;
	MqttClient_Init(&client, &net, mqtt_message_cb, tx_buf, tx_buf_len, rx_buf, rx_buf_len, cmd_timeout_ms);


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
