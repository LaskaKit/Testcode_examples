#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"


#define CS    10
#define MOSI  11
#define SCK   12  
#define DC    13        
#define CS2   39
#define BUSY  40
#define RES   41

#define isEPD_W21_BUSY digitalRead(BUSY)  //BUSY
#define EPD_W21_RST_0 digitalWrite(RES,LOW)  //RES
#define EPD_W21_RST_1 digitalWrite(RES,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC,LOW) //DC
#define EPD_W21_DC_1  digitalWrite(DC,HIGH)
#define EPD_W21_CS_0 digitalWrite(CS,LOW) //CS1
#define EPD_W21_CS_1 digitalWrite(CS,HIGH)
#define EPD_W21_CS2_0 digitalWrite(CS2,LOW) //CS2
#define EPD_W21_CS2_1 digitalWrite(CS2,HIGH)

void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);
void EPD_W21_WriteDATA1(unsigned char datas);
void EPD_W21_WriteCMD1(unsigned char command);
void EPD_W21_WriteDATA2(unsigned char datas);
void EPD_W21_WriteCMD2(unsigned char command);
#endif 
