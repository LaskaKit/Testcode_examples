/* Display test for Good Display GDEH0169E02
 * example from Good Display is used
 * 
 * Board:   Good Display ESP32-L
 * Display: Good Display GDEH0169E02
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>
#include <SPI.h>
#include "image.h"

// ====================== 开发板选择 ======================
// 选择你的开发板: 1 = ESP32S3, 0 = ESP32
#define BOARD_ESP32S3 0

// Panel: 1.69" e-paper, 400x400, 4bpp packed (2 pixels per byte), driver IC: JD7601/IST7601
static const uint16_t EPD_WIDTH = 400;
static const uint16_t EPD_HEIGHT = 400;
static const uint32_t EPD_FRAME_BYTES = (EPD_WIDTH * EPD_HEIGHT) / 2; // 400*400/2 = 80000

// Pin assignment - based on board selection (identical to GDEH0154E01 project)
static int PIN_EPD_MOSI;
static int PIN_EPD_CLK;
static int PIN_EPD_BUSY;
static int PIN_EPD_DC;
static int PIN_EPD_CS;
static int PIN_EPD_RST;

void initPins() {
#if BOARD_ESP32S3
    // ESP32-S3 pin definition
    PIN_EPD_MOSI = 11;
    PIN_EPD_CLK = 12;
    PIN_EPD_BUSY = 5;
    PIN_EPD_DC = 3;
    PIN_EPD_CS = 2;
    PIN_EPD_RST = 4;
    Serial.println("[BOARD] Using ESP32-S3");
#else
    // ESP32 pin definition (using A14, A15, A16, A17)
    PIN_EPD_BUSY = A14;  // 14
    PIN_EPD_RST = A15;   // 15
    PIN_EPD_DC = A16;    // 16
    PIN_EPD_CS = A17;    // 17
    PIN_EPD_CLK = 18;    // SCK (SPI default)
    PIN_EPD_MOSI = 23;   // MOSI (SPI default)
    Serial.println("[BOARD] Using ESP32");
#endif
}

// Enable to also run the two diagnostic reads from the original MSP430 code
// (read_temperture / read_Revision). These need the MOSI/SDA line to be
// bidirectional (bit-banged), since the reference hardware used a single
// SDA pin for both write and read. Hardware SPI (used for the normal write
// path below) cannot do this, so these two functions temporarily bit-bang
// the MOSI pin directly. Leave disabled unless you specifically need them.
#define ENABLE_DIAG_READ 0

// Most EPDs use BUSY low-active (LOW=busy, HIGH=idle) - matches READBUSY() in reference.
static bool gBusyActiveLevelLow = true;
static bool gIgnoreBusy = false;

static const uint32_t BUSY_TIMEOUT_INIT_MS = 15000;
static const uint32_t BUSY_TIMEOUT_POWER_MS = 60000;
static const uint32_t BUSY_TIMEOUT_REFRESH_MS = 120000;

// Color byte values are copied 1:1 from the original MSP430 reference code
// (PIC_BLACK / PIC_WHITE / ... defines in 1D69_E6_400x400_7601_test.c).
enum EpdColorByte : uint8_t {
    COLOR_BLACK = 0x00,
    COLOR_WHITE = 0x11,
    COLOR_YELLOW = 0x22,
    COLOR_RED = 0x33,
    COLOR_BLUE = 0x55,
    COLOR_GREEN = 0x66,
};

// Lower SPI clock improves signal integrity during bring-up.
static SPISettings epdSpiSettings(2000000, MSBFIRST, SPI_MODE0);

// ====================== 函数前向声明 ======================
void epdDisplaySolid(EpdColorByte color);
void epdDisplayImage(const unsigned char* imgData, uint32_t dataLen);
void epdEnterDeepSleep();
void epdReset();
void epdInitIST7601();
bool epdWaitBusy(uint32_t timeoutMs);
bool epdWaitBusyStage(const char* stage, uint32_t timeoutMs);
void epdWriteCommand(uint8_t cmd);
void epdWriteData(uint8_t data);

static inline void epdSelect() {
    digitalWrite(PIN_EPD_CS, LOW);
}

static inline void epdDeselect() {
    digitalWrite(PIN_EPD_CS, HIGH);
}

void epdWriteCommand(uint8_t cmd) {
    SPI.beginTransaction(epdSpiSettings);
    epdSelect();
    digitalWrite(PIN_EPD_DC, LOW);
    SPI.transfer(cmd);
    epdDeselect();
    SPI.endTransaction();
}

void epdWriteData(uint8_t data) {
    SPI.beginTransaction(epdSpiSettings);
    epdSelect();
    digitalWrite(PIN_EPD_DC, HIGH);
    SPI.transfer(data);
    epdDeselect();
    SPI.endTransaction();
}

#if ENABLE_DIAG_READ
// Bit-banged read on the MOSI/SDA pin, mirroring SPI4W_READDATA() in the
// reference code (P1DIR toggled to switch the pin between output/input).
uint8_t epdBitbangReadByte() {
    SPI.end(); // release the hardware SPI peripheral from the MOSI pin
    pinMode(PIN_EPD_MOSI, INPUT);
    pinMode(PIN_EPD_CLK, OUTPUT);

    uint8_t value = 0;
    epdSelect();
    digitalWrite(PIN_EPD_DC, HIGH);
    for (uint8_t i = 0; i < 8; ++i) {
        digitalWrite(PIN_EPD_CLK, HIGH);
        value = (value << 1) | (digitalRead(PIN_EPD_MOSI) ? 0x01 : 0x00);
        digitalWrite(PIN_EPD_CLK, LOW);
    }
    epdDeselect();

    pinMode(PIN_EPD_MOSI, OUTPUT);
#if BOARD_ESP32S3
    SPI.begin(PIN_EPD_CLK, -1, PIN_EPD_MOSI, PIN_EPD_CS);
#else
    SPI.begin(PIN_EPD_CLK, -1, PIN_EPD_MOSI, PIN_EPD_CS);
#endif
    return value;
}

// read_temperture() in reference: writes 0x40, waits ~1s, reads 2 bytes.
void epdReadTemperature() {
    epdWriteCommand(0x40);
    delay(1000);
    uint8_t t1 = epdBitbangReadByte();
    uint8_t t2 = epdBitbangReadByte();
    Serial.printf("[EPD] temperature raw bytes: 0x%02X 0x%02X\n", t1, t2);
}

// read_Revision() in reference: writes 0x70, reads chip id + 3 LUT version bytes.
void epdReadRevision() {
    epdWriteCommand(0x70);
    uint8_t chipId = epdBitbangReadByte();
    uint8_t lut0 = epdBitbangReadByte();
    uint8_t lut1 = epdBitbangReadByte();
    uint8_t lut2 = epdBitbangReadByte();
    Serial.printf("[EPD] chip id=0x%02X lut_ver=0x%02X%02X%02X\n", chipId, lut2, lut1, lut0);
}
#endif // ENABLE_DIAG_READ

bool epdWaitBusy(uint32_t timeoutMs = 15000) {
    if (gIgnoreBusy) {
        return true;
    }

    uint32_t start = millis();
    const int busyLevel = gBusyActiveLevelLow ? LOW : HIGH;
    while (digitalRead(PIN_EPD_BUSY) == busyLevel) {
        delay(1);
        if (millis() - start > timeoutMs) {
            Serial.printf("[EPD] wait busy timeout, pin=%d, activeLow=%d\n",
                          digitalRead(PIN_EPD_BUSY), gBusyActiveLevelLow ? 1 : 0);
            return false;
        }
    }
    return true;
}

bool epdWaitBusyStage(const char* stage, uint32_t timeoutMs) {
    bool ok = epdWaitBusy(timeoutMs);
    if (!ok) {
        Serial.printf("[EPD] stage timeout: %s\n", stage);
    }
    return ok;
}

// RESET() in reference does two low/high pulses before waiting BUSY - kept
// identical here since it's part of the vendor-verified bring-up sequence.
void epdReset() {
    digitalWrite(PIN_EPD_RST, LOW);
    delay(20);
    digitalWrite(PIN_EPD_RST, HIGH);
    delay(10);
    digitalWrite(PIN_EPD_RST, LOW);
    delay(20);
    digitalWrite(PIN_EPD_RST, HIGH);
    delay(10);
    epdWaitBusy(BUSY_TIMEOUT_INIT_MS);
    delay(10);
}

// INIT_IST7601() from reference:
//   enter test cmd (0xFF/0xA5) -> 0xD6/0x00 -> exit test cmd (0xFF/0xE3) -> 0xE9/0x01
void epdInitIST7601() {
    epdWriteCommand(0xFF);   // Enter Test command
    epdWriteData(0xA5);

    epdWriteCommand(0xD6);
    epdWriteData(0x00);

    epdWriteCommand(0xFF);   // Exit Test command
    epdWriteData(0xE3);

    epdWriteCommand(0xE9);
    epdWriteData(0x01);
}

void epdEnterDeepSleep() {
    epdWriteCommand(0x07);  // Deep sleep
    epdWriteData(0xA5);
}

// dis_img(num) in reference only ever wrote a single solid color across the
// whole frame. Kept as-is for parity; use epdDisplayImage() below to push a
// real image buffer instead.
void epdDisplaySolid(EpdColorByte color) {
    Serial.println("[EPD] stage: write frame (solid)");
    epdWriteCommand(0x10);  // DTM1 Write
    epdWaitBusyStage("DTM1", BUSY_TIMEOUT_INIT_MS);

    for (uint32_t i = 0; i < EPD_FRAME_BYTES; ++i) {
        epdWriteData(static_cast<uint8_t>(color));
    }

    Serial.println("[EPD] stage: power on");
    epdWriteCommand(0x04);  // Power ON
    epdWaitBusyStage("Power ON", BUSY_TIMEOUT_POWER_MS);
    delay(10);

    Serial.println("[EPD] stage: refresh");
    epdWriteCommand(0x12);  // Display Refresh
    epdWriteData(0x00);
    delay(10);
    epdWaitBusyStage("Display Refresh", BUSY_TIMEOUT_REFRESH_MS);

    Serial.println("[EPD] stage: power off");
    epdWriteCommand(0x02);  // Power OFF
    epdWriteData(0x00);
    epdWaitBusyStage("Power OFF", BUSY_TIMEOUT_POWER_MS);
    delay(20);
}

void epdDisplayImage(const unsigned char* imgData, uint32_t dataLen) {
    Serial.println("[EPD] stage: write frame (image)");
    epdWriteCommand(0x10);  // DTM1 Write
    epdWaitBusyStage("DTM1", BUSY_TIMEOUT_INIT_MS);

    if (dataLen != EPD_FRAME_BYTES) {
        Serial.printf("[EPD] warning: image length mismatch: dataLen=%u expected=%u\n", dataLen, (unsigned)EPD_FRAME_BYTES);
    }

    for (uint32_t i = 0; i < EPD_FRAME_BYTES; ++i) {
        uint8_t b = 0x11; // default: white
        if (i < dataLen) {
            b = pgm_read_byte(&imgData[i]);
        }
        epdWriteData(b);
    }

    Serial.println("[EPD] stage: power on");
    epdWriteCommand(0x04);
    epdWaitBusyStage("Power ON", BUSY_TIMEOUT_POWER_MS);
    delay(10);

    Serial.println("[EPD] stage: refresh");
    epdWriteCommand(0x12);
    epdWriteData(0x00);
    delay(10);
    epdWaitBusyStage("Display Refresh", BUSY_TIMEOUT_REFRESH_MS);

    Serial.println("[EPD] stage: power off");
    epdWriteCommand(0x02);
    epdWriteData(0x00);
    epdWaitBusyStage("Power OFF", BUSY_TIMEOUT_POWER_MS);
    delay(20);
}

// ====================== 显示函数结束 ======================

void setup() {
    Serial.begin(115200);
    delay(300);

    // 初始化引脚
    initPins();

    pinMode(PIN_EPD_BUSY, INPUT);
    pinMode(PIN_EPD_DC, OUTPUT);
    pinMode(PIN_EPD_CS, OUTPUT);
    pinMode(PIN_EPD_RST, OUTPUT);

    digitalWrite(PIN_EPD_CS, HIGH);
    digitalWrite(PIN_EPD_DC, HIGH);
    digitalWrite(PIN_EPD_RST, HIGH);

    // Hardware SPI 初始化
    SPI.begin(PIN_EPD_CLK, -1, PIN_EPD_MOSI, PIN_EPD_CS);

    Serial.printf("[EPD] boot, BUSY pin now=%d\n", digitalRead(PIN_EPD_BUSY));

    // 复位 (与参考代码 RESET() 一致：两次低/高脉冲)
    epdReset();

#if ENABLE_DIAG_READ
    epdReadTemperature();
    epdReset();
    epdReadRevision();
#endif

    epdReset();
    epdWaitBusy(BUSY_TIMEOUT_INIT_MS);

    // 初始化 IST7601 (与参考代码 INIT_IST7601() 一致)
    epdInitIST7601();
    Serial.println("[EPD] init done");

    // 显示图片 (来自 image.h，占位数据；替换为真实图片数据即可)
    Serial.println("[EPD] 显示图片...");
    epdDisplayImage(image_data_sixcolor, sizeof(image_data_sixcolor));
    delay(3000);

    // 参考代码 main() 中最终显示的是纯色 PIC_GREEN，如需复现可改用:
    // epdDisplaySolid(COLOR_GREEN);

    // 进入深睡眠
    epdEnterDeepSleep();
}

void loop() {
    delay(1000);
}
