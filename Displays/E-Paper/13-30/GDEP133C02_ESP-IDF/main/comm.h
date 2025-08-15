

#ifndef __COMM_H__
#define __COMM_H__
//========== ESP32-S3-WROOM-1-N16R8 Parameters =======================
#include "esp_system.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//====================================================================
#endif //#ifndef __COMM_H__

#ifdef __COMM_C__
#define __COMM_EXTERN__
#else
#define __COMM_EXTERN__ extern
#endif

__COMM_EXTERN__ esp_err_t initialSpi(void);
__COMM_EXTERN__ void initialGpio(void);
__COMM_EXTERN__ void delayms(unsigned int delayTime);
__COMM_EXTERN__ esp_err_t spiTransmitCommand(unsigned char commandBuf);
__COMM_EXTERN__ esp_err_t spiTransmitData(unsigned char *dataBuffer, unsigned long dataLength);
__COMM_EXTERN__ esp_err_t spiReceiveData(unsigned char *dataBuffer, unsigned long dataLength);
__COMM_EXTERN__ esp_err_t spiTransmitLargeData(unsigned char commandBuf, unsigned char *dataBuffer, unsigned long dataLength);
__COMM_EXTERN__ esp_err_t spiTransmit(unsigned char commandBuf, unsigned char *dataBuffer, unsigned int dataLength);
__COMM_EXTERN__ esp_err_t spiReceive(unsigned char commandBuf, unsigned char *dataBuffer, unsigned int dataLength);
__COMM_EXTERN__ void setGpioLevel(unsigned char pinNumber, unsigned char voltageLevel);
__COMM_EXTERN__ unsigned char getGpioLevel(unsigned char pinNumber);

