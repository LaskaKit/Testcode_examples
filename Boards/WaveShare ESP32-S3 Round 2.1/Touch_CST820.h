// Kapacitni dotyk CST820 (I2C 0x15, reset pres expander EXIO2).
// Zjednodusena verze ovladace z projektu CooLajz/waveshare-hodiny (MIT) -
// tento firmware vystaci s jednim dotykovym bodem, gesta neresi.
#pragma once

#include <Arduino.h>

#include "I2C_Driver.h"
#include "TCA9554PWR.h"

#define CST820_ADDR 0x15
#define CST820_INT_PIN 16

#define CST820_REG_GESTURE_ID 0x01
#define CST820_REG_VERSION 0x15
#define CST820_REG_CHIP_ID 0xA7
#define CST820_REG_DIS_AUTO_SLEEP 0xFE

struct TouchPoint {
  bool pressed = false;
  uint16_t x = 0;
  uint16_t y = 0;
};

bool Touch_Init();
// Posledni vzorek z pozadoveho tasku; false = cteni po I2C selhalo.
bool Touch_ReadPoint(TouchPoint &point);
