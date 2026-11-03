#ifndef GDEB0709E01_H
#define GDEB0709E01_H

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#define GDEB0709E01_WIDTH         1200
#define GDEB0709E01_HEIGHT        1600
#define GDEB0709E01_BYTES_PER_ROW 600
#define GDEB0709E01_SIDE_BYTES    300
#define GDEB0709E01_IMAGE_SIZE    (GDEB0709E01_WIDTH * GDEB0709E01_HEIGHT / 2)

#define GDEB0709E01_BLACK   0x0
#define GDEB0709E01_WHITE   0x1
#define GDEB0709E01_YELLOW  0x2
#define GDEB0709E01_RED     0x3
#define GDEB0709E01_BLUE    0x5
#define GDEB0709E01_GREEN   0x6

esp_err_t gdeb0709e01_init(void);
esp_err_t gdeb0709e01_display(const uint8_t *image);
esp_err_t gdeb0709e01_clear(uint8_t color);
esp_err_t gdeb0709e01_sleep(void);

#endif
