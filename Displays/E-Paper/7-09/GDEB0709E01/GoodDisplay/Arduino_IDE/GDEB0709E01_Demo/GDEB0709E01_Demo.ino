#include <Arduino.h>
#include "DEV_Config.h"
#include "GDEB0709E01.h"
#include "image.h"

// Demo behavior:
// 1. Initialize the panel.
// 2. Display the image stored in image.h.
// 3. Keep the image on the panel.
// Set this to 1 if you want the demo to clear to white after the delay.
#define CLEAR_TO_WHITE_AFTER_IMAGE 0
#define CLEAR_DELAY_MS 10000

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("=== GDEB0709E01 Arduino Demo ===");
    Serial.printf("Panel: %ux%u, image bytes: %u\n",
                  GDEB0709E01_WIDTH,
                  GDEB0709E01_HEIGHT,
                  (unsigned)GDEB0709E01_IMAGE_SIZE);

    DEV_Module_Init();
    GDEB0709E01_Init();

    Serial.println("Display image from image.h...");
    GDEB0709E01_Display(gImage);

#if CLEAR_TO_WHITE_AFTER_IMAGE
    delay(CLEAR_DELAY_MS);
    Serial.println("Clear to white...");
    GDEB0709E01_Init();
    GDEB0709E01_Clear(GDEB0709E01_WHITE);
#endif

    GDEB0709E01_Sleep();
    Serial.println("Demo finished.");
}

void loop()
{
    delay(1000);
}
