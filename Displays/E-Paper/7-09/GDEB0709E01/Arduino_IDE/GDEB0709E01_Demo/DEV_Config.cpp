#include "DEV_Config.h"

static SPIClass *g_spi = nullptr;

UBYTE DEV_Module_Init(void)
{
    pinMode(EPD_BUSY_PIN, INPUT);
    pinMode(EPD_RST_PIN, OUTPUT);
    pinMode(EPD_DC_PIN, OUTPUT);
    pinMode(EPD_PWR_PIN, OUTPUT);
    pinMode(EPD_CS_M_PIN, OUTPUT);
    pinMode(EPD_CS_S_PIN, OUTPUT);

    digitalWrite(EPD_CS_M_PIN, HIGH);
    digitalWrite(EPD_CS_S_PIN, HIGH);
    digitalWrite(EPD_DC_PIN, HIGH);
    digitalWrite(EPD_PWR_PIN, HIGH);
    digitalWrite(EPD_RST_PIN, HIGH);

    Serial.begin(115200);

    // ESP32-S3 SPI2. Arduino SPI driver uses the hardware DMA path
    // for bulk transfers on supported ESP32-S3 Arduino cores.
    g_spi = new SPIClass(FSPI);
    g_spi->begin(EPD_SCK_PIN, EPD_MISO_PIN, EPD_MOSI_PIN, -1);
    g_spi->beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

    return 0;
}

void DEV_SPI_WriteByte(UBYTE data)
{
    g_spi->transfer(data);
}

UBYTE DEV_SPI_ReadByte(void)
{
    return g_spi->transfer(0x00);
}

void DEV_SPI_Write_nByte(const UBYTE *data, UDOUBLE len)
{
    // SPIClass bulk write is used so the Arduino SPI driver can use DMA.
    g_spi->writeBytes(const_cast<UBYTE *>(data), len);
}

void DEV_Module_Exit(void)
{
    if (g_spi) {
        g_spi->endTransaction();
        g_spi->end();
        delete g_spi;
        g_spi = nullptr;
    }
    digitalWrite(EPD_PWR_PIN, LOW);
}
