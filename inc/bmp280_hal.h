/*
 * bmp280_hal.h
 *
 *  Created on: 28 sie 2017
 *      Author: tomek
 */

#ifndef BMP280_HAL_H_
#define BMP280_HAL_H_


#define bmp280_CSN_L()              GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define bmp280_CSN_H()              GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define SPI_BUFFER_LEN 64
#define BUFFER_LENGTH	0xFF
#define	SPI_READ	0x80
#define SPI_WRITE	0x7F
#define BMP280_DATA_INDEX	1
#define BMP280_ADDRESS_INDEX	2



void spi2_init(void);
uint8_t  spi_read_write_string(uint8_t * read, uint8_t* write, uint8_t cnt);
s8  BMP280_SPI_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8  BMP280_SPI_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void  BMP280_delay_msek(u32 msek);
s8 SPI_routine(void);

#endif /* BMP280_HAL_H_ */
