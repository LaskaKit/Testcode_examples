/* Display test with bitmaps for Good Display GDEY075T7
 * example from GxEPD2 library is used
 * 
 * Board:   LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display: Good Display GDEY075T7          https://www.laskakit.cz/good-display-gdey075t7-7-5--800x480-epaper-displej-grayscale/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// Display Library example for SPI e-paper panels from Dalian Good Display.
// Requires Adafruit_GFX and https://github.com/ZinggJM/GxEPD2 Author: Jean-Marc Zingg
// Display Library based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html

#define ENABLE_GxEPD2_GFX 1

#include <GxEPD2_BW.h>
#include "bitmaps/Bitmaps800x480.h" // 7.5"  b/w

//#define ESPink_V2     //for version v2.6 and earlier
#define ESPink_V3     //for version v3.0 and above

#ifdef ESPink_V2
  //MOSI/SDI    23
  //CLK/SCK     18
  //SS/CS       5
  #define DC    17 
  #define RST   16  
  #define BUSY  4 
  #define POWER 2
#else ESPink_V3
  //MOSI/SDI    11
  //CLK/SCK     12
  //SS/CS       10
  #define DC    48 
  #define RST   45  
  #define BUSY  36 
  #define POWER 47
#endif

#define SLEEP_SEC 15         // Measurement interval (seconds)

GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(SS, DC, RST, BUSY)); // GDEY075T7 800x480, GDEW075T7, Waveshare 7.5"

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);

// turn on power to display
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);   // turn the Display Power on (HIGH is the voltage level)
  Serial.println("Display power ON");
  delay(1000);   
  
  display.init(); // inicializace

  display.fillScreen(GxEPD_WHITE);
  delay(1000);

  display.setRotation(0);
  drawBitmaps800x480();

  display.powerOff();
  Serial.println("setup done");
}

void loop() {
}

void drawBitmaps800x480() {
  const unsigned char* bitmaps[] =  {
    Bitmap800x480_1, Bitmap800x480_2, Bitmap800x480_3, Bitmap800x480_4
  };

  if ((display.epd2.WIDTH == 800) && (display.epd2.HEIGHT == 480)) {
    for (uint16_t i = 0; i < sizeof(bitmaps) / sizeof(char*); i++) {
      display.firstPage();
      do {
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0, 0, bitmaps[i], 800, 480, GxEPD_BLACK);
      }
      while (display.nextPage());
      delay(2000);
    }
    if (display.epd2.panel == GxEPD2::GDEW075T7) {
      // avoid ghosting caused by OTP waveform
      display.clearScreen();
      display.refresh(false); // full update
    }
  }
}