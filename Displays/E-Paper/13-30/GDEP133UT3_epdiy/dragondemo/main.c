/* Simple firmware for a ESP32 displaying a static image on an EPaper Screen */

#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dragon.h"
#include "epd_highlevel.h"
#include "epdiy.h"

EpdiyHighlevelState hl;

// choose the default demo board depending on the architecture
#ifdef CONFIG_IDF_TARGET_ESP32
#define DEMO_BOARD epd_board_v5
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define DEMO_BOARD epd_board_v7
#endif

void idf_loop() {
    EpdRect dragon_area = {.x = 0, .y = 0, .width = dragon_width, .height = dragon_height};

    int temperature = 22;

    epd_poweron();
    epd_fullclear(&hl, temperature);

    epd_copy_to_framebuffer(dragon_area, dragon_data, epd_hl_get_framebuffer(&hl));

    enum EpdDrawError _err = epd_hl_update_screen(&hl, MODE_GC16, temperature);
    epd_poweroff();

    vTaskDelay(1000);
}

void idf_setup() {
    // Choose the board (defined automaticaly beetween )
    // Set display with waveform: Library now fully compatible only with this ED133UT2 and not GDEP133UT3 display. 
    // GDEP133UT3 is not fully compatible,  no good waveform and because of it is has grayscale problem )// ED042TC2 ED133UT2
    // You can try use ED047TC2 instead, but it has smaller resolution.
    epd_init(&DEMO_BOARD, &ED133UT2, EPD_LUT_64K); // ED047TC2 or ED133UT2
    epd_set_vcom(2230);
    hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
}

#ifndef ARDUINO_ARCH_ESP32
void app_main() {
    idf_setup();

    while (1) {
        idf_loop();
    };
}
#endif
