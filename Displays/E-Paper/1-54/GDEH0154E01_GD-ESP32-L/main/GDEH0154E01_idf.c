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

static const char *TAG = "[EPD]";

// EPD resolution: 240x240, 4bpp packed (2 pixels per byte)
static const uint16_t EPD_WIDTH = 240;
static const uint16_t EPD_HEIGHT = 240;
static const uint32_t EPD_FRAME_BYTES = (EPD_WIDTH * EPD_HEIGHT) / 2;

// Pin definitions
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

// Color byte values
typedef enum {
    COLOR_BLACK = 0x00,
    COLOR_WHITE = 0x11,
    COLOR_YELLOW = 0x22,
    COLOR_RED = 0x33,
    COLOR_BLUE = 0x55,
    COLOR_GREEN = 0x66,
} EpdColorByte;

// BUSY pin polarity
static bool gBusyActiveLevelLow = true;
static bool gIgnoreBusy = false;

// Water ripple settings
static const bool USE_WATER_RIPPLE_INIT = false;

typedef enum {
    WATER_RIPPLE_FACTORY = 0,
    WATER_RIPPLE_NEW = 1,
} WaterRippleProfile;

static const WaterRippleProfile ACTIVE_WATER_RIPPLE_PROFILE __attribute__((unused)) = WATER_RIPPLE_NEW;

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
    PIN_EPD_BUSY = GPIO_NUM_14;   // A14
    PIN_EPD_RST = GPIO_NUM_15;    // A15
    PIN_EPD_DC = GPIO_NUM_16;     // A16
    PIN_EPD_CS = GPIO_NUM_17;     // A17
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

// ====================== BUSY 极性检测 ======================
void epd_try_fix_busy_polarity() {
    if (epd_wait_busy(3000)) {
        ESP_LOGI(TAG, "busy polarity ok, activeLow=%d", gBusyActiveLevelLow ? 1 : 0);
        return;
    }

    gBusyActiveLevelLow = !gBusyActiveLevelLow;
    if (epd_wait_busy(3000)) {
        ESP_LOGI(TAG, "busy polarity switched, activeLow=%d", gBusyActiveLevelLow ? 1 : 0);
    } else {
        ESP_LOGW(TAG, "busy pin still not ready, check BUSY wire and panel power");
        ESP_LOGW(TAG, "fallback: ignore BUSY and continue");
        gIgnoreBusy = true;
    }
}

// ====================== 复位 ======================
void epd_reset() {
    gpio_set_level(PIN_EPD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(PIN_EPD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(20));
    epd_wait_busy(BUSY_TIMEOUT_INIT_MS);
    vTaskDelay(pdMS_TO_TICKS(10));
}

// ====================== 初始化函数 ======================
void epd_init_jd7601() {
    epd_write_command(0xE9);
    epd_write_data(0x01);
}

void epd_init_jd7601_water_ripple_v2() {
    epd_init_jd7601();

    epd_write_command(0xFF);
    epd_write_data(0xA5);  // 测试模式

    epd_write_command(0xEB);
    epd_write_data(0x01);

    epd_write_command(0xDA);
    epd_write_data(0x04);

    epd_write_command(0xB4);
    epd_write_data(0x31);

    epd_write_command(0xB0);
    epd_write_data(0x03);

    epd_write_command(0xC0);
    epd_write_data(0x78);

    epd_write_command(0xEF);
    epd_write_data(3);
    epd_write_data(80);
    epd_write_data(6);
    epd_write_data(45);
    epd_write_data(10);
    epd_write_data(36);
    epd_write_data(14);
    epd_write_data(8);
    epd_write_data(18);
    epd_write_data(4);

    epd_write_command(0xDC);
    epd_write_data(0x01);

    epd_write_command(0xDD);
    epd_write_data(6);
    epd_write_data(18);

    epd_write_command(0xDE);
    epd_write_data(4);
    epd_write_data(10);
    epd_write_data(16);
    epd_write_data(22);
    epd_write_data(28);

    epd_write_command(0xFF);
    epd_write_data(0xE3);  // 退出测试模式
}

void epd_enter_deep_sleep() {
    epd_write_command(0x07);
    epd_write_data(0xA5);
}

// ====================== 显示函数 ======================
void epd_display_solid(EpdColorByte color) {
    ESP_LOGI(TAG, "stage: write frame");
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

    for (uint32_t i = 0; i < data_len; ++i) {
        epd_write_data(img_data[i]);
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
void epd_hardware_init() {
    init_pins();

    // 配置 GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_EPD_BUSY) | (1ULL << PIN_EPD_DC) |
                        (1ULL << PIN_EPD_CS) | (1ULL << PIN_EPD_RST),
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

    // 重置并初始化
    epd_reset();
    epd_try_fix_busy_polarity();

    if (USE_WATER_RIPPLE_INIT) {
        ESP_LOGI(TAG, "init: water-ripple mode");
        epd_init_jd7601_water_ripple_v2();
    } else {
        ESP_LOGI(TAG, "init: normal mode");
        epd_init_jd7601();
    }
    ESP_LOGI(TAG, "init done");

    // 显示图片
    ESP_LOGI(TAG, "displaying image...");
    epd_display_image(image_data_sixcolor, sizeof(image_data_sixcolor));
    vTaskDelay(pdMS_TO_TICKS(3000));

    // 刷白
    ESP_LOGI(TAG, "clearing screen to white...");
    epd_display_solid(COLOR_WHITE);

    // 进入深睡眠
    ESP_LOGI(TAG, "entering deep sleep");
    epd_enter_deep_sleep();

    // 任务完成，删除自己
    ESP_LOGI(TAG, "EPD task completed");
    vTaskDelete(NULL);
}

// ====================== 主程序入口 ======================
void app_main() {
    ESP_LOGI(TAG, "Starting EPD application");
    
    // 创建 EPD 任务（优先级为 5，栈大小 4096）
    xTaskCreate(epd_task, "epd_task", 4096, NULL, 5, NULL);
}
