#ifndef GDEB0709E01_DEV_CONFIG_H
#define GDEB0709E01_DEV_CONFIG_H

#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>

using UBYTE = uint8_t;
using UWORD = uint16_t;
using UDOUBLE = uint32_t;

// ESP32-S3 driver-board pin mapping.
#define EPD_SCK_PIN     9
#define EPD_MOSI_PIN    41
#define EPD_MISO_PIN    40
#define EPD_CS_M_PIN    18
#define EPD_CS_S_PIN    17
#define EPD_RST_PIN     6
#define EPD_DC_PIN      2
#define EPD_BUSY_PIN    7
#define EPD_PWR_PIN     45

#define DEV_Digital_Write(pin, value) digitalWrite((pin), ((value) != 0) ? HIGH : LOW)
#define DEV_Digital_Read(pin) digitalRead((pin))
#define DEV_Delay_ms(ms) delay(ms)

UBYTE DEV_Module_Init(void);
void DEV_Module_Exit(void);
void DEV_SPI_WriteByte(UBYTE data);
UBYTE DEV_SPI_ReadByte(void);
void DEV_SPI_Write_nByte(const UBYTE *data, UDOUBLE len);

#endif
