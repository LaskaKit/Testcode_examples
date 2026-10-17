/* Display test for Good Display GDEH0154E01
 * example from Good Display is used
 * 
 * Board:   Good Display ESP32-L
 * Display: Good Display GDEH0154E01
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>
#include <SPI.h>
#include "image.h"

// ====================== 开发板选择 ======================
// 选择你的开发板: 1 = ESP32S3, 0 = ESP32
#define BOARD_ESP32S3 0   // GoodDisplay ESP32-L = klasicky ESP32

// EPD resolution from original code: 480x720, 4bpp packed (2 pixels per byte).
// Panel is GDEH0154E01: 240x240, 4bpp packed (2 pixels per byte)
static const uint16_t EPD_WIDTH = 240;
static const uint16_t EPD_HEIGHT = 240;
static const uint32_t EPD_FRAME_BYTES = (EPD_WIDTH * EPD_HEIGHT) / 2; // 240*240/2 = 28800

// Pin assignment - based on board selection
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

// Set false to run normal panel flow and draw color bars.
static const bool SAFE_DIAG_MODE = false;

// Most EPDs use BUSY low-active (LOW=busy, HIGH=idle). Some panels are opposite.
static bool gBusyActiveLevelLow = true;
static bool gIgnoreBusy = false;

// false: normal init, true: apply factory water-ripple init sequence.
static const bool USE_WATER_RIPPLE_INIT = false;

enum WaterRippleProfile : uint8_t {
    WATER_RIPPLE_FACTORY = 0,
    WATER_RIPPLE_NEW = 1,
};

// Select which water-ripple initialization profile to use.
static const WaterRippleProfile ACTIVE_WATER_RIPPLE_PROFILE = WATER_RIPPLE_NEW;

static const uint32_t BUSY_TIMEOUT_INIT_MS = 15000;
static const uint32_t BUSY_TIMEOUT_POWER_MS = 60000;
static const uint32_t BUSY_TIMEOUT_REFRESH_MS = 120000;

// Color byte values are copied from the original MSP430 reference code.
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
void epdTryFixBusyPolarity();
void epdInitJD7601();
void epdInitJD7601WaterRipple_v2();
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

void diagPrintBusyWithPullModes() {
    pinMode(PIN_EPD_BUSY, INPUT);
    delay(3);
    int vInput = digitalRead(PIN_EPD_BUSY);

    pinMode(PIN_EPD_BUSY, INPUT_PULLUP);
    delay(3);
    int vPullup = digitalRead(PIN_EPD_BUSY);

    pinMode(PIN_EPD_BUSY, INPUT_PULLDOWN);
    delay(3);
    int vPulldown = digitalRead(PIN_EPD_BUSY);

    Serial.printf("[EPD] BUSY sample: IN=%d PU=%d PD=%d\n", vInput, vPullup, vPulldown);
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

void epdTryFixBusyPolarity() {
    if (epdWaitBusy(3000)) {
        Serial.printf("[EPD] busy polarity ok, activeLow=%d\n", gBusyActiveLevelLow ? 1 : 0);
        return;
    }

    gBusyActiveLevelLow = !gBusyActiveLevelLow;
    if (epdWaitBusy(3000)) {
        Serial.printf("[EPD] busy polarity switched, activeLow=%d\n", gBusyActiveLevelLow ? 1 : 0);
    } else {
        Serial.println("[EPD] busy pin still not ready, check BUSY wire and panel power");
        Serial.println("[EPD] fallback: ignore BUSY and continue");
        gIgnoreBusy = true;
    }
}

void epdReset() {
    digitalWrite(PIN_EPD_RST, LOW);
    delay(20);
    digitalWrite(PIN_EPD_RST, HIGH);
    delay(20);
    epdWaitBusy(BUSY_TIMEOUT_INIT_MS);
    delay(10);
}

void epdInitJD7601() {
    epdWriteCommand(0xE9);
    epdWriteData(0x01);
}

void epdInitJD7601WaterRipple() {
    // Base init first.
    epdInitJD7601();

    // Enter test command mode.
    epdWriteCommand(0xFF);
    epdWriteData(0xA5);

    epdWriteCommand(0xEB);  // PWM AC
    epdWriteData(0x01);

    epdWriteCommand(0xDA);  // IBDC
    epdWriteData(0x04);

    epdWriteCommand(0xB4);  // Dither
    epdWriteData(ACTIVE_WATER_RIPPLE_PROFILE == WATER_RIPPLE_NEW ? 0x31 : 0x11);

    epdWriteCommand(0xEF);  // PWM
    if (ACTIVE_WATER_RIPPLE_PROFILE == WATER_RIPPLE_NEW) {
        // New profile: softer multi-stage PWM to reduce abrupt transitions.
        epdWriteData(3);   // H0
        epdWriteData(80);  // L0
        epdWriteData(6);   // H1
        epdWriteData(45);  // L1
        epdWriteData(10);  // H2
        epdWriteData(36);  // L2
        epdWriteData(14);  // H3
        epdWriteData(8);   // L3
        epdWriteData(18);  // H4
        epdWriteData(4);   // L4
    } else {
        epdWriteData(2);    // H0
        epdWriteData(100);  // L0
        epdWriteData(5);    // H1
        epdWriteData(50);   // L1
        epdWriteData(9);    // H2
        epdWriteData(60);   // L2
        epdWriteData(15);   // H3
        epdWriteData(3);    // L3
        epdWriteData(15);   // H4
        epdWriteData(5);    // L4
    }

    epdWriteCommand(0xDC);  // CPCK SET
    epdWriteData(0x01);     // CPCKEN

    epdWriteCommand(0xDD);  // CPCK
    if (ACTIVE_WATER_RIPPLE_PROFILE == WATER_RIPPLE_NEW) {
        epdWriteData(6);
        epdWriteData(18);
    } else {
        epdWriteData(5);
        epdWriteData(20);
    }

    epdWriteCommand(0xDE);  // CPCK OFT
    if (ACTIVE_WATER_RIPPLE_PROFILE == WATER_RIPPLE_NEW) {
        epdWriteData(4);
        epdWriteData(10);
        epdWriteData(16);
        epdWriteData(22);
        epdWriteData(28);
    } else {
        epdWriteData(6);
        epdWriteData(12);
        epdWriteData(18);
        epdWriteData(24);
        epdWriteData(30);
    }

    // Exit test command mode.
    epdWriteCommand(0xFF);
    epdWriteData(0xE3);
}

void epdInitJD7601WaterRipple_v2() {
    epdInitJD7601();  // 0xE9 基础

    epdWriteCommand(0xFF);
    epdWriteData(0xA5);  // 测试模式

    epdWriteCommand(0xEB);
    epdWriteData(0x01);

    epdWriteCommand(0xDA);
    epdWriteData(0x04);

    // ★ 重点试验：改这个值
    epdWriteCommand(0xB4);
    epdWriteData(0x31);  // 原来是0x31，试 0x51/0x61/0x71

    // ★ 新增：波形模式
    epdWriteCommand(0xB0);
    epdWriteData(0x03);  // 逐个试 0x00~0x04

    // ★ 新增：扩散中心（针对240x240，中心在0x78行）
    epdWriteCommand(0xC0);
    epdWriteData(0x78);

    epdWriteCommand(0xEF);
    epdWriteData(3);
    epdWriteData(80);
    epdWriteData(6);
    epdWriteData(45);
    epdWriteData(10);
    epdWriteData(36);
    epdWriteData(14);
    epdWriteData(8);
    epdWriteData(18);
    epdWriteData(4);

    epdWriteCommand(0xDC);
    epdWriteData(0x01);

    epdWriteCommand(0xDD);
    epdWriteData(6);
    epdWriteData(18);

    epdWriteCommand(0xDE);
    epdWriteData(4);
    epdWriteData(10);
    epdWriteData(16);
    epdWriteData(22);
    epdWriteData(28);

    epdWriteCommand(0xFF);
    epdWriteData(0xE3);  // 退出测试模式
}




void epdEnterDeepSleep() {
    epdWriteCommand(0x07);
    epdWriteData(0xA5);
}

// ====================== 自定义波形加载函数 ======================
// 已移除 - waveform.h 不再使用



void epdDisplaySolid(EpdColorByte color) {
    Serial.println("[EPD] stage: write frame");
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

void epdDisplayColorBars() {
    static const EpdColorByte bars[6] = {
        COLOR_RED,
        COLOR_YELLOW,
        COLOR_BLUE,
        COLOR_BLACK,
        COLOR_WHITE,
        COLOR_GREEN,
    };

    const uint16_t bytesPerLine = EPD_WIDTH / 2;
    const uint16_t bandHeight = EPD_HEIGHT / 6; // still valid for 6 color bars

    Serial.println("[EPD] stage: write frame (color bars)");
    epdWriteCommand(0x10);  // DTM1 Write
    epdWaitBusyStage("DTM1", BUSY_TIMEOUT_INIT_MS);

    for (uint16_t y = 0; y < EPD_HEIGHT; ++y) {
        uint8_t band = y / bandHeight;
        if (band > 5) {
            band = 5;
        }
        uint8_t px = static_cast<uint8_t>(bars[band]);
        for (uint16_t x = 0; x < bytesPerLine; ++x) {
            epdWriteData(px);
        }
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

    // Ensure we write exactly the expected frame size. If image data length
    // doesn't match EPD_FRAME_BYTES, truncate or pad with white.
    if (dataLen != EPD_FRAME_BYTES) {
        Serial.printf("[EPD] warning: image length mismatch: dataLen=%u expected=%u\n", dataLen, (unsigned)EPD_FRAME_BYTES);
    }

    for (uint32_t i = 0; i < EPD_FRAME_BYTES; ++i) {
        uint8_t b = 0x11; // default: white
        if (i < dataLen) {
            b = pgm_read_byte(&imgData[i]);  // 从 Flash 读取
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
#if BOARD_ESP32S3
    // ESP32-S3: SCK=IO12, MISO unused(-1), MOSI=IO11, SS=IO2
    SPI.begin(PIN_EPD_CLK, -1, PIN_EPD_MOSI, PIN_EPD_CS);
#else
    // ESP32: SCK=IO18, MISO=IO19, MOSI=IO23, SS=IO17
    SPI.begin(PIN_EPD_CLK, -1, PIN_EPD_MOSI, PIN_EPD_CS);
#endif

    Serial.printf("[EPD] boot, BUSY pin now=%d\n", digitalRead(PIN_EPD_BUSY));

    // 重置并初始化
    epdReset();
    epdTryFixBusyPolarity();
    
    if (USE_WATER_RIPPLE_INIT) {
        Serial.println("[EPD] init: water-ripple mode");
        epdInitJD7601WaterRipple_v2();
    } else {
        Serial.println("[EPD] init: normal mode");
        epdInitJD7601();
    }
    Serial.println("[EPD] init done");

    // 显示图片
    Serial.println("[EPD] 显示图片...");
    epdDisplayImage(image_data_sixcolor, sizeof(image_data_sixcolor));
    delay(3000);

    // 刷白
    Serial.println("[EPD] 刷白屏幕...");
    epdDisplaySolid(COLOR_WHITE);

    // 进入深睡眠
    epdEnterDeepSleep();
}

void loop() {
    delay(1000);
}


