#ifndef _MCU_SPI_H_
#define _MCU_SPI_H_
#include "stm32f10x.h"

#define EPD_W21_SPI_SPEED 0x02

#define CSB_M1   	  PB8
#define CSB_S1   	  PD3
#define CSB_M2   	  PD4
#define CSB_S2   	  PB9

#define BUSY_M1   	  PB6
#define BUSY_M2   	  PB7
#define BUSY_S1   	  PD1
#define BUSY_S2   	  PD2

#define RST_M1   	  PD7
#define RST_M2   	  PB5

#define DC_M1   	  PD5
#define DC_M2   	  PD6

#define SCL   	  PE0
#define SDA   	  PE1

//CS
#define EPD_W21_CS_M1_0  (GPIO_ResetBits(GPIOB, GPIO_Pin_8)) //PB8
#define EPD_W21_CS_M1_1  (GPIO_SetBits(GPIOB, GPIO_Pin_8))

#define EPD_W21_CS_S1_0  (GPIO_ResetBits(GPIOD, GPIO_Pin_3))//PD3
#define EPD_W21_CS_S1_1  (GPIO_SetBits(GPIOD, GPIO_Pin_3))

#define EPD_W21_CS_M2_0  (GPIO_ResetBits(GPIOD, GPIO_Pin_4))//PD4
#define EPD_W21_CS_M2_1  (GPIO_SetBits(GPIOD, GPIO_Pin_4))

#define EPD_W21_CS_S2_0  (GPIO_ResetBits(GPIOB, GPIO_Pin_9))//PB9
#define EPD_W21_CS_S2_1  (GPIO_SetBits(GPIOB, GPIO_Pin_9))

//RST
#define EPD_W21_RST_M1_0  (GPIO_ResetBits(GPIOD, GPIO_Pin_7))//PD7
#define EPD_W21_RST_M1_1  (GPIO_SetBits(GPIOD, GPIO_Pin_7))

#define EPD_W21_RST_M2_0  (GPIO_ResetBits(GPIOB, GPIO_Pin_5)) //PB5
#define EPD_W21_RST_M2_1  (GPIO_SetBits(GPIOB, GPIO_Pin_5))
//DC
#define EPD_W21_DC_M1_0  (GPIO_ResetBits(GPIOD, GPIO_Pin_5))//PD5
#define EPD_W21_DC_M1_1  (GPIO_SetBits(GPIOD, GPIO_Pin_5))

#define EPD_W21_DC_M2_0  (GPIO_ResetBits(GPIOD, GPIO_Pin_6))//PD6
#define EPD_W21_DC_M2_1  (GPIO_SetBits(GPIOD, GPIO_Pin_6))
//SCL
#define EPD_W21_SCL_0   (GPIO_ResetBits(GPIOE, GPIO_Pin_0))//PE0
#define EPD_W21_SCL_1   (GPIO_SetBits(GPIOE, GPIO_Pin_0))
//SDA
#define EPD_W21_SDA_0   (GPIO_ResetBits(GPIOE, GPIO_Pin_1))//PE1
#define EPD_W21_SDA_1   (GPIO_SetBits(GPIOE, GPIO_Pin_1))

//read SDA
#define EPD_W21_ReadDATA_SDA (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)) //PE1

//BUSY
#define EPD_W21_ReadDATA_M1 (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)) //PB6
#define EPD_W21_ReadDATA_M2 (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)) //PB7
#define EPD_W21_ReadDATA_S1 (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1)) //PD1
#define EPD_W21_ReadDATA_S2 (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)) //PD2



void GPIO_1248_Configuration(void);
void GPIO_IO(unsigned char i); 


void SPI_Write(unsigned char value);
void EPD_W21_WriteCMD_M1(unsigned char command);
void EPD_W21_WriteCMD_S1(unsigned char command);
void EPD_W21_WriteCMD_M2(unsigned char command);
void EPD_W21_WriteCMD_S2(unsigned char command);
void EPD_W21_WriteDATA_M1(unsigned char command);
void EPD_W21_WriteDATA_S1(unsigned char command);
void EPD_W21_WriteDATA_M2(unsigned char command);
void EPD_W21_WriteDATA_S2(unsigned char command);
void EPD_W21_WriteCMD_ALL(unsigned char command);
void EPD_W21_WriteDATA_ALL(unsigned char command);
void EPD_W21_WriteCMD_M1M2(unsigned char command);
void EPD_W21_WriteDATA_M1M2(unsigned char command);
//EPD init
void EPD_W21_Init(void);
//EPD Read
unsigned char EPD_W21_ReadDATA_M1_temp(void);



#endif  //#ifndef _MCU_SPI_H_

/***********************************************************
						end file
***********************************************************/
