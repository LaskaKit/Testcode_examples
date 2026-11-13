#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"

// ---------------------------------------------------------------
// LaskaKit ESPink v3.6 (ESP32-S3) - pinout e-paper konektoru
// ---------------------------------------------------------------
#define EPD_MOSI   11   // SDI
#define EPD_SCK    12   // CLK
#define EPD_MISO   -1   // nepouzito
#define EPD_CS     10   // CS
#define EPD_DC     48   // D/C
#define EPD_RST    45   // RES
#define EPD_BUSY   38   // BUSY
#define EPD_POWER  47   // napajeni e-paperu a uSup (HIGH = zapnuto)

#define isEPD_W21_BUSY digitalRead(EPD_BUSY)          //BUSY
#define EPD_W21_RST_0  digitalWrite(EPD_RST, LOW)     //RES
#define EPD_W21_RST_1  digitalWrite(EPD_RST, HIGH)
#define EPD_W21_DC_0   digitalWrite(EPD_DC, LOW)      //DC
#define EPD_W21_DC_1   digitalWrite(EPD_DC, HIGH)
#define EPD_W21_CS_0   digitalWrite(EPD_CS, LOW)      //CS
#define EPD_W21_CS_1   digitalWrite(EPD_CS, HIGH)


void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);


#endif
