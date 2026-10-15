#include "Buzzer.h"

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "TCA9554PWR.h"

namespace {
constexpr uint32_t ALARM_BEEP_ON_MS = 120;
constexpr uint32_t ALARM_BEEP_OFF_MS = 380;
constexpr uint32_t TICK_MS = 10;

TaskHandle_t worker = nullptr;
portMUX_TYPE stateMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool alarmRequested = false;
volatile uint32_t clickRequestMs = 0;

bool outputOn = false;

void setOutput(bool on) {
  if (outputOn == on) return;
  // Pri chybe na I2C stav neukladame, dalsi pruchod se pokusi znovu.
  if (Set_EXIO_Checked(EXIO_PIN8, on ? High : Low)) outputOn = on;
}

void run(void *) {
  uint32_t clickRemainingMs = 0;
  uint32_t alarmPhaseMs = 0;
  bool alarmPhaseOn = false;

  for (;;) {
    uint32_t click = 0;
    portENTER_CRITICAL(&stateMux);
    click = clickRequestMs;
    clickRequestMs = 0;
    const bool alarm = alarmRequested;
    portEXIT_CRITICAL(&stateMux);

    if (click) clickRemainingMs = click;

    if (clickRemainingMs) {
      setOutput(true);
      clickRemainingMs = clickRemainingMs > TICK_MS ? clickRemainingMs - TICK_MS : 0;
      alarmPhaseMs = 0;
      alarmPhaseOn = false;
    } else if (alarm) {
      if (alarmPhaseMs == 0) {
        alarmPhaseOn = !alarmPhaseOn;
        alarmPhaseMs = alarmPhaseOn ? ALARM_BEEP_ON_MS : ALARM_BEEP_OFF_MS;
      }
      setOutput(alarmPhaseOn);
      alarmPhaseMs = alarmPhaseMs > TICK_MS ? alarmPhaseMs - TICK_MS : 0;
    } else {
      setOutput(false);
      alarmPhaseMs = 0;
      alarmPhaseOn = false;
    }

    vTaskDelay(pdMS_TO_TICKS(TICK_MS));
  }
}
}  // namespace

void buzzerBegin() {
  if (worker) return;
  // Pri startu overime ticho; vystup expanderu muze byt po resetu libovolny.
  outputOn = true;
  setOutput(false);
  xTaskCreate(run, "buzzer", 3072, nullptr, 2, &worker);
}

void buzzerClick(uint32_t milliseconds) {
  if (milliseconds > 500) milliseconds = 500;
  portENTER_CRITICAL(&stateMux);
  clickRequestMs = milliseconds;
  portEXIT_CRITICAL(&stateMux);
}

void buzzerSetAlarm(bool active) {
  portENTER_CRITICAL(&stateMux);
  alarmRequested = active;
  portEXIT_CRITICAL(&stateMux);
}

bool buzzerAlarmActive() {
  portENTER_CRITICAL(&stateMux);
  const bool active = alarmRequested;
  portEXIT_CRITICAL(&stateMux);
  return active;
}
