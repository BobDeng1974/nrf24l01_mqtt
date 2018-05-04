/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
* Please define your platform spesific functions in this file ...
* -----------------------------------------------------------------------------
*/

#include "stdint.h"
#include "nrf24_hal.h"
#define set_bit(reg,bit) reg |= (1<<bit)
#define clr_bit(reg,bit) reg &= ~(1<<bit)
#define check_bit(reg,bit) (reg&(1<<bit))

/* ------------------------------------------------------------------------- */
void nrf24_setupPins()
{
	nRF24_GPIO_Init();
}




/* ------------------------------------------------------------------------- */
void nrf24_ce_digitalWrite(uint8_t state)
{
    if(state)
    {
    	nRF24_CE_H();
    }
    else
    {
    	 nRF24_CE_L();
    }
}
/* ------------------------------------------------------------------------- */
void nrf24_csn_digitalWrite(uint8_t state)
{
    if(state)
    {
    	nRF24_CSN_H();
    }
    else
    {
    	nRF24_CSN_L();
    }
}
///* ------------------------------------------------------------------------- */
//void nrf24_sck_digitalWrite(uint8_t state)
//{
//    if(state)
//    {
//        set_bit(PORTC,2);
//    }
//    else
//    {
//        clr_bit(PORTC,2);
//    }
//}
///* ------------------------------------------------------------------------- */
//void nrf24_mosi_digitalWrite(uint8_t state)
//{
//    if(state)
//    {
//        set_bit(PORTC,3);
//    }
//    else
//    {
//        clr_bit(PORTC,3);
//    }
//}
///* ------------------------------------------------------------------------- */
//uint8_t nrf24_miso_digitalRead()
//{
//    return check_bit(PINC,4);
//}
///* ------------------------------------------------------------------------- */
