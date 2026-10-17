// DistanceRing - laserovy meric vzdalenosti s kruhovou stupnici
//
// Hardware: Waveshare ESP32-S3-Touch-LCD-2.1 (kruhovy 480x480 IPS, ST7701S)
//           + laserovy dalkomer VL53L1X nebo VL53L0X na I2C konektoru desky.
//           Ktery senzor je pripojeny, firmware pozna sam.
//
// Displej ukazuje vzdalenost velkym cislem, po obvodu bezi stupnice (0-4 m
// pro VL53L1X, 0-2 m pro VL53L0X) se znackou nastavene meze a tlacitky se
// nastavuje upozorneni vestavenym bzucakem.
// Kresli se vlastni vrstvou primo do framebufferu panelu, zadna graficka
// knihovna neni potreba.

// laskakit 2026

#include <Arduino.h>
#include <Wire.h>

#include "AlarmLogic.h"
#include "Buzzer.h"
#include "Display_ST7701.h"
#include "DistanceSensor.h"
#include "Gfx.h"
#include "I2C_Driver.h"
#include "Screen.h"
#include "Settings.h"
#include "TCA9554PWR.h"
#include "Touch_CST820.h"

// Diagnostika senzoru: vypis na seriovou linku a radek pod tlacitky se
// surovym stavem mereni. Pro ostry provoz staci prepnout na 0.
#define SENSOR_DIAGNOSTICS 1

namespace {
constexpr uint32_t UI_UPDATE_INTERVAL_MS = 100;
constexpr uint32_t DIAGNOSTICS_INTERVAL_MS = 500;
constexpr uint32_t SENSOR_TIMEOUT_MS = 1500;
constexpr uint32_t SENSOR_RETRY_INTERVAL_MS = 2000;
constexpr uint32_t LONG_PRESS_MS = 600;
constexpr uint32_t REPEAT_INTERVAL_MS = 120;
constexpr uint8_t DISPLAY_BRIGHTNESS = 90;

AlarmSettings settings;
DistanceFilter filter;
AlarmEvaluator evaluator;

bool sensorPresent = false;
SensorModel activeModel = SENSOR_NONE;
uint32_t lastSampleMs = 0;
uint32_t lastRetryMs = 0;
uint32_t lastUiUpdateMs = 0;
DistanceSample lastSample;
uint32_t lastDiagnosticsMs = 0;

int8_t activeButton = BUTTON_NONE;
bool touchCancelled = false;
bool repeating = false;
uint32_t pressStartMs = 0;
uint32_t lastRepeatMs = 0;

void applyThresholdStep(int8_t steps) {
  const uint16_t next = stepThreshold(settings.thresholdMm, steps);
  if (next == settings.thresholdMm) return;
  settings.thresholdMm = next;
  buzzerClick();
  screenSetSettings(settings);
  settingsScheduleSave(settings);
}

void activateButton(int8_t button) {
  switch (button) {
    case BUTTON_MINUS:
      applyThresholdStep(-1);
      break;
    case BUTTON_PLUS:
      applyThresholdStep(1);
      break;
    case BUTTON_TOGGLE:
      settings.enabled = !settings.enabled;
      if (settings.enabled) {
        buzzerClick();
      } else {
        evaluator.reset();
        buzzerSetAlarm(false);
        screenSetAlarm(false);
      }
      screenSetSettings(settings);
      settingsScheduleSave(settings);
      break;
    case BUTTON_DIRECTION:
      settings.direction = settings.direction == ALARM_WHEN_CLOSER
                               ? ALARM_WHEN_FARTHER
                               : ALARM_WHEN_CLOSER;
      // Po zmene smeru se alarm vyhodnoti znovu, jinak by mohl zustat viset.
      evaluator.reset();
      buzzerClick();
      screenSetSettings(settings);
      settingsScheduleSave(settings);
      break;
    default:
      break;
  }
}

void releaseActiveButton(bool activate) {
  if (activeButton == BUTTON_NONE) return;
  screenSetButtonPressed(activeButton, false);
  if (activate && !repeating) activateButton(activeButton);
  activeButton = BUTTON_NONE;
  repeating = false;
}

void serviceTouch(uint32_t now) {
  TouchPoint point;
  if (!Touch_ReadPoint(point)) return;  // chyba na I2C neni stisk ani uvolneni

  if (!point.pressed) {
    releaseActiveButton(true);
    touchCancelled = false;
    return;
  }

  const int8_t button = screenButtonAt(point.x, point.y);

  if (activeButton == BUTTON_NONE) {
    // Novy stisk se prijme jen tehdy, kdyz prst dosedl primo na tlacitko.
    if (touchCancelled || button == BUTTON_NONE) {
      touchCancelled = true;
      return;
    }
    activeButton = button;
    pressStartMs = now;
    repeating = false;
    screenSetButtonPressed(button, true);
    return;
  }

  if (button != activeButton) {
    // Prst sjel mimo tlacitko - akce se neprovede a cekame na uvolneni.
    releaseActiveButton(false);
    touchCancelled = true;
    return;
  }

  if (activeButton != BUTTON_MINUS && activeButton != BUTTON_PLUS) return;

  // Podrzeni sipek posouva mez opakovane.
  const int8_t steps = activeButton == BUTTON_PLUS ? 1 : -1;
  if (!repeating) {
    if (now - pressStartMs < LONG_PRESS_MS) return;
    repeating = true;
    lastRepeatMs = now;
    applyThresholdStep(steps);
  } else if (now - lastRepeatMs >= REPEAT_INTERVAL_MS) {
    lastRepeatMs = now;
    applyThresholdStep(steps);
  }
}

// Po rozpoznani senzoru prizpusobi stupnici jeho dosahu. Pri vymene senzoru
// za behu (napr. VL53L1X za VL53L0X) se obrazovka prekresli cela.
void applySensorProfile() {
  const SensorProfile &profile = distanceSensorProfile();
  if (profile.model == SENSOR_NONE || profile.model == activeModel) return;
  activeModel = profile.model;
  setScaleMaxMm(profile.scaleMaxMm);
  // Mez nad dosahem senzoru by nemela smysl. Upravime jen pracovni kopii,
  // ulozene nastaveni zustane - po navratu delsiho senzoru plati zase.
  settings.thresholdMm = clampThreshold(settings.thresholdMm);
  filter.reset();
  evaluator.reset();
  screenInit(settings);
#if !SENSOR_DIAGNOSTICS
  screenSetDiagnostics(profile.name);
#endif
}

void serviceSensor(uint32_t now) {
  if (sensorPresent) {
    DistanceSample sample;
    if (distanceSensorPoll(sample)) {
      lastSampleMs = now;
      lastSample = sample;
      filter.update(sample.kind, sample.mm, now);
#if SENSOR_DIAGNOSTICS
      // Vypisujeme kazdy vzorek - radsi hustsi log nez prehlednuta serie chyb.
      Serial.printf("%s status=%u (%s) mm=%u signal=%.2f ambient=%.2f\n",
                    distanceSensorProfile().name, sample.status,
                    distanceSensorStatusName(sample.status),
                    sample.mm, sample.signal, sample.ambient);
#endif
      return;
    }
    // Odpojeny senzor prestane dodavat vzorky; nesmime drzet posledni hodnotu.
    if (now - lastSampleMs > SENSOR_TIMEOUT_MS) {
      sensorPresent = false;
      filter.reset();
      screenSetSensorError(true);
      lastRetryMs = now;
    }
    return;
  }

  if (now - lastRetryMs < SENSOR_RETRY_INTERVAL_MS) return;
  lastRetryMs = now;
  sensorPresent = distanceSensorBegin();
  if (sensorPresent) {
    lastSampleMs = now;
    applySensorProfile();
    screenSetSensorError(false);
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("DistanceRing startuje");

  I2C_Init();
  // Vychozi urovne vystupu expanderu a smery pinu (EXIO5-7 jsou vstupy).
  Set_EXIOS(0x0C);
  TCA9554PWR_Init(0x70);

  buzzerBegin();
  settingsBegin(settings);

  LCD_Init();
  gfxBegin(LCD_GetFramebuffer());
  screenInit(settings);
  Set_Backlight(DISPLAY_BRIGHTNESS);

#if SENSOR_DIAGNOSTICS
  distanceSensorScanBus();
#endif
  sensorPresent = distanceSensorBegin();
  lastSampleMs = millis();
  applySensorProfile();
  screenSetSensorError(!sensorPresent);
}

void loop() {
  const uint32_t now = millis();

  serviceSensor(now);
  serviceTouch(now);

  if (now - lastUiUpdateMs >= UI_UPDATE_INTERVAL_MS) {
    lastUiUpdateMs = now;
    const MeasurementState state = filter.state();
    const uint16_t distance = filter.distanceMm();
    // Cil za dosahem je pro alarm platny udaj - je dal nez konec stupnice.
    const bool triggered =
        evaluator.update(state != MEASUREMENT_INVALID, distance, settings);
    buzzerSetAlarm(triggered);
    screenSetAlarm(triggered);
    screenSetDistance(state, distance);
    screenSetWeakSignal(filter.weakSignal());
  }

#if SENSOR_DIAGNOSTICS
  if (now - lastDiagnosticsMs >= DIAGNOSTICS_INTERVAL_MS) {
    lastDiagnosticsMs = now;
    char line[48];
    snprintf(line, sizeof(line), "%s  st %u  %u mm  s %.1f",
             distanceSensorProfile().name, lastSample.status, lastSample.mm,
             lastSample.signal);
    screenSetDiagnostics(line);
  }
#endif

  settingsLoop(now);
  delay(5);
}
