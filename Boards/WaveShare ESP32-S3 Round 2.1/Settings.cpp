#include "Settings.h"

#include <Arduino.h>
#include <Preferences.h>

namespace {
constexpr char NAMESPACE[] = "distring";
constexpr char KEY_ENABLED[] = "enabled";
constexpr char KEY_THRESHOLD[] = "threshold";
constexpr char KEY_DIRECTION[] = "direction";
constexpr uint32_t SAVE_DELAY_MS = 2000;

Preferences preferences;
AlarmSettings pending;
bool savePending = false;
uint32_t saveAtMs = 0;
}  // namespace

void settingsBegin(AlarmSettings &settings) {
  if (!preferences.begin(NAMESPACE, false)) {
    Serial.println("NVS se nepodarilo otevrit, pouziji vychozi nastaveni");
    return;
  }
  settings.enabled = preferences.getBool(KEY_ENABLED, settings.enabled);
  settings.thresholdMm =
      clampThreshold(preferences.getUShort(KEY_THRESHOLD, settings.thresholdMm));
  settings.direction = static_cast<AlarmDirection>(
      preferences.getUChar(KEY_DIRECTION, settings.direction) ==
              ALARM_WHEN_FARTHER
          ? ALARM_WHEN_FARTHER
          : ALARM_WHEN_CLOSER);
}

void settingsScheduleSave(const AlarmSettings &settings) {
  pending = settings;
  savePending = true;
  saveAtMs = millis() + SAVE_DELAY_MS;
}

void settingsLoop(uint32_t nowMs) {
  if (!savePending) return;
  if (static_cast<int32_t>(nowMs - saveAtMs) < 0) return;
  savePending = false;
  preferences.putBool(KEY_ENABLED, pending.enabled);
  preferences.putUShort(KEY_THRESHOLD, pending.thresholdMm);
  preferences.putUChar(KEY_DIRECTION, static_cast<uint8_t>(pending.direction));
}
