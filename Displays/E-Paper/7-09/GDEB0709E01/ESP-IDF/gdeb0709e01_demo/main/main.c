#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "GDEB0709E01.h"
#include "image.h"

// 0: keep the demo image on the panel.
// 1: display the image, wait, then clear the panel to white.
#define CLEAR_TO_WHITE_AFTER_IMAGE 0
#define CLEAR_DELAY_MS 10000

static const char *TAG = "demo";

void app_main(void)
{
    ESP_LOGI(TAG, "=== GDEB0709E01 ESP-IDF Demo ===");
    ESP_LOGI(TAG, "Panel: %ux%u, image bytes: %u",
             GDEB0709E01_WIDTH,
             GDEB0709E01_HEIGHT,
             GDEB0709E01_IMAGE_SIZE);

    ESP_ERROR_CHECK(gdeb0709e01_init());

    ESP_LOGI(TAG, "Display image from image.h...");
    ESP_ERROR_CHECK(gdeb0709e01_display(gImage));

#if CLEAR_TO_WHITE_AFTER_IMAGE
    vTaskDelay(pdMS_TO_TICKS(CLEAR_DELAY_MS));
    ESP_LOGI(TAG, "Clear to white...");
    ESP_ERROR_CHECK(gdeb0709e01_init());
    ESP_ERROR_CHECK(gdeb0709e01_clear(GDEB0709E01_WHITE));
#endif

    ESP_ERROR_CHECK(gdeb0709e01_sleep());
    ESP_LOGI(TAG, "Demo finished.");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
