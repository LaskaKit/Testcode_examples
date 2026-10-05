#ifndef __IT8957_IO_H__
#define __IT8957_IO_H__

#include <Arduino.h>
#include <SPI.h>

//typedef for variables
typedef uint8_t TByte; //1 byte
typedef uint16_t TWord; //2 bytes
typedef uint32_t TDWord; //4 bytes

#define VSPI_MISO       16
#define VSPI_MOSI       6
#define VSPI_SCLK       15
#define IT8957_SPI_CS		7

#define IT8957_RESET_PIN		  18
#define IT8957_HOST_HRDY_PIN	5
#define IT8957_WAKE_UP_PIN    17

#define TCON_POWER_PIN 46  //3 for V2.0

//Power & Reset & Wakeup Control
extern void IT8957_IO_Initialize(void);
extern void IT8957_IO_Deinitialize(void);
extern void IT8957_IO_Reset(void);
extern void IT8957_IO_ResetHold(void);
extern void Tcon_PowerCtrl(int on);

//Needed by T2001 driver
#define HRDY_GPIO_STATE  digitalRead(IT8957_HOST_HRDY_PIN)
extern void SPIWrite(TByte* pWBuf, TDWord ulSizeByte,  TByte CS);
extern void SPIRead (TByte* pRBuf, TDWord ulSizeByte,  TByte CS);
extern void SPIWrite(TWord* pWBuf, TDWord ulSizeWord,  TByte CS);
extern void SPIRead (TWord* pRBuf, TDWord ulSizeWord,  TByte CS);
#endif
