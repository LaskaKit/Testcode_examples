#include "Touch_CST820.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

namespace {
SemaphoreHandle_t sampleMutex() {
  static StaticSemaphore_t storage;
  static SemaphoreHandle_t mutex = xSemaphoreCreateMutexStatic(&storage);
  return mutex;
}

TaskHandle_t sampler = nullptr;
TouchPoint latest;
bool sampleValid = false;

bool readRegisters(uint8_t reg, uint8_t *buffer, uint8_t length) {
  I2CBusGuard guard;
  Wire.beginTransmission(CST820_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(true) != 0) return false;
  if (Wire.requestFrom(static_cast<uint8_t>(CST820_ADDR), length) != length) {
    while (Wire.available()) Wire.read();
    return false;
  }
  for (uint8_t i = 0; i < length; ++i) {
    if (!Wire.available()) return false;
    buffer[i] = Wire.read();
  }
  return true;
}

bool writeRegister(uint8_t reg, uint8_t value) {
  I2CBusGuard guard;
  Wire.beginTransmission(CST820_ADDR);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission(true) == 0;
}

void touchReset() {
  Set_EXIO(EXIO_PIN2, Low);
  vTaskDelay(pdMS_TO_TICKS(10));
  Set_EXIO(EXIO_PIN2, High);
  vTaskDelay(pdMS_TO_TICKS(50));
}

void sampleTouch(void *) {
  for (;;) {
    uint8_t buffer[6] = {};
    const bool ok = readRegisters(CST820_REG_GESTURE_ID, buffer, sizeof(buffer));
    xSemaphoreTake(sampleMutex(), portMAX_DELAY);
    sampleValid = ok;
    if (ok) {
      latest.pressed = buffer[1] != 0;
      // Paket pri uvolneni muze souradnice vynechat, posledni pozici drzime.
      if (latest.pressed) {
        latest.x = ((buffer[2] & 0x0F) << 8) | buffer[3];
        latest.y = ((buffer[4] & 0x0F) << 8) | buffer[5];
      }
    } else {
      // Chyba na I2C neni uvolneni dotyku, ale nesmi ho ani predstirat.
      latest.pressed = false;
    }
    xSemaphoreGive(sampleMutex());
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
}  // namespace

bool Touch_Init() {
  pinMode(CST820_INT_PIN, INPUT_PULLUP);
  touchReset();

  // Vypnuti automatickeho uspani radice, jinak po chvili prestane hlasit dotyky.
  const uint8_t disableAutoSleep = 0xFF;
  writeRegister(CST820_REG_DIS_AUTO_SLEEP, disableAutoSleep);

  uint8_t identification[3] = {};
  if (readRegisters(CST820_REG_CHIP_ID, identification, sizeof(identification))) {
    Serial.printf("CST820 ChipID:0x%02X ProjID:0x%02X FW:0x%02X\n",
                  identification[0], identification[1], identification[2]);
  }

  if (sampler) return true;
  return xTaskCreate(sampleTouch, "touch-sampler", 3072, nullptr, 2, &sampler) ==
         pdPASS;
}

bool Touch_ReadPoint(TouchPoint &point) {
  xSemaphoreTake(sampleMutex(), portMAX_DELAY);
  point = latest;
  const bool valid = sampleValid;
  xSemaphoreGive(sampleMutex());
  return valid;
}
