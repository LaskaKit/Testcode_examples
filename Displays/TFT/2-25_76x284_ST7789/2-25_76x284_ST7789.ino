/* 
 * Display only test for 2.25" 76x284 TFT ST7789, SPI https://www.laskakit.cz/2-25--76x284-tft-barevny-displej-st7789--spi/
 * with LaskaKit ESP32-S3-DEVKit                      https://www.laskakit.cz/laskakit-esp32-s3-devkit/
 * 
 * How to steps:
 * 1. Copy Setup304_ST7789_2-25_LaskaKit_ESP32-S3-DevKit.h to Arduino/libraries/TFT_eSPI/User_Setups/
 *
 * 2. in Arduino/libraries/TFT_eSPI/User_Setup_Select.h 
      a. comment: #include <User_Setup.h> 
      b. add:  #include <User_Setups/Setup304_ST7789_2-25_LaskaKit_ESP32-S3-DevKit.h>  // Setup file for LaskaKit ESP32-S3-DevKit with 2.25 76x284, ST7789
 * 3. Replace file ST7789_Rotation.h in /Arduino/libraries/TFT_eSPI/TFT_Drivers with file in this folder ()
      added: (_init_width == 76 && _init_height == 284)
        colstart = 18;
        rowstart = 82;
 * 
 *
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
*/

#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial.println("LaskaKit ST7789P3 76x284 init...");

  pinMode(PW_ON, OUTPUT);
  digitalWrite(PW_ON, HIGH);         // Power ON

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);        // Backlight ON

  delay(50);
  tft.init();
  tft.setRotation(1); // or 3
  tft.invertDisplay(false);
  tft.fillScreen(TFT_BLACK);

  int h = tft.height();
  int w = tft.width();
  int h3 = h / 3;

  // Třetiny výšky – vždy 3 pruhy
  tft.fillRect(0, 0,      w, h3,     TFT_RED);
  tft.fillRect(0, h3,     w, h3,     TFT_GREEN);
  tft.fillRect(0, 2*h3,   w, h - 2*h3, TFT_BLUE);

  // Okraje pro kontrolu mapování
  tft.drawFastVLine(0, 0, h, TFT_WHITE);
  tft.drawFastVLine(w-1, 0, h, TFT_WHITE);
  tft.drawFastHLine(0, 0, w, TFT_WHITE);
  tft.drawFastHLine(0, h-1, w, TFT_WHITE);
  tft.setTextColor(TFT_WHITE); tft.setTextSize(1);
  tft.setCursor(4, 4); tft.print("LaskaKit 76x284");
}

void loop() {
}