#include "Display_EPD_W21_spi.h"
#include <SPI.h>

static const unsigned char csPins[2] = {CS_M, CS_S};

void EPD_CS(unsigned char csx, unsigned char level)
{
  digitalWrite(csPins[csx], level);
}

void EPD_CS_All(unsigned char level)
{
  digitalWrite(CS_M, level);
  digitalWrite(CS_S, level);
}

void SPI_Init(void)
{
  SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, -1);   // CS is driven manually (two driver ICs)
  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
}

// The panel takes the first byte after CS goes LOW as the command,
// all following bytes until CS goes HIGH are data (no D/C pin).
void EPD_W21_WriteCMD(unsigned char command)
{
#if DC >= 0
  digitalWrite(DC, LOW);
#endif
  SPI.transfer(command);
#if DC >= 0
  digitalWrite(DC, HIGH);
#endif
}

void EPD_W21_WriteDATA(unsigned char datas)
{
  SPI.transfer(datas);
}

void EPD_W21_WriteDATA_n(const unsigned char *datas, unsigned long len)
{
  // writeBytes() sends from flash/RAM in bulk, much faster than byte by byte
  while (len) {
    unsigned long chunk = (len > 4096) ? 4096 : len;
    SPI.writeBytes((uint8_t *)datas, chunk);
    datas += chunk;
    len -= chunk;
  }
}

void EPD_W21_WriteREG(unsigned char command, const unsigned char *datas, unsigned int len)
{
  EPD_W21_WriteCMD(command);
  if (len) EPD_W21_WriteDATA_n(datas, len);
}
