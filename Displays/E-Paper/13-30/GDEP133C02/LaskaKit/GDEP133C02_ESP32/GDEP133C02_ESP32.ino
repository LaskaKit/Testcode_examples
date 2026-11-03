/* Display test for Good Display GDEP133C02 (13.3" Spectra 6, 1200x1600, 6 colours)
 * Ported to Arduino IDE from the Good Display GDEP133C02 ESP32 example (2026-04-20),
 * register values and refresh sequence are 1:1 from that example.
 *
 * Board:   Good Display ESP32-133C02  https://www.laskakit.cz/good-display-esp32-133c02-esp32-s3-vyvojova-deska-pro-qspi-e6-series-color-e-paper/
 *          or LaskaKit E6 adapter (set the pins in Display_EPD_W21_spi.h)
 * Display: Good Display GDEP133C02    https://www.laskakit.cz/good-display-gdep133c02-13-3--1600x1200-qspi-epaper-barevny-displej/
 *
 * Arduino IDE settings (ESP32-S3):
 *   Board:            ESP32S3 Dev Module
 *   Flash Size:       16MB (ESP32-133C02 uses ESP32-S3-WROOM-1-N16R8)
 *   Partition Scheme: 16M Flash (3MB APP/9.9MB FATFS) or Huge APP - image.h alone has 960 kB
 *   USB CDC On Boot:  Enabled (to see Serial over USB-C)
 *
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// ! For the board settings see Display_EPD_W21_spi.h

#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "image.h"     // gImage[960000] - picture from Good Display example

#define PAUSE_MS 5000  // pause between refreshes

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nGDEP133C02 test");

  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);   // turn the display power on
  delay(100);

  pinMode(BUSY, INPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
  pinMode(CS_M, OUTPUT);
  pinMode(CS_S, OUTPUT);
  EPD_CS_All(HIGH);
#if DC >= 0
  pinMode(DC, OUTPUT);
#endif

  SPI_Init();
}

//Tips//
/*
1.Spectra 6 full refresh takes ~20-30 s at 25 C, flickering is normal.
2.Re-initialization (EPD_init) is required before every full screen update.
3.Do not plug/unplug the FPC while the board is powered.
4.Keep the refresh interval recommended by the manufacturer for production use.
*/
void loop() {
  EPD_init();
  EPD_check_driver_ICs();      // prints status of master/slave driver IC (0xF2)

#if 1 // 6 colour bars: black, white, yellow, red, blue, green
  Serial.println("Color bar");
  Display_Color_Bar();
  delay(PAUSE_MS);
#endif

#if 1 // picture from Good Display example
  Serial.println("Picture");
  EPD_init();
  PIC_display(gImage);
  delay(PAUSE_MS);
#endif

#if 1 // 6x8 checkerboard of all colours
  Serial.println("Checkerboard");
  EPD_init();
  Display_Checkerboard();
  delay(PAUSE_MS);
#endif

#if 1 // 6 vertical bars - every row has all colours (compare green with horizontal bars)
  Serial.println("Vertical bars");
  EPD_init();
  Display_Vertical_Bars();
  delay(PAUSE_MS);
#endif

#if 0 // single colours, change 0 to 1 to enable
  const unsigned char cols[6] = {black, white, yellow, red, blue, green};
  for (int i = 0; i < 6; i++) {
    EPD_init();
    Display_All(cols[i]);
    delay(PAUSE_MS);
  }
#endif

#if 0 // clear to white at the end, change 0 to 1 to enable
  EPD_init();
  Display_All_White();
#endif

  EPD_sleep();                 // panel power off, image stays on the display
  Serial.println("Done");
  while (1) delay(1000);       // the program stops here
}

//////////////////////////////////END//////////////////////////////////////////////////
