#ifndef __NRF24_HAL_H
#define __NRF24_HAL_H


// Hardware abstraction layer for NRF24L01+ transceiver (hardware depended functions)
// GPIO pins definition
// GPIO pins initialization and control functions
// SPI transmit functions


// Peripheral libraries
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>


typedef struct{
	uint8_t addr;
	uint8_t func;
	uint8_t data[8];
	uint8_t crc;
}data_frm_t;



// SPI port peripheral
#define nRF24_SPI_PORT             SPI1

// nRF24 GPIO peripherals
#define nRF24_GPIO_PERIPHERALS     (RCC_APB2ENR_IOPBEN) //+ PB dla CS

// CE (chip enable) pin (PB6)
#define nRF24_CE_PORT              GPIOB
#define nRF24_CE_PIN               GPIO_Pin_0
#define nRF24_CE_L()               GPIO_ResetBits(nRF24_CE_PORT, nRF24_CE_PIN)
#define nRF24_CE_H()               GPIO_SetBits(nRF24_CE_PORT, nRF24_CE_PIN)

// CSN (chip select negative)
#define nRF24_CSN_PORT             GPIOB
#define nRF24_CSN_PIN              GPIO_Pin_1
#define nRF24_CSN_L()              GPIO_ResetBits(nRF24_CSN_PORT, nRF24_CSN_PIN)
#define nRF24_CSN_H()              GPIO_SetBits(nRF24_CSN_PORT, nRF24_CSN_PIN)

// IRQ pin (PB10)
#define nRF24_IRQ_PORT            // GPIOB
#define nRF24_IRQ_PIN             //GPIO_Pin_10


// Function prototypes
void nRF24_GPIO_Init(void);
uint8_t nRF24_LL_RW(uint8_t data);
//uint8_t nrF24_set_as_TX (void);
//void nrf24l01_set_as_RX(void);


void nrf24l01_set_up_general();
void nrf24l01_set_as_RX (const uint8_t * addr);
uint8_t nrF24_set_as_TX (const uint8_t * addr);
void nrf24l01_send (uint8_t * frm, uint8_t len);
#endif // __NRF24_HAL_H
