#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"

//board settings
//#define ESPink_V2     //for version v2.6 and earlier
#define ESPink_V3     //for version v3.0 and above

#ifdef ESPink_V2
  #define MISO  -1
  #define MOSI  23
  #define SCK   18
  #define CS    5
  #define DC    17 
  #define RST   16  
  #define BUSY  4 
  #define POWER 2
#else ESPink_V3
  #define MISO  -1
  #define MOSI  11
  #define SCK   12
  #define CS    10
  #define DC    48 
  #define RST   45  
  #define BUSY  38 
  #define POWER 47
#endif

#define isEPD_W21_BUSY digitalRead(BUSY)
#define EPD_W21_RST_0 digitalWrite(RST,LOW)
#define EPD_W21_RST_1 digitalWrite(RST,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC,LOW)
#define EPD_W21_DC_1  digitalWrite(DC,HIGH)
#define EPD_W21_CS_0 digitalWrite(CS,LOW)
#define EPD_W21_CS_1 digitalWrite(CS,HIGH)

void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);

#endif 