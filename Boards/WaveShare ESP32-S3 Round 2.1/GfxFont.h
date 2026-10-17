// Format fontu pro vlastni vykreslovani: jeden bajt alfa na pixel, znaky
// serazene podle kodu (vyhledava se puleni intervalu).
#pragma once

#include <stdint.h>

struct GfxGlyph {
  uint16_t code;     // Unicode kod znaku
  uint32_t offset;   // zacatek obrazu v poli bitmap
  uint8_t width;
  uint8_t height;
  int8_t left;       // posun od pozice pera doprava
  int8_t top;        // vyska horniho okraje nad uctarou
  uint8_t advance;   // o kolik se posune pero
};

struct GfxFont {
  const uint8_t *bitmap;
  const GfxGlyph *glyphs;
  uint16_t glyphCount;
  uint8_t lineHeight;
  uint8_t ascent;
};

extern const GfxFont fontValue;
extern const GfxFont fontUi20;
extern const GfxFont fontUi28;
extern const GfxFont fontUi40;
