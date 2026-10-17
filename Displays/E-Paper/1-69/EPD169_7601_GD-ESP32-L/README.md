# EPD169_7601 (1.69" 400x400, JD7601/IST7601)

Converted from your original MSP430 reference `1D69_E6_400x400_7601_test.c`
into the same Arduino + ESP-IDF layout used by the GDEH0154E01 project,
with **identical IO pin definitions**.

## Project Structure

```
EPD169_7601/
├── CMakeLists.txt              # Root project CMake config (ESP-IDF)
├── EPD169_7601.ino             # Arduino version
├── image.h                     # Image data header (Arduino copy)
├── main/
│   ├── CMakeLists.txt          # Main component CMake config (ESP-IDF)
│   ├── EPD169_7601_idf.c       # Main application source code (ESP-IDF)
│   └── image.h                 # Image data header (ESP-IDF copy)
└── README.md                   # This file
```

## What changed vs. the MSP430 reference

- Panel resolution: **400 x 400**, 4bpp packed (2 pixels/byte) → frame = 80000 bytes
  (reference `dis_img()` looped `col 0..399` x `row 0..199`, i.e. the same 80000-byte frame).
- Color byte values (`PIC_BLACK/WHITE/YELLOW/RED/BLUE/GREEN`) copied 1:1 into `EpdColorByte`.
- `INIT_IST7601()` sequence copied 1:1: enter test cmd (`0xFF/0xA5`) → `0xD6/0x00` → exit test
  cmd (`0xFF/0xE3`) → `0xE9/0x01`.
- `RESET()` double low/high pulse copied 1:1 (reference toggles RST twice before waiting BUSY).
- Command set for display (`0x10` DTM1 write, `0x04` Power ON, `0x12` Refresh, `0x02` Power OFF,
  `0x07`+`0xA5` Deep sleep) copied 1:1.
- The MSP430 code only ever displayed a **solid color** (`dis_img(PIC_GREEN)` in `main()`).
  This conversion keeps that as `epd_display_solid()` / `epdDisplaySolid()`, and additionally
  adds `epd_display_image()` / `epdDisplayImage()` so you can push a real image buffer from
  `image.h` (same pattern as the GDEH0154E01 project). `image.h` currently contains a
  **placeholder** solid-yellow (`0x22`) 80000-byte array — replace it with real converted image
  data.
- `read_temperture()` and `read_Revision()` used a single SDA line switched between
  output/input (`P1DIR` toggling) to read data back — this is a 3-wire bit-banged read that
  hardware SPI (MOSI-only, no MISO wired) can't do directly. Both are ported as optional,
  **disabled by default** (`ENABLE_DIAG_READ 0`) functions that bit-bang the MOSI pin directly
  when enabled. They are not required for normal display operation.

## IO Pin Definitions (identical to GDEH0154E01 project)

Edit the board-select line to pick your target:

```c
// For ESP32-S3
#define BOARD_ESP32S3 1

// For ESP32
#define BOARD_ESP32S3 0
```

### ESP32-S3 Pin Configuration
- MOSI: GPIO 11
- CLK:  GPIO 12
- BUSY: GPIO 5
- DC:   GPIO 3
- CS:   GPIO 2
- RST:  GPIO 4

### ESP32 Pin Configuration
- MOSI: GPIO 23
- CLK:  GPIO 18
- BUSY: GPIO 14 (A14)
- DC:   GPIO 16 (A16)
- CS:   GPIO 17 (A17)
- RST:  GPIO 15 (A15)

## Arduino Usage

1. Open `EPD169_7601.ino` in Arduino IDE (with ESP32 board package installed).
2. Make sure `image.h` is in the same sketch folder (it already is).
3. Select your board (ESP32-S3 or ESP32 dev module), set `BOARD_ESP32S3` accordingly.
4. Upload.

## ESP-IDF Usage

```bash
. $IDF_PATH/export.sh
cd EPD169_7601
idf.py set-target esp32s3   # or esp32
idf.py menuconfig           # optional
idf.py -p PORT build flash monitor
```

## Application Flow

1. **Initialize**: GPIO + hardware SPI (2 MHz, MOSI-only, no MISO)
2. **Reset**: double low/high RST pulse + wait BUSY (matches reference `RESET()`)
3. **(optional) Diagnostic reads**: temperature / chip revision, bit-banged, disabled by default
4. **Init EPD**: `INIT_IST7601()` test-mode command sequence
5. **Display image**: pushes `image_data_sixcolor` from `image.h` (replace placeholder with real data)
6. **Deep sleep**: `0x07` / `0xA5`

## Customization

### Replace the image
Regenerate `image.h` (both copies, root and `main/`) with your own 400x400, 4bpp packed
(2px/byte, one nibble per pixel using the same color byte values) image data — 80000 bytes total.

### Reproduce the reference's solid-green output exactly
Call `epdDisplaySolid(COLOR_GREEN)` / `epd_display_solid(COLOR_GREEN)` instead of
`epdDisplayImage()` / `epd_display_image()`.

### SPI Speed
Change `clock_speed_hz` in `epd_hardware_init()` (ESP-IDF) or `epdSpiSettings` (Arduino).
