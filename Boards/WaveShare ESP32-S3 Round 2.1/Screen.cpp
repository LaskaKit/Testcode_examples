#include "Screen.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "Gfx.h"

namespace {
// Paleta
constexpr uint16_t COLOR_BACKGROUND = gfxColor(0x0B0F14);
constexpr uint16_t COLOR_TRACK = gfxColor(0x1C2733);
constexpr uint16_t COLOR_TICK_MINOR = gfxColor(0x334155);
constexpr uint16_t COLOR_TICK_MAJOR = gfxColor(0x7A8CA3);
constexpr uint16_t COLOR_VALUE = gfxColor(0x22D3EE);
constexpr uint16_t COLOR_VALUE_DIM = gfxColor(0x155E75);
constexpr uint16_t COLOR_ALARM = gfxColor(0xF43F5E);
constexpr uint16_t COLOR_MARKER = gfxColor(0xFBBF24);
constexpr uint16_t COLOR_MARKER_OFF = gfxColor(0x475569);
constexpr uint16_t COLOR_TEXT = gfxColor(0xE2E8F0);
constexpr uint16_t COLOR_TEXT_MUTED = gfxColor(0x94A3B8);
constexpr uint16_t COLOR_BUTTON = gfxColor(0x1E293B);
constexpr uint16_t COLOR_BUTTON_PRESSED = gfxColor(0x3B4A5F);
constexpr uint16_t COLOR_BUTTON_ON = gfxColor(0x15803D);
constexpr uint16_t COLOR_BUTTON_ON_PRESSED = gfxColor(0x1C9A4B);

// Geometrie
constexpr int16_t CENTER_X = 240;
constexpr int16_t CENTER_Y = 240;
constexpr int16_t RING_INNER = 216;
constexpr int16_t RING_OUTER = 234;
constexpr int16_t MARKER_INNER = 210;
constexpr int16_t MARKER_OUTER = 238;
constexpr int16_t MARKER_HALF_WIDTH_MM = 40;
constexpr int16_t TICK_MINOR_INNER = 196;
constexpr int16_t TICK_OUTER = 206;
constexpr int16_t TICK_MAJOR_INNER = 188;
constexpr int16_t TICK_LABEL_RADIUS = 166;

constexpr int16_t CAPTION_BASELINE = 128;
constexpr int16_t VALUE_BASELINE = 248;
constexpr int16_t VALUE_CLEAR_X = 92;
constexpr int16_t VALUE_CLEAR_WIDTH = 296;
// Horni hrana lezi pod popisky stupnice (0,5 a 1,5 na kratsi stupnici).
constexpr int16_t VALUE_CLEAR_Y = 166;
constexpr int16_t VALUE_CLEAR_HEIGHT = 108;
// Cisla se kresli do pevnych bunek, aby pri zmene hodnoty neposkakovala.
constexpr int16_t VALUE_CELL_DIGIT = 67;
constexpr int16_t VALUE_CELL_COMMA = 29;
constexpr int16_t VALUE_START_X = 102;
constexpr int16_t UNIT_X = 344;

constexpr int16_t PILL_WIDTH = 220;
constexpr int16_t PILL_HEIGHT = 46;
constexpr int16_t PILL_CENTER_Y = 300;

constexpr int16_t BUTTON_HEIGHT = 56;
constexpr int16_t BUTTON_CENTER_Y = 362;
constexpr int16_t BUTTON_SIDE_WIDTH = 76;
constexpr int16_t BUTTON_MIDDLE_WIDTH = 96;
constexpr int16_t BUTTON_SIDE_OFFSET = 98;

// Diagnosticky radek pod tlacitky, jeste uvnitr kruhove plochy displeje.
constexpr int16_t DIAGNOSTICS_BASELINE = 418;
constexpr int16_t DIAGNOSTICS_X = 90;
constexpr int16_t DIAGNOSTICS_WIDTH = 300;

struct Rect {
  int16_t x;
  int16_t y;
  int16_t width;
  int16_t height;
};

constexpr Rect rectCentered(int16_t centerX, int16_t centerY, int16_t width,
                            int16_t height) {
  return Rect{static_cast<int16_t>(centerX - width / 2),
              static_cast<int16_t>(centerY - height / 2), width, height};
}

const Rect BUTTON_RECTS[4] = {
    rectCentered(CENTER_X - BUTTON_SIDE_OFFSET, BUTTON_CENTER_Y,
                 BUTTON_SIDE_WIDTH, BUTTON_HEIGHT),
    rectCentered(CENTER_X, BUTTON_CENTER_Y, BUTTON_MIDDLE_WIDTH, BUTTON_HEIGHT),
    rectCentered(CENTER_X + BUTTON_SIDE_OFFSET, BUTTON_CENTER_Y,
                 BUTTON_SIDE_WIDTH, BUTTON_HEIGHT),
    rectCentered(CENTER_X, PILL_CENTER_Y, PILL_WIDTH, PILL_HEIGHT),
};

// Stav obrazovky
AlarmSettings settings;
MeasurementState measurement = MEASUREMENT_INVALID;
uint16_t distanceMm = 0;
bool alarmTriggered = false;
bool sensorError = false;
bool weakSignal = false;
bool buttonPressed[4] = {false, false, false, false};

uint16_t ringColor() { return alarmTriggered ? COLOR_ALARM : COLOR_VALUE; }
uint16_t markerColor() {
  return settings.enabled ? COLOR_MARKER : COLOR_MARKER_OFF;
}

void drawMarker() {
  // Znacka presahuje prstenec dovnitr i ven, proto ji nelze zamazat jen
  // prekreslenim prstence - stopy po predchozi poloze by zustaly na okrajich.
  const float markerStart =
      angleForDistance(clampDistance(settings.thresholdMm - MARKER_HALF_WIDTH_MM));
  const float markerEnd =
      angleForDistance(clampDistance(settings.thresholdMm + MARKER_HALF_WIDTH_MM));
  gfxFillRing(CENTER_X, CENTER_Y, MARKER_INNER, MARKER_OUTER, markerStart,
              markerEnd, markerColor());
}

// Smaze cely pas, ve kterem se pohybuje prstenec i znacka, a vykresli podklad.
void drawRingTrack() {
  const float startAngle = angleForDistance(DISTANCE_MIN_MM);
  const float endAngle = angleForDistance(scaleMaxMm());
  gfxFillRing(CENTER_X, CENTER_Y, MARKER_INNER, MARKER_OUTER, startAngle,
              endAngle, COLOR_BACKGROUND);
  gfxFillRing(CENTER_X, CENTER_Y, RING_INNER, RING_OUTER, startAngle, endAngle,
              COLOR_TRACK);
}

void drawRing() {
  const float startAngle = angleForDistance(DISTANCE_MIN_MM);
  const float endAngle = angleForDistance(scaleMaxMm());

  drawRingTrack();
  if (measurement == MEASUREMENT_OVER_RANGE) {
    // Za dosahem je prstenec plny, ale tlumeny - neni to zmerena hodnota.
    gfxFillRing(CENTER_X, CENTER_Y, RING_INNER, RING_OUTER, startAngle,
                endAngle, COLOR_VALUE_DIM);
  } else if (measurement == MEASUREMENT_VALID && distanceMm > 0) {
    gfxFillRing(CENTER_X, CENTER_Y, RING_INNER, RING_OUTER, startAngle,
                angleForDistance(distanceMm), ringColor());
  }
  drawMarker();
}

// Pri zmene hodnoty staci prebarvit vysec mezi starym a novym uhlem.
void updateRingFill(uint16_t previousMm, uint16_t currentMm) {
  const bool growing = currentMm > previousMm;
  const uint16_t from = growing ? previousMm : currentMm;
  const uint16_t to = growing ? currentMm : previousMm;
  gfxFillRing(CENTER_X, CENTER_Y, RING_INNER, RING_OUTER,
              angleForDistance(from), angleForDistance(to),
              growing ? ringColor() : COLOR_TRACK);
  // Vysec mohla prekryt znacku, takze ji vratime zpet.
  drawMarker();
}

void drawTicks() {
  // Na kratsi stupnici (VL53L0X, 2 m) jsou popisky po pul metru, at jich je
  // podobne jako na ctyrmetrove.
  const uint16_t majorStep = scaleMaxMm() <= 2000 ? 500 : 1000;
  for (uint16_t millimeters = DISTANCE_MIN_MM; millimeters <= scaleMaxMm();
       millimeters += 100) {
    const bool major = millimeters % majorStep == 0;
    const float angle = angleForDistance(millimeters);
    // Sirka rysky v uhlu podle pozadovane sirky v pixelech na vnejsim okraji.
    const float halfWidth =
        (major ? 2.0f : 1.0f) / TICK_OUTER * 180.0f / static_cast<float>(M_PI);
    gfxFillRing(CENTER_X, CENTER_Y, major ? TICK_MAJOR_INNER : TICK_MINOR_INNER,
                TICK_OUTER, angle - halfWidth, angle + halfWidth,
                major ? COLOR_TICK_MAJOR : COLOR_TICK_MINOR);

    if (!major) continue;
    const float radians = angle * static_cast<float>(M_PI) / 180.0f;
    const int16_t labelX =
        CENTER_X + static_cast<int16_t>(lroundf(cosf(radians) * TICK_LABEL_RADIUS));
    const int16_t labelY =
        CENTER_Y + static_cast<int16_t>(lroundf(sinf(radians) * TICK_LABEL_RADIUS));
    char label[8];
    if (millimeters % 1000 == 0)
      snprintf(label, sizeof(label), "%u", millimeters / 1000u);
    else
      snprintf(label, sizeof(label), "%u,%u", millimeters / 1000u,
               (millimeters % 1000u) / 100u);
    gfxDrawTextCentered(labelX, labelY + 7, label, fontUi20, COLOR_TICK_MAJOR);
  }
}

void drawCaption() {
  const char *text = "VZDÁLENOST";
  uint16_t color = COLOR_TEXT_MUTED;
  if (sensorError) {
    text = "SENZOR NEODPOVÍDÁ";
    color = COLOR_ALARM;
  } else if (measurement == MEASUREMENT_OVER_RANGE) {
    text = "MIMO DOSAH";
  } else if (measurement == MEASUREMENT_VALID && weakSignal) {
    // Hodnota je odhad z rozptylenych vzorku - at je to poznat.
    text = "SLABÝ ODRAZ";
    color = COLOR_MARKER;
  }
  // Mazeme jen plochu textu - sirsi obdelnik by vyrizl prstenec a rysky.
  gfxFillRect(CENTER_X - 120, CAPTION_BASELINE - 22, 240, 30, COLOR_BACKGROUND);
  gfxDrawTextCentered(CENTER_X, CAPTION_BASELINE, text, fontUi20, color);
}

void drawValue() {
  gfxFillRect(VALUE_CLEAR_X, VALUE_CLEAR_Y, VALUE_CLEAR_WIDTH,
              VALUE_CLEAR_HEIGHT, COLOR_BACKGROUND);

  char meters[8] = "-,--";
  uint16_t color = COLOR_TEXT_MUTED;
  if (measurement == MEASUREMENT_VALID) {
    formatMeters(distanceMm, meters, sizeof(meters));
    color = ringColor();
  } else if (measurement == MEASUREMENT_OVER_RANGE) {
    // Konec stupnice tlumene - cil je dal, nez senzor dohledne.
    formatMeters(scaleMaxMm(), meters, sizeof(meters));
    color = COLOR_VALUE_DIM;
  }

  // Rozlozeni "c,cc": tri cislice v sirokych bunkach a uzka bunka pro carku.
  const int16_t cells[4] = {VALUE_CELL_DIGIT, VALUE_CELL_COMMA, VALUE_CELL_DIGIT,
                            VALUE_CELL_DIGIT};
  int16_t x = VALUE_START_X;
  for (uint8_t index = 0; index < 4 && meters[index] != '\0'; ++index) {
    gfxDrawGlyphInCell(x, cells[index], VALUE_BASELINE,
                       static_cast<uint8_t>(meters[index]), fontValue, color);
    x += cells[index];
  }
  gfxDrawText(UNIT_X, VALUE_BASELINE, "m", fontUi40, COLOR_TEXT_MUTED);
}

void drawButton(int8_t index) {
  const Rect &rect = BUTTON_RECTS[index];
  const bool pressed = buttonPressed[index];
  const bool highlighted = index == BUTTON_TOGGLE && settings.enabled;
  uint16_t background = COLOR_BUTTON;
  if (highlighted)
    background = pressed ? COLOR_BUTTON_ON_PRESSED : COLOR_BUTTON_ON;
  else if (pressed)
    background = COLOR_BUTTON_PRESSED;

  const int16_t radius = index == BUTTON_DIRECTION ? rect.height / 2 : 20;
  gfxFillRoundRect(rect.x, rect.y, rect.width, rect.height, radius, background);

  const int16_t centerX = rect.x + rect.width / 2;
  const int16_t centerY = rect.y + rect.height / 2;
  switch (index) {
    case BUTTON_MINUS:
      gfxDrawTextCentered(centerX, centerY + 14, "-", fontUi40, COLOR_TEXT);
      break;
    case BUTTON_PLUS:
      gfxDrawTextCentered(centerX, centerY + 14, "+", fontUi40, COLOR_TEXT);
      break;
    case BUTTON_TOGGLE:
      gfxDrawTextCentered(centerX, centerY + 10,
                          settings.enabled ? "ZAP" : "VYP", fontUi28,
                          settings.enabled ? COLOR_TEXT : COLOR_TEXT_MUTED);
      break;
    case BUTTON_DIRECTION: {
      char text[32];
      char meters[8];
      formatMeters(settings.thresholdMm, meters, sizeof(meters));
      snprintf(text, sizeof(text), "%s %s m",
               settings.direction == ALARM_WHEN_CLOSER ? "blíž než" : "dál než",
               meters);
      gfxDrawTextCentered(centerX, centerY + 10, text, fontUi28,
                          settings.enabled ? COLOR_MARKER : COLOR_TEXT_MUTED);
      break;
    }
    default:
      break;
  }
}
}  // namespace

void screenInit(const AlarmSettings &initialSettings) {
  settings = initialSettings;
  gfxFillScreen(COLOR_BACKGROUND);
  drawRing();
  drawTicks();
  drawCaption();
  drawValue();
  for (int8_t index = 0; index < 4; ++index) drawButton(index);
}

void screenSetDistance(MeasurementState state, uint16_t millimeters) {
  if (state == measurement && millimeters == distanceMm) return;
  const bool stateChanges = state != measurement;
  const bool textChanges =
      stateChanges || millimeters / 10 != distanceMm / 10;
  const uint16_t previousMm = distanceMm;
  const MeasurementState previousState = measurement;
  measurement = state;
  distanceMm = millimeters;

  if (stateChanges) drawCaption();
  if (textChanges) drawValue();

  if (!stateChanges && state == MEASUREMENT_VALID &&
      previousState == MEASUREMENT_VALID) {
    updateRingFill(previousMm, millimeters);
  } else {
    drawRing();
  }
}

void screenSetAlarm(bool triggered) {
  if (alarmTriggered == triggered) return;
  alarmTriggered = triggered;
  drawValue();
  drawRing();
}

void screenSetSettings(const AlarmSettings &updated) {
  settings = updated;
  drawRing();
  drawButton(BUTTON_TOGGLE);
  drawButton(BUTTON_DIRECTION);
}

void screenSetSensorError(bool error) {
  if (sensorError == error) return;
  sensorError = error;
  drawCaption();
}

void screenSetWeakSignal(bool weak) {
  if (weakSignal == weak) return;
  weakSignal = weak;
  drawCaption();
}

void screenSetDiagnostics(const char *text) {
  gfxFillRect(DIAGNOSTICS_X, DIAGNOSTICS_BASELINE - 20, DIAGNOSTICS_WIDTH, 26,
              COLOR_BACKGROUND);
  if (text == nullptr || text[0] == '\0') return;
  gfxDrawTextCentered(CENTER_X, DIAGNOSTICS_BASELINE, text, fontUi20,
                      COLOR_TEXT_MUTED);
}

void screenSetButtonPressed(int8_t button, bool pressed) {
  if (button < 0 || button > BUTTON_DIRECTION) return;
  if (buttonPressed[button] == pressed) return;
  buttonPressed[button] = pressed;
  drawButton(button);
}

int8_t screenButtonAt(int16_t x, int16_t y) {
  for (int8_t index = 0; index < 4; ++index) {
    const Rect &rect = BUTTON_RECTS[index];
    // Dotykove okoli tlacitek je o neco vetsi nez jejich obrys.
    if (x >= rect.x - 6 && x < rect.x + rect.width + 6 && y >= rect.y - 6 &&
        y < rect.y + rect.height + 6) {
      return index;
    }
  }
  return BUTTON_NONE;
}
