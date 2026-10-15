// Vykresleni obrazovky mericiho pristroje. Zadna vazba na Arduino - diky tomu
// jde stejny kod prelozit na pocitaci a vyrenderovat nahled (tools/preview.cpp).
#pragma once

#include <stdint.h>

#include "AlarmLogic.h"

// Poradi tlacitek odpovida indexum vracenym z screenButtonAt().
enum ScreenButton : int8_t {
  BUTTON_NONE = -1,
  BUTTON_MINUS = 0,
  BUTTON_TOGGLE = 1,
  BUTTON_PLUS = 2,
  BUTTON_DIRECTION = 3,
};

void screenInit(const AlarmSettings &settings);
void screenSetDistance(MeasurementState state, uint16_t distanceMm);
void screenSetAlarm(bool triggered);
void screenSetSettings(const AlarmSettings &settings);
void screenSetSensorError(bool error);
// Hodnota je jen odhad pri velmi slabem odrazu; nadpis to da najevo.
void screenSetWeakSignal(bool weak);
// Diagnosticky radek pod tlacitky; prazdny retezec ho smaze.
void screenSetDiagnostics(const char *text);
void screenSetButtonPressed(int8_t button, bool pressed);

// Ktere tlacitko lezi pod dotykem; BUTTON_NONE kdyz zadne.
int8_t screenButtonAt(int16_t x, int16_t y);
