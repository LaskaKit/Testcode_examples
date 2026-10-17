#include "Gfx.h"

#include <math.h>

namespace {
uint16_t *buffer = nullptr;

inline void blendPixel(int16_t x, int16_t y, uint16_t color, uint8_t alpha) {
  if (x < 0 || y < 0 || x >= GFX_WIDTH || y >= GFX_HEIGHT || alpha == 0) return;
  uint16_t *pixel = &buffer[static_cast<int32_t>(y) * GFX_WIDTH + x];
  if (alpha == 255) {
    *pixel = color;
    return;
  }
  // Michani po slozkach primo ve formatu RGB565.
  const uint16_t destination = *pixel;
  const uint16_t inverse = 255 - alpha;
  const uint16_t red =
      (((color >> 11) & 0x1F) * alpha + ((destination >> 11) & 0x1F) * inverse) /
      255;
  const uint16_t green =
      (((color >> 5) & 0x3F) * alpha + ((destination >> 5) & 0x3F) * inverse) /
      255;
  const uint16_t blue =
      ((color & 0x1F) * alpha + (destination & 0x1F) * inverse) / 255;
  *pixel = static_cast<uint16_t>((red << 11) | (green << 5) | blue);
}

const GfxGlyph *findGlyph(const GfxFont &font, uint32_t codepoint) {
  int16_t low = 0;
  int16_t high = static_cast<int16_t>(font.glyphCount) - 1;
  while (low <= high) {
    const int16_t middle = (low + high) / 2;
    const uint16_t code = font.glyphs[middle].code;
    if (code == codepoint) return &font.glyphs[middle];
    if (code < codepoint)
      low = middle + 1;
    else
      high = middle - 1;
  }
  return nullptr;
}

// Vrati dalsi znak z UTF-8 retezce a posune ukazatel.
uint32_t nextCodepoint(const char **text) {
  const uint8_t *cursor = reinterpret_cast<const uint8_t *>(*text);
  const uint8_t first = *cursor;
  if (first == 0) return 0;

  uint32_t codepoint = first;
  uint8_t extra = 0;
  if ((first & 0xE0) == 0xC0) {
    codepoint = first & 0x1F;
    extra = 1;
  } else if ((first & 0xF0) == 0xE0) {
    codepoint = first & 0x0F;
    extra = 2;
  } else if ((first & 0xF8) == 0xF0) {
    codepoint = first & 0x07;
    extra = 3;
  }

  ++cursor;
  for (uint8_t i = 0; i < extra; ++i) {
    if ((*cursor & 0xC0) != 0x80) break;  // poskozena sekvence, neutikat z retezce
    codepoint = (codepoint << 6) | (*cursor & 0x3F);
    ++cursor;
  }
  *text = reinterpret_cast<const char *>(cursor);
  return codepoint;
}

void drawGlyph(int16_t penX, int16_t baselineY, const GfxGlyph &glyph,
               const GfxFont &font, uint16_t color) {
  const uint8_t *source = font.bitmap + glyph.offset;
  const int16_t left = penX + glyph.left;
  const int16_t top = baselineY - glyph.top;
  for (uint8_t row = 0; row < glyph.height; ++row) {
    for (uint8_t column = 0; column < glyph.width; ++column) {
      blendPixel(left + column, top + row, color, source[row * glyph.width + column]);
    }
  }
}
}  // namespace

void gfxBegin(uint16_t *framebuffer) { buffer = framebuffer; }

uint16_t *gfxFramebuffer() { return buffer; }

void gfxFillScreen(uint16_t color) {
  gfxFillRect(0, 0, GFX_WIDTH, GFX_HEIGHT, color);
}

void gfxFillRect(int16_t x, int16_t y, int16_t width, int16_t height,
                 uint16_t color) {
  if (buffer == nullptr || width <= 0 || height <= 0) return;
  int16_t left = x < 0 ? 0 : x;
  int16_t top = y < 0 ? 0 : y;
  int16_t right = x + width > GFX_WIDTH ? GFX_WIDTH : x + width;
  int16_t bottom = y + height > GFX_HEIGHT ? GFX_HEIGHT : y + height;
  for (int16_t row = top; row < bottom; ++row) {
    uint16_t *line = &buffer[static_cast<int32_t>(row) * GFX_WIDTH + left];
    for (int16_t column = left; column < right; ++column) *line++ = color;
  }
}

void gfxFillRoundRect(int16_t x, int16_t y, int16_t width, int16_t height,
                      int16_t radius, uint16_t color) {
  if (buffer == nullptr || width <= 0 || height <= 0) return;
  if (radius > width / 2) radius = width / 2;
  if (radius > height / 2) radius = height / 2;

  for (int16_t row = 0; row < height; ++row) {
    int16_t inset = 0;
    const int16_t fromTop = row;
    const int16_t fromBottom = height - 1 - row;
    const int16_t distance = fromTop < fromBottom ? fromTop : fromBottom;
    if (distance < radius) {
      const float offset = radius - distance - 0.5f;
      const float span = sqrtf(static_cast<float>(radius) * radius - offset * offset);
      inset = static_cast<int16_t>(radius - span + 0.5f);
    }
    gfxFillRect(x + inset, y + row, width - 2 * inset, 1, color);
  }
}

void gfxFillRing(int16_t centerX, int16_t centerY, int16_t innerRadius,
                 int16_t outerRadius, float startDegrees, float endDegrees,
                 uint16_t color) {
  if (buffer == nullptr || outerRadius <= innerRadius) return;
  if (endDegrees < startDegrees) {
    const float swap = startDegrees;
    startDegrees = endDegrees;
    endDegrees = swap;
  }

  // Krok uhlu volime tak, aby sousedni paprsky byly na vnejsim okraji vzdalene
  // nejvyse pul pixelu - mezikruzi pak nema diry.
  const float step = 0.5f / static_cast<float>(outerRadius);
  const float start = startDegrees * static_cast<float>(M_PI) / 180.0f;
  const float end = endDegrees * static_cast<float>(M_PI) / 180.0f;

  // Paprsky lezi na pevne mrizce uhlu spolecne pro vsechna volani se stejnym
  // vnejsim polomerem. Dve ruzne vysece pak zasahnou presne tytez pixely a
  // mazani sirsi vyseci spolehlive smaze i uzsi vysec nakreslenou driv -
  // jinak by po posunu znacky zustavaly na okrajich zbytky.
  const int32_t firstIndex = static_cast<int32_t>(ceilf(start / step));
  const int32_t lastIndex = static_cast<int32_t>(floorf(end / step));

  for (int32_t index = firstIndex; index <= lastIndex; ++index) {
    const float angle = static_cast<float>(index) * step;
    const float cosine = cosf(angle);
    const float sine = sinf(angle);
    for (int16_t radius = innerRadius; radius <= outerRadius; ++radius) {
      const int16_t x = centerX + static_cast<int16_t>(lroundf(cosine * radius));
      const int16_t y = centerY + static_cast<int16_t>(lroundf(sine * radius));
      blendPixel(x, y, color, 255);
    }
  }
}

int16_t gfxTextWidth(const GfxFont &font, const char *text) {
  int16_t width = 0;
  const char *cursor = text;
  uint32_t codepoint = 0;
  while ((codepoint = nextCodepoint(&cursor)) != 0) {
    const GfxGlyph *glyph = findGlyph(font, codepoint);
    if (glyph != nullptr) width += glyph->advance;
  }
  return width;
}

void gfxDrawText(int16_t x, int16_t baselineY, const char *text,
                 const GfxFont &font, uint16_t color) {
  if (buffer == nullptr) return;
  const char *cursor = text;
  uint32_t codepoint = 0;
  while ((codepoint = nextCodepoint(&cursor)) != 0) {
    const GfxGlyph *glyph = findGlyph(font, codepoint);
    if (glyph == nullptr) continue;
    if (glyph->width != 0) drawGlyph(x, baselineY, *glyph, font, color);
    x += glyph->advance;
  }
}

void gfxDrawTextCentered(int16_t centerX, int16_t baselineY, const char *text,
                         const GfxFont &font, uint16_t color) {
  gfxDrawText(centerX - gfxTextWidth(font, text) / 2, baselineY, text, font,
              color);
}

void gfxDrawGlyphInCell(int16_t cellX, int16_t cellWidth, int16_t baselineY,
                        uint32_t codepoint, const GfxFont &font,
                        uint16_t color) {
  const GfxGlyph *glyph = findGlyph(font, codepoint);
  if (glyph == nullptr || glyph->width == 0) return;
  const int16_t penX = cellX + (cellWidth - glyph->advance) / 2;
  drawGlyph(penX, baselineY, *glyph, font, color);
}
