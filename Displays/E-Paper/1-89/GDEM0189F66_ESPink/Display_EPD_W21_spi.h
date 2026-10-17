#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"

// LaskaKit microESPink (ESP32-S3) pinout
// JP1 OFF, JP2 OFF (same as other 4-color GoodDisplay panels, e.g. GDEY029F51H)
#define MISO  -1
#define MOSI  11   // SDI
#define SCK   12   // SCLK
#define CS    10
#define DC    48
#define RST   45
#define BUSY  38
#define POWER 47   // ePaper + uSup + fuel gauge power, active HIGH

#define isEPD_W21_BUSY digitalRead(BUSY)
#define EPD_W21_RST_0 digitalWrite(RST,LOW)
#define EPD_W21_RST_1 digitalWrite(RST,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC,LOW)
#define EPD_W21_DC_1  digitalWrite(DC,HIGH)
#define EPD_W21_CS_0  digitalWrite(CS,LOW)
#define EPD_W21_CS_1  digitalWrite(CS,HIGH)


void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);


#endif 
