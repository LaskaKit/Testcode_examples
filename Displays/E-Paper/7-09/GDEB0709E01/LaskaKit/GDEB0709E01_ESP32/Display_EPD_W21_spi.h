#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"

// ---------------------------------------------------------------
// Board settings - choose exactly one
// ---------------------------------------------------------------
#define BOARD_GD_ESP32_133C02     // Good Display ESP32-133C02 (ESP32-S3)
//#define BOARD_LASKAKIT_ADAPTER  // LaskaKit E6 adapter - CHECK THE PINS BELOW!

#if defined(BOARD_GD_ESP32_133C02)
  // Pins taken from Good Display pindefine.h (GDEP133C02 example 2026-04-20)
  #define EPD_MISO   40   // SPI_Data1
  #define EPD_MOSI   41   // SPI_Data0
  #define EPD_SCK    9
  #define CS_M   18   // SPI_CS0 - master driver IC (left half, 600 columns)
  #define CS_S   17   // SPI_CS1 - slave driver IC (right half, 600 columns)
  #define RST    6
  #define BUSY   7    // LOW = busy
  #define POWER  45   // LOAD_SW - panel power switch, HIGH = on
  #define DC     -1   // not used: the first byte after CS falls is the command
#elif defined(BOARD_LASKAKIT_ADAPTER)
  // ESP32-S3-WROOM-1-N16 board -> 60pin-Spectra adapter (JP1)
  #define EPD_SCK    15   // JP1-12 SPI_CLK
  #define EPD_MOSI   41   // JP1-11 SPI_MOSI0
  #define EPD_MISO   -1   // JP1-10 SPI_MOSI1 (-1 = not connected)
  #define DC         8    // JP1-7  DC/X
  #define CS_S       17   // JP1-6  CSB_S
  #define CS_M       18   // JP1-5  CSB_M
  #define BUSY       7    // JP1-4  BUSY_N
  #define RST        6    // JP1-3  RST_N
  #define POWER      47   // VSENSOR enable (IO47_USUP) - check!
#else
  #error "Select a board in Display_EPD_W21_spi.h"
#endif

#define SPI_SPEED  10000000   // 10 MHz, same as Good Display example

#define isEPD_W21_BUSY  digitalRead(BUSY)
#define EPD_W21_RST_0   digitalWrite(RST, LOW)
#define EPD_W21_RST_1   digitalWrite(RST, HIGH)

// chip select: 0 = master, 1 = slave
void EPD_CS(unsigned char csx, unsigned char level);
void EPD_CS_All(unsigned char level);

void SPI_Init(void);
void EPD_W21_WriteCMD(unsigned char command);                        // CS must already be LOW
void EPD_W21_WriteDATA(unsigned char datas);                         // CS must already be LOW
void EPD_W21_WriteDATA_n(const unsigned char *datas, unsigned long len);
void EPD_W21_WriteREG(unsigned char command, const unsigned char *datas, unsigned int len); // CMD + data, CS handled by caller

#endif
