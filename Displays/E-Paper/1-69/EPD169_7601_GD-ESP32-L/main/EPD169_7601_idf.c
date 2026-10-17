#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "image.h"

// ====================== 开发板选择 ======================
// 选择你的开发板: 1 = ESP32S3, 0 = ESP32
#define BOARD_ESP32S3 1

// Enable to also run the two diagnostic reads from the original MSP430 code
// (read_temperture / read_Revision). These need the MOSI/SDA line to be
// bidirectional (bit-banged), since the reference hardware used a single
// SDA pin for both write and read. Hardware SPI (used for the normal write
// path below) is MOSI-only (MISO unused), so these two functions temporarily
// bit-bang the MOSI pin directly via plain GPIO. Leave disabled unless you
// specifically need them.
#define ENABLE_DIAG_READ 0

static const char *TAG = "[EPD]";

// Converted from original MSP430 reference: 1D69_E6_400x400_7601_test.c
// Panel: 1.69" e-paper, 400x400, 4bpp packed (2 pixels per byte), driver IC: JD7601/IST7601
static const uint16_t EPD_WIDTH = 400;
static const uint16_t EPD_HEIGHT = 400;
static const uint32_t EPD_FRAME_BYTES = (EPD_WIDTH * EPD_HEIGHT) / 2; // 400*400/2 = 80000

// Pin definitions (identical to GDEH0154E01 project)
static gpio_num_t PIN_EPD_MOSI;
static gpio_num_t PIN_EPD_CLK;
static gpio_num_t PIN_EPD_BUSY;
static gpio_num_t PIN_EPD_DC;
static gpio_num_t PIN_EPD_CS;
static gpio_num_t PIN_EPD_RST;

// SPI device handle
static spi_device_handle_t spi_handle;

// Timeout settings
static const uint32_t BUSY_TIMEOUT_INIT_MS = 15000;
static const uint32_t BUSY_TIMEOUT_POWER_MS = 60000;
static const uint32_t BUSY_TIMEOUT_REFRESH_MS = 120000;

// Color byte values are copied 1:1 from the original MSP430 reference code
// (PIC_BLACK / PIC_WHITE / ... defines in 1D69_E6_400x400_7601_test.c).
typedef enum {
    COLOR_BLACK = 0x00,
    COLOR_WHITE = 0x11,
    COLOR_YELLOW = 0x22,
    COLOR_RED = 0x33,
    COLOR_BLUE = 0x55,
    COLOR_GREEN = 0x66,
} EpdColorByte;

// BUSY pin polarity - matches READBUSY() in reference (active on P1IN bit == 0x04, i.e. low-active line inverted at MCU; treated as low-active here as in the sibling GDEH0154E01 project)
static bool gBusyActiveLevelLow = true;
static bool gIgnoreBusy = false;

// ====================== 初始化引脚定义 ======================
void init_pins() {
#if BOARD_ESP32S3
    // ESP32-S3 pin definition
    PIN_EPD_MOSI = GPIO_NUM_11;
    PIN_EPD_CLK = GPIO_NUM_12;
    PIN_EPD_BUSY = GPIO_NUM_5;
    PIN_EPD_DC = GPIO_NUM_3;
    PIN_EPD_CS = GPIO_NUM_2;
    PIN_EPD_RST = GPIO_NUM_4;
    ESP_LOGI(TAG, "Using ESP32-S3");
#else
    // ESP32 pin definition (using A14, A15, A16, A17)
     // ESP32 pin definition (using A14, A15, A16, A17)
    PIN_EPD_BUSY = GPIO_NUM_13;   // A14
    PIN_EPD_RST = GPIO_NUM_12;    // A15
    PIN_EPD_DC = GPIO_NUM_14;     // A16
    PIN_EPD_CS = GPIO_NUM_27;     // A17
    PIN_EPD_CLK = GPIO_NUM_18;    // SCK (SPI default)
    PIN_EPD_MOSI = GPIO_NUM_23;   // MOSI (SPI default)
    ESP_LOGI(TAG, "Using ESP32");
#endif
}

// ====================== GPIO 操作 ======================
static inline void epd_select() {
    gpio_set_level(PIN_EPD_CS, 0);
}

static inline void epd_deselect() {
    gpio_set_level(PIN_EPD_CS, 1);
}

// ====================== SPI 操作 ======================
void epd_write_command(uint8_t cmd) {
    gpio_set_level(PIN_EPD_DC, 0);  // DC = LOW (command)

    spi_transaction_t trans = {
        .length = 8,
        .tx_buffer = &cmd,
    };

    epd_select();
    spi_device_polling_transmit(spi_handle, &trans);
    epd_deselect();
}

void epd_write_data(uint8_t data) {
    gpio_set_level(PIN_EPD_DC, 1);  // DC = HIGH (data)

    spi_transaction_t trans = {
        .length = 8,
        .tx_buffer = &data,
    };

    epd_select();
    spi_device_polling_transmit(spi_handle, &trans);
    epd_deselect();
}

#if ENABLE_DIAG_READ
// Bit-banged read on the MOSI/SDA pin, mirroring SPI4W_READDATA() in the
// reference code (P1DIR toggled to switch the pin between output/input).
// Hardware SPI is temporarily removed from the pin to bit-bang it directly.
static uint8_t epd_bitbang_read_byte(void) {
    gpio_set_direction(PIN_EPD_MOSI, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_EPD_CLK, GPIO_MODE_OUTPUT);

    uint8_t value = 0;
    epd_select();
    gpio_set_level(PIN_EPD_DC, 1);
    for (int i = 0; i < 8; ++i) {
        gpio_set_level(PIN_EPD_CLK, 1);
        value = (value << 1) | (gpio_get_level(PIN_EPD_MOSI) ? 0x01 : 0x00);
        gpio_set_level(PIN_EPD_CLK, 0);
    }
    epd_deselect();

    gpio_set_direction(PIN_EPD_MOSI, GPIO_MODE_OUTPUT);
    return value;
}

// read_temperture() in reference: writes 0x40, waits ~1s, reads 2 bytes.
void epd_read_temperature(void) {
    epd_write_command(0x40);
    vTaskDelay(pdMS_TO_TICKS(1000));
    uint8_t t1 = epd_bitbang_read_byte();
    uint8_t t2 = epd_bitbang_read_byte();
    ESP_LOGI(TAG, "temperature raw bytes: 0x%02X 0x%02X", t1, t2);
}

// read_Revision() in reference: writes 0x70, reads chip id + 3 LUT version bytes.
void epd_read_revision(void) {
    epd_write_command(0x70);
    uint8_t chip_id = epd_bitbang_read_byte();
    uint8_t lut0 = epd_bitbang_read_byte();
    uint8_t lut1 = epd_bitbang_read_byte();
    uint8_t lut2 = epd_bitbang_read_byte();
    ESP_LOGI(TAG, "chip id=0x%02X lut_ver=0x%02X%02X%02X", chip_id, lut2, lut1, lut0);
}
#endif // ENABLE_DIAG_READ

// ====================== BUSY 等待 ======================
bool epd_wait_busy(uint32_t timeout_ms) {
    if (gIgnoreBusy) {
        return true;
    }

    uint32_t start = xTaskGetTickCount();
    const int busy_level = gBusyActiveLevelLow ? 0 : 1;

    while (gpio_get_level(PIN_EPD_BUSY) == busy_level) {
        vTaskDelay(pdMS_TO_TICKS(1));
        uint32_t elapsed = (xTaskGetTickCount() - start) * portTICK_PERIOD_MS;
        if (elapsed > timeout_ms) {
            ESP_LOGW(TAG, "wait busy timeout, pin=%d, activeLow=%d",
                     gpio_get_level(PIN_EPD_BUSY), gBusyActiveLevelLow ? 1 : 0);
            return false;
        }
    }
    return true;
}

bool epd_wait_busy_stage(const char *stage, uint32_t timeout_ms) {
    bool ok = epd_wait_busy(timeout_ms);
    if (!ok) {
        ESP_LOGE(TAG, "stage timeout: %s", stage);
    }
    return ok;
}

// ====================== 复位 ======================
// RESET() in reference does two low/high pulses before waiting BUSY - kept
// identical here since it's part of the vendor-verified bring-up sequence.
void epd_reset(void) {
    gpio_set_level(PIN_EPD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(PIN_EPD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_EPD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(PIN_EPD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    epd_wait_busy(BUSY_TIMEOUT_INIT_MS);
    vTaskDelay(pdMS_TO_TICKS(10));
}

// ====================== 初始化函数 ======================
// INIT_IST7601() from reference:
//   enter test cmd (0xFF/0xA5) -> 0xD6/0x00 -> exit test cmd (0xFF/0xE3) -> 0xE9/0x01
void epd_init_ist7601(void) {
    epd_write_command(0xFF);   // Enter Test command
    epd_write_data(0xA5);

    epd_write_command(0xD6);
    epd_write_data(0x00);

    epd_write_command(0xFF);   // Exit Test command
    epd_write_data(0xE3);

    epd_write_command(0xE9);
    epd_write_data(0x01);
}

void epd_enter_deep_sleep(void) {
    epd_write_command(0x07);
    epd_write_data(0xA5);
}

// ====================== 显示函数 ======================
// dis_img(num) in reference only ever wrote a single solid color across the
// whole frame. Kept as-is for parity; use epd_display_image() below to push
// a real image buffer instead.
void epd_display_solid(EpdColorByte color) {
    ESP_LOGI(TAG, "stage: write frame (solid)");
    epd_write_command(0x10);  // DTM1 Write
    epd_wait_busy_stage("DTM1", BUSY_TIMEOUT_INIT_MS);

    for (uint32_t i = 0; i < EPD_FRAME_BYTES; ++i) {
        epd_write_data((uint8_t)color);
    }

    ESP_LOGI(TAG, "stage: power on");
    epd_write_command(0x04);  // Power ON
    epd_wait_busy_stage("Power ON", BUSY_TIMEOUT_POWER_MS);
    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_LOGI(TAG, "stage: refresh");
    epd_write_command(0x12);  // Display Refresh
    epd_write_data(0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    epd_wait_busy_stage("Display Refresh", BUSY_TIMEOUT_REFRESH_MS);

    ESP_LOGI(TAG, "stage: power off");
    epd_write_command(0x02);  // Power OFF
    epd_write_data(0x00);
    epd_wait_busy_stage("Power OFF", BUSY_TIMEOUT_POWER_MS);
    vTaskDelay(pdMS_TO_TICKS(20));
}

void epd_display_image(const unsigned char *img_data, uint32_t data_len) {
    ESP_LOGI(TAG, "stage: write frame (image)");
    epd_write_command(0x10);  // DTM1 Write
    epd_wait_busy_stage("DTM1", BUSY_TIMEOUT_INIT_MS);

    if (data_len != EPD_FRAME_BYTES) {
        ESP_LOGW(TAG, "image length mismatch: dataLen=%u expected=%u", (unsigned)data_len, (unsigned)EPD_FRAME_BYTES);
    }

    for (uint32_t i = 0; i < EPD_FRAME_BYTES; ++i) {
        uint8_t b = 0x11; // default: white
        if (i < data_len) {
            b = img_data[i];
        }
        epd_write_data(b);
    }

    ESP_LOGI(TAG, "stage: power on");
    epd_write_command(0x04);
    epd_wait_busy_stage("Power ON", BUSY_TIMEOUT_POWER_MS);
    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_LOGI(TAG, "stage: refresh");
    epd_write_command(0x12);
    epd_write_data(0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    epd_wait_busy_stage("Display Refresh", BUSY_TIMEOUT_REFRESH_MS);

    ESP_LOGI(TAG, "stage: power off");
    epd_write_command(0x02);
    epd_write_data(0x00);
    epd_wait_busy_stage("Power OFF", BUSY_TIMEOUT_POWER_MS);
    vTaskDelay(pdMS_TO_TICKS(20));
}

// ====================== 硬件初始化 ======================
void epd_hardware_init(void) {
    init_pins();

    // 配置 GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_EPD_DC) | (1ULL << PIN_EPD_CS) | (1ULL << PIN_EPD_RST),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // 配置 BUSY 为输入
    gpio_config_t busy_conf = {
        .pin_bit_mask = 1ULL << PIN_EPD_BUSY,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&busy_conf);

    gpio_set_level(PIN_EPD_CS, 1);
    gpio_set_level(PIN_EPD_DC, 1);
    gpio_set_level(PIN_EPD_RST, 1);

    // 初始化 SPI
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_EPD_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_EPD_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };

    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 128,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 2000000,  // 2MHz
        .input_delay_ns = 0,
        .spics_io_num = -1,  // 手动控制 CS
        .flags = 0,
        .queue_size = 7,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_handle));

    ESP_LOGI(TAG, "Hardware initialized");
}

// ====================== 主任务 ======================
void epd_task(void *arg) {
    ESP_LOGI(TAG, "EPD task started");

    // 硬件初始化
    epd_hardware_init();

    vTaskDelay(pdMS_TO_TICKS(300));

    ESP_LOGI(TAG, "boot, BUSY pin now=%d", gpio_get_level(PIN_EPD_BUSY));

    // 复位 (与参考代码 RESET() 一致：两次低/高脉冲)
    epd_reset();

#if ENABLE_DIAG_READ
    epd_read_temperature();
    epd_reset();
    epd_read_revision();
#endif

    epd_reset();
    epd_wait_busy(BUSY_TIMEOUT_INIT_MS);

    // 初始化 IST7601 (与参考代码 INIT_IST7601() 一致)
    epd_init_ist7601();
    ESP_LOGI(TAG, "init done");

    // 显示图片 (来自 image.h，占位数据；替换为真实图片数据即可)
    ESP_LOGI(TAG, "displaying image...");
    epd_display_image(image_data_sixcolor, sizeof(image_data_sixcolor));
    vTaskDelay(pdMS_TO_TICKS(3000));

    // 参考代码 main() 中最终显示的是纯色 PIC_GREEN，如需复现可改用:
    // epd_display_solid(COLOR_GREEN);

    // 进入深睡眠
    ESP_LOGI(TAG, "entering deep sleep");
    epd_enter_deep_sleep();

    // 任务完成，删除自己
    ESP_LOGI(TAG, "EPD task completed");
    vTaskDelete(NULL);
}

// ====================== 主程序入口 ======================
void app_main(void) {
    ESP_LOGI(TAG, "Starting EPD application");

    // 创建 EPD 任务（优先级为 5，栈大小 4096）
    xTaskCreate(epd_task, "epd_task", 4096, NULL, 5, NULL);
}
