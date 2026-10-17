#include "Display_ST7701.h"

#include <Arduino.h>

esp_lcd_panel_handle_t panel_handle = NULL;

namespace {
spi_device_handle_t spiHandle = NULL;
uint16_t *framebuffer = nullptr;

void writeCommand(uint8_t command) {
  spi_transaction_t transaction = {
      .cmd = 0,
      .addr = command,
      .length = 0,
      .rxlength = 0,
  };
  spi_device_transmit(spiHandle, &transaction);
}

void writeData(uint8_t data) {
  spi_transaction_t transaction = {
      .cmd = 1,
      .addr = data,
      .length = 0,
      .rxlength = 0,
  };
  spi_device_transmit(spiHandle, &transaction);
}

void chipSelectEnable() {
  Set_EXIO(EXIO_PIN3, Low);
  vTaskDelay(pdMS_TO_TICKS(10));
}

void chipSelectDisable() {
  Set_EXIO(EXIO_PIN3, High);
  vTaskDelay(pdMS_TO_TICKS(10));
}

void panelReset() {
  Set_EXIO(EXIO_PIN1, Low);
  vTaskDelay(pdMS_TO_TICKS(10));
  Set_EXIO(EXIO_PIN1, High);
  vTaskDelay(pdMS_TO_TICKS(50));
}

// Inicializacni sekvence panelu ST7701S presne podle vyrobce desky.
struct PanelCommand {
  uint8_t command;
  uint8_t length;
  uint16_t delayMs;
  uint8_t data[16];
};

const PanelCommand PANEL_INIT[] = {
    {0xFF, 5, 0, {0x77, 0x01, 0x00, 0x00, 0x10}},
    {0xC0, 2, 0, {0x3B, 0x00}},
    {0xC1, 2, 0, {0x0B, 0x02}},
    {0xC2, 2, 0, {0x07, 0x02}},
    {0xCC, 1, 0, {0x10}},
    {0xCD, 1, 0, {0x08}},
    {0xB0, 16, 0, {0x00, 0x11, 0x16, 0x0E, 0x11, 0x06, 0x05, 0x09, 0x08, 0x21,
                   0x06, 0x13, 0x10, 0x29, 0x31, 0x18}},
    {0xB1, 16, 0, {0x00, 0x11, 0x16, 0x0E, 0x11, 0x07, 0x05, 0x09, 0x09, 0x21,
                   0x05, 0x13, 0x11, 0x2A, 0x31, 0x18}},
    {0xFF, 5, 0, {0x77, 0x01, 0x00, 0x00, 0x11}},
    {0xB0, 1, 0, {0x6D}},
    {0xB1, 1, 0, {0x37}},
    {0xB2, 1, 0, {0x81}},
    {0xB3, 1, 0, {0x80}},
    {0xB5, 1, 0, {0x43}},
    {0xB7, 1, 0, {0x85}},
    {0xB8, 1, 0, {0x20}},
    {0xC1, 1, 0, {0x78}},
    {0xC2, 1, 0, {0x78}},
    {0xD0, 1, 0, {0x88}},
    {0xE0, 3, 0, {0x00, 0x00, 0x02}},
    {0xE1, 11, 0,
     {0x03, 0xA0, 0x00, 0x00, 0x04, 0xA0, 0x00, 0x00, 0x00, 0x20, 0x20}},
    {0xE2, 13, 0,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00}},
    {0xE3, 4, 0, {0x00, 0x00, 0x11, 0x00}},
    {0xE4, 2, 0, {0x22, 0x00}},
    {0xE5, 16, 0, {0x05, 0xEC, 0xA0, 0xA0, 0x07, 0xEE, 0xA0, 0xA0, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0xE6, 4, 0, {0x00, 0x00, 0x11, 0x00}},
    {0xE7, 2, 0, {0x22, 0x00}},
    {0xE8, 16, 0, {0x06, 0xED, 0xA0, 0xA0, 0x08, 0xEF, 0xA0, 0xA0, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0xEB, 7, 0, {0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00}},
    {0xED, 16, 0, {0xFF, 0xFF, 0xFF, 0xBA, 0x0A, 0xBF, 0x45, 0xFF, 0xFF, 0x54,
                   0xFB, 0xA0, 0xAB, 0xFF, 0xFF, 0xFF}},
    {0xEF, 6, 0, {0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F}},
    {0xFF, 5, 0, {0x77, 0x01, 0x00, 0x00, 0x13}},
    {0xEF, 1, 0, {0x08}},
    {0xFF, 5, 0, {0x77, 0x01, 0x00, 0x00, 0x00}},
    {0x36, 1, 0, {0x00}},
    {0x3A, 1, 0, {0x66}},
    {0x11, 0, 480, {}},  // Sleep Out
    {0x20, 0, 120, {}},  // Display Inversion Off
    {0x29, 0, 0, {}},    // Display On
};

void panelSendInitSequence() {
  for (const PanelCommand &entry : PANEL_INIT) {
    writeCommand(entry.command);
    for (uint8_t i = 0; i < entry.length; ++i) writeData(entry.data[i]);
    if (entry.delayMs) vTaskDelay(pdMS_TO_TICKS(entry.delayMs));
  }
}

void startRgbPanel() {
  esp_lcd_rgb_panel_config_t rgbConfig = {
      .clk_src = LCD_CLK_SRC_DEFAULT,
      .timings =
          {
              .pclk_hz = LCD_RGB_TIMING_FREQ_HZ,
              .h_res = LCD_WIDTH,
              .v_res = LCD_HEIGHT,
              .hsync_pulse_width = LCD_RGB_TIMING_HPW,
              .hsync_back_porch = LCD_RGB_TIMING_HBP,
              .hsync_front_porch = LCD_RGB_TIMING_HFP,
              .vsync_pulse_width = LCD_RGB_TIMING_VPW,
              .vsync_back_porch = LCD_RGB_TIMING_VBP,
              .vsync_front_porch = LCD_RGB_TIMING_VFP,
              .flags =
                  {
                      .hsync_idle_low = 0,
                      .vsync_idle_low = 0,
                      .de_idle_high = 0,
                      .pclk_active_neg = false,
                      .pclk_idle_high = 0,
                  },
          },
      .data_width = LCD_RGB_DATA_WIDTH,
      .bits_per_pixel = LCD_RGB_PIXEL_BITS,
      .num_fbs = LCD_RGB_FRAME_BUF_NUM,
      .bounce_buffer_size_px = LCD_RGB_BOUNCE_BUF_SIZE,
      .psram_trans_align = 64,
      .hsync_gpio_num = LCD_PIN_NUM_RGB_HSYNC,
      .vsync_gpio_num = LCD_PIN_NUM_RGB_VSYNC,
      .de_gpio_num = LCD_PIN_NUM_RGB_DE,
      .pclk_gpio_num = LCD_PIN_NUM_RGB_PCLK,
      .disp_gpio_num = LCD_PIN_NUM_RGB_DISP,
      .data_gpio_nums =
          {
              LCD_PIN_NUM_RGB_DATA0, LCD_PIN_NUM_RGB_DATA1,
              LCD_PIN_NUM_RGB_DATA2, LCD_PIN_NUM_RGB_DATA3,
              LCD_PIN_NUM_RGB_DATA4, LCD_PIN_NUM_RGB_DATA5,
              LCD_PIN_NUM_RGB_DATA6, LCD_PIN_NUM_RGB_DATA7,
              LCD_PIN_NUM_RGB_DATA8, LCD_PIN_NUM_RGB_DATA9,
              LCD_PIN_NUM_RGB_DATA10, LCD_PIN_NUM_RGB_DATA11,
              LCD_PIN_NUM_RGB_DATA12, LCD_PIN_NUM_RGB_DATA13,
              LCD_PIN_NUM_RGB_DATA14, LCD_PIN_NUM_RGB_DATA15,
          },
      .flags =
          {
              .disp_active_low = 0,
              .refresh_on_demand = 0,
              .fb_in_psram = true,
              .double_fb = false,
              .no_fb = 0,
              .bb_invalidate_cache = 0,
          },
  };

  ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&rgbConfig, &panel_handle));
  esp_lcd_panel_reset(panel_handle);
  esp_lcd_panel_init(panel_handle);

  void *frame = nullptr;
  ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 1, &frame));
  framebuffer = static_cast<uint16_t *>(frame);
  // Panel startuje s nahodnym obsahem PSRAM, hned ho tedy zhasneme.
  memset(framebuffer, 0, static_cast<size_t>(LCD_WIDTH) * LCD_HEIGHT *
                             sizeof(uint16_t));
}

void st7701Init() {
  spi_bus_config_t busConfig = {
      .mosi_io_num = LCD_MOSI_PIN,
      .miso_io_num = -1,
      .sclk_io_num = LCD_CLK_PIN,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 64,
  };
  spi_bus_initialize(SPI2_HOST, &busConfig, SPI_DMA_CH_AUTO);

  spi_device_interface_config_t deviceConfig = {
      .command_bits = 1,
      .address_bits = 8,
      .mode = SPI_MODE0,
      .clock_speed_hz = 40000000,
      .spics_io_num = -1,
      .queue_size = 1,
  };
  spi_bus_add_device(SPI2_HOST, &deviceConfig, &spiHandle);

  chipSelectEnable();
  panelSendInitSequence();
  chipSelectDisable();

  startRgbPanel();
}
}  // namespace

void LCD_Init() {
  panelReset();
  st7701Init();
  Touch_Init();
  Backlight_Init();
}

void LCD_Resync() {
  if (panel_handle != nullptr) esp_lcd_rgb_panel_restart(panel_handle);
}

uint16_t *LCD_GetFramebuffer() { return framebuffer; }

uint8_t LCD_Backlight = 90;

void Backlight_Init() {
  ledcAttach(LCD_BACKLIGHT_PIN, BACKLIGHT_FREQUENCY, BACKLIGHT_RESOLUTION);
  Set_Backlight(LCD_Backlight);
}

void Set_Backlight(uint8_t light) {
  if (light > BACKLIGHT_MAX) light = BACKLIGHT_MAX;
  LCD_Backlight = light;
  uint32_t duty = light * 10;
  if (duty == 1000) duty = 1024;
  ledcWrite(LCD_BACKLIGHT_PIN, duty);
}
