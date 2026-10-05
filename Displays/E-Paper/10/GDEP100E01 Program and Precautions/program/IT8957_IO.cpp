/*
 * IT8951 IO部分的配置。
 * 与MCU选型及对应的SDK相关。
 * 移植到其他平台，修改这个文件即可。
 * 提供I80和SPI两种接口方式。
*/
#include "IT8957_IO.h"

SPIClass epd_spi;                    //default using HSPI, not VSPI or FSPI
static const int spiClk = 12000000;  // 12 MHz

void IT8957_IO_Initialize(void) {
	//IO初始化
	pinMode(TCON_POWER_PIN, OUTPUT);
	pinMode(IT8957_RESET_PIN, OUTPUT);
	pinMode(IT8957_SPI_CS, OUTPUT);
	pinMode(IT8957_HOST_HRDY_PIN, INPUT_PULLUP);
	// pinMode(IT8957_WAKE_UP_PIN, OUTPUT);	//暂时不管WAKEUP

	digitalWrite(TCON_POWER_PIN, LOW);
	digitalWrite(IT8957_SPI_CS, LOW);
	digitalWrite(IT8957_RESET_PIN, LOW);

	// digitalWrite(IT8957_WAKE_UP_PIN, HIGH);

	//IO初始化-SPI
	epd_spi.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, -1);  //SCLK, MISO, MOSI, SS
	epd_spi.setHwCs(false);
	epd_spi.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
}

void IT8957_IO_Deinitialize(void) {
	epd_spi.endTransaction();
	epd_spi.end();
}

void IT8957_IO_Reset(void) {
	digitalWrite(IT8957_RESET_PIN, HIGH);
	delay(20);
	digitalWrite(IT8957_RESET_PIN, LOW);  //module reset
	delay(30);
	digitalWrite(IT8957_RESET_PIN, HIGH);
	delay(30);
	delay(2000);  //Wait T2001 booting complete
}

//for FW Initial Burning...
void IT8957_IO_ResetHold(void){
	digitalWrite(IT8957_RESET_PIN, LOW);  //module reset
}


void Tcon_PowerCtrl(int on) {
	if (on) {
		digitalWrite(TCON_POWER_PIN, HIGH);
		digitalWrite(IT8957_SPI_CS, HIGH);
		digitalWrite(IT8957_RESET_PIN, HIGH);
		// delay(2000);
		Serial.println("T2001 Booting...");
	} else {
		digitalWrite(TCON_POWER_PIN, LOW);
		digitalWrite(IT8957_SPI_CS, LOW);
		digitalWrite(IT8957_RESET_PIN, LOW);
	}
}


//   Assumed the SPI API as following:
//
//   void SPIWrite(TByte* pWBuf, TDWord ulSizeByte,  TByte CS)
//   void SPIRead (TByte* pRBuf, TDWord ulSizeByte,  TByte CS)
//
//   	CS = L : the CS will keep L when this spi transfder terminated
//    CS = H : the CS will change from L to H when this spi transfder terminated
//
//   The SPI API function should be check by your host spi controller

void SPIWrite(TByte* pWBuf, TDWord ulSizeByte, TByte CS) {
	digitalWrite(IT8957_SPI_CS, LOW);
	// epd_spi.writeBytes((const uint8_t *) pWBuf, ulSizeByte);
	epd_spi.transferBytes(pWBuf, NULL, ulSizeByte);
	if (CS)
		digitalWrite(IT8957_SPI_CS, HIGH);
}

void SPIRead(TByte* pRBuf, TDWord ulSizeByte, TByte CS) {
	digitalWrite(IT8957_SPI_CS, LOW);
	for (int i = 0; i < ulSizeByte; i++) {
		*pRBuf++ = epd_spi.transfer(0xFF);
	}
	if (CS)
		digitalWrite(IT8957_SPI_CS, HIGH);
}

void SPIWrite(TWord* pWBuf, TDWord ulSizeWord, TByte CS) {
	digitalWrite(IT8957_SPI_CS, LOW);
	for (int i = 0; i < ulSizeWord; i++) {
		epd_spi.transfer16(*pWBuf++);
	}

	if (CS)
		digitalWrite(IT8957_SPI_CS, HIGH);
}

void SPIRead(TWord* pRBuf, TDWord ulSizeWord, TByte CS) {
	digitalWrite(IT8957_SPI_CS, LOW);
	for (int i = 0; i < ulSizeWord; i++) {
		*pRBuf++ = epd_spi.transfer16(0xFFFF);
	}
	if (CS)
		digitalWrite(IT8957_SPI_CS, HIGH);
}
