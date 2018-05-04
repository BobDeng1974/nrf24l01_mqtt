#include "stm32f10x.h"
#include "bmp280.h"
#include "bmp280_hal.h"


void spi2_init(void) {

	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI1 and GPIOA clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	//SPI - SCK, MISO, MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//CS hdwr
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_SSOutputCmd(SPI2, ENABLE);
	SPI_Init(SPI2, &SPI_InitStructure);

	/* Enable SPI1  */
	SPI_Cmd(SPI2, ENABLE);

}


uint8_t  spi_read_write_string(uint8_t * read, uint8_t* write, uint8_t cnt){
	 // Wait until TX buffer is empty

	bmp280_CSN_L();
	uint8_t i =0;
	while (i != cnt){

		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
		// Send byte to SPI (TXE cleared)
		SPI_I2S_SendData(SPI2, write[i]);
			// Wait while receive buffer is empty
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);


		read[i] = SPI_I2S_ReceiveData(SPI2);

		i++;
	}
	bmp280_CSN_H();
return 0;
}


s8  bmp280_SPI_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError=BMP280_INIT_VALUE;
	u8 array[SPI_BUFFER_LEN]={BUFFER_LENGTH};
	u8 stringpos;
	/*	For the SPI mode only 7 bits of register addresses are used.
	The MSB of register address denotes the type of SPI data transfer, whether
	read/write (read as 1/write as 0)*/
	array[BMP280_INIT_VALUE] = reg_addr|SPI_READ;
	spi_read_write_string(array, array, cnt+1);
	/*read routine is initiated by masking register address with 0x80*/
	/*
	* Please take the below function as your reference to
	* read the data using SPI communication
	* " IERROR = SPI_READ_WRITE_STRING(ARRAY, ARRAY, CNT+1)"
	* add your SPI read function here
	* iError is an return value of SPI read function
	* Please select your valid return value
	* In the driver SUCCESS defined as 0
	* and FAILURE defined as -1
	* Note :
	* This is a full duplex operation,
	* The first read data is discarded, for that extra write operation
	* have to be initiated. Thus cnt+1 operation done in the SPI read
	* and write string function
	* For more information please refer the SPI communication in data sheet
	*/
	for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++) {
		*(reg_data + stringpos) = array[stringpos+BMP280_DATA_INDEX];
	}
	return (s8)iError;
}

/*	\Brief: The function is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, where data is to be written
 *	\param reg_data : It is a value held in the array,
 *		which is written in the register
 *	\param cnt : The no of bytes of data to be written
 */
s8  bmp280_SPI_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMP280_INIT_VALUE;
	u8 array[SPI_BUFFER_LEN * BMP280_ADDRESS_INDEX];
	u8 stringpos = BMP280_INIT_VALUE;
	u8 index = BMP280_INIT_VALUE;

	for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++) {
		/* the operation of (reg_addr++)&0x7F done as per the
		SPI communication protocol specified in the data sheet*/
		index = stringpos * BMP280_ADDRESS_INDEX;
		array[index] = (reg_addr++) & SPI_WRITE;
		array[index + BMP280_DATA_INDEX] = *(reg_data + stringpos);
	}

	uint8_t read_buff[8];
	spi_read_write_string(read_buff, array, cnt*2 );


	/* Please take the below function as your reference
	 * to write the data using SPI communication
	 * add your SPI write function here.
	 * "IERROR = SPI_WRITE_STRING(ARRAY, CNT*2)"
	 * iError is an return value of SPI write function
	 * Please select your valid return value
	 * In the driver SUCCESS is defined as 0 and FAILURE is defined as -1
	 */
	return (s8)iError;
}

/*	Brief : The delay routine
 *	\param : delay in ms
*/
void  bmp280_delay_msek(u32 msek)
{
	/*Here you can write your own delay routine*/
}




s8 bmp280_assign_SPI(struct bmp280_t *bmp280) {
/*--------------------------------------------------------------------------*
 *  By using bmp280 the following structure parameter can be accessed
 *	Bus write function pointer: BMP280_WR_FUNC_PTR
 *	Bus read function pointer: BMP280_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *--------------------------------------------------------------------------*/

	bmp280->bus_write = bmp280_SPI_bus_write;
	bmp280->bus_read = bmp280_SPI_bus_read;
	bmp280->delay_msec = bmp280_delay_msek;

	return BMP280_INIT_VALUE;
}




