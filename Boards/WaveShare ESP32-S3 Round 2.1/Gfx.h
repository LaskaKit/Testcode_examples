// Minimalisticke kresleni primo do framebufferu RGB panelu (RGB565).
// Nahrazuje grafickou knihovnu - firmware si vystaci s obdelniky, mezikruzim
// a textem z vlastnich fontu.
#pragma once

#include <stdint.h>

#include "GfxFont.h"

constexpr int16_t GFX_WIDTH = 480;
constexpr int16_t GFX_HEIGHT = 480;

void gfxBegin(uint16_t *framebuffer);
uint16_t *gfxFramebuffer();

constexpr uint16_t gfxColor(uint32_t rgb888) {
  return static_cast<uint16_t>(((rgb888 >> 8) & 0xF800) |
                               ((rgb888 >> 5) & 0x07E0) |
                               ((rgb888 >> 3) & 0x001F));
}

void gfxFillScreen(uint16_t color);
void gfxFillRect(int16_t x, int16_t y, int16_t width, int16_t height,
                 uint16_t color);
void gfxFillRoundRect(int16_t x, int16_t y, int16_t width, int16_t height,
                      int16_t radius, uint16_t color);
// Vysec mezikruzi; uhly ve stupnich, 0 miri doprava a rostou po smeru
// hodinovych rucicek.
void gfxFillRing(int16_t centerX, int16_t centerY, int16_t innerRadius,
                 int16_t outerRadius, float startDegrees, float endDegrees,
                 uint16_t color);

int16_t gfxTextWidth(const GfxFont &font, const char *text);
void gfxDrawText(int16_t x, int16_t baselineY, const char *text,
                 const GfxFont &font, uint16_t color);
void gfxDrawTextCentered(int16_t centerX, int16_t baselineY, const char *text,
                         const GfxFont &font, uint16_t color);
// Znak vycentrovany do bunky pevne sirky - cisla pak pri zmene neposkakuji.
void gfxDrawGlyphInCell(int16_t cellX, int16_t cellWidth, int16_t baselineY,
                        uint32_t codepoint, const GfxFont &font,
                        uint16_t color);
