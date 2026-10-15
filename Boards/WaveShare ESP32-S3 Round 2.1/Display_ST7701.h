// Panel ST7701S (480x480, kruhovy IPS) na desce Waveshare ESP32-S3-Touch-LCD-2.1.
// Inicializacni sekvence a piny vychazeji z projektu CooLajz/waveshare-hodiny (MIT),
// ponechano je jen to, co tento firmware potrebuje.
#pragma once

#include <inttypes.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "TCA9554PWR.h"
#include "Touch_CST820.h"

// Konfiguracni SPI panelu (3-wire, CS vede pres expander EXIO3).
#define LCD_CLK_PIN 2
#define LCD_MOSI_PIN 1
#define LCD_BACKLIGHT_PIN 6

#define BACKLIGHT_FREQUENCY 20000
#define BACKLIGHT_RESOLUTION 10
#define BACKLIGHT_MAX 100

#define LCD_WIDTH (480)
#define LCD_HEIGHT (480)

#define LCD_RGB_PIXEL_BITS (16)
#define LCD_RGB_DATA_WIDTH (16)
#define LCD_RGB_TIMING_FREQ_HZ (12 * 1000 * 1000)
#define LCD_RGB_TIMING_HPW (8)
#define LCD_RGB_TIMING_HBP (10)
#define LCD_RGB_TIMING_HFP (50)
#define LCD_RGB_TIMING_VPW (3)
#define LCD_RGB_TIMING_VBP (8)
#define LCD_RGB_TIMING_VFP (8)
// Jediny framebuffer v PSRAM: kresli se do nej primo, zadne prehazovani.
#define LCD_RGB_FRAME_BUF_NUM (1)
// Dvacet radku v interni RAM omezuje trhani obrazu pri soubehu s Wi-Fi.
#define LCD_RGB_BOUNCE_BUF_SIZE (LCD_WIDTH * 20)

#define LCD_PIN_NUM_RGB_HSYNC (38)
#define LCD_PIN_NUM_RGB_VSYNC (39)
#define LCD_PIN_NUM_RGB_DE (40)
#define LCD_PIN_NUM_RGB_PCLK (41)
#define LCD_PIN_NUM_RGB_DATA0 (5)
#define LCD_PIN_NUM_RGB_DATA1 (45)
#define LCD_PIN_NUM_RGB_DATA2 (48)
#define LCD_PIN_NUM_RGB_DATA3 (47)
#define LCD_PIN_NUM_RGB_DATA4 (21)
#define LCD_PIN_NUM_RGB_DATA5 (14)
#define LCD_PIN_NUM_RGB_DATA6 (13)
#define LCD_PIN_NUM_RGB_DATA7 (12)
#define LCD_PIN_NUM_RGB_DATA8 (11)
#define LCD_PIN_NUM_RGB_DATA9 (10)
#define LCD_PIN_NUM_RGB_DATA10 (9)
#define LCD_PIN_NUM_RGB_DATA11 (46)
#define LCD_PIN_NUM_RGB_DATA12 (3)
#define LCD_PIN_NUM_RGB_DATA13 (8)
#define LCD_PIN_NUM_RGB_DATA14 (18)
#define LCD_PIN_NUM_RGB_DATA15 (17)
#define LCD_PIN_NUM_RGB_DISP (-1)

extern esp_lcd_panel_handle_t panel_handle;
extern uint8_t LCD_Backlight;

void LCD_Init();
void LCD_Resync();
// Framebuffer panelu v PSRAM (RGB565, 480x480). Zapis se projevi na displeji
// pri nejblizsim obnoveni obrazu.
uint16_t *LCD_GetFramebuffer();

void Backlight_Init();
void Set_Backlight(uint8_t light);
