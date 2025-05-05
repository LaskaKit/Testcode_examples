/* Display test for Good Display GDEY075T7
 * example from bb_epaper library is used https://github.com/bitbank2/bb_epaper 
 * 
 * Board:   LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display: Good Display GDEY075T7          https://www.laskakit.cz/good-display-gdey075t7-7-5--800x480-epaper-displej-grayscale/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */
 
//
// Example sketch showing how to display uncompressed and compressed images without a backbuffer
// written by Larry Bank (bitbank@pobox.com)
//
#include <bb_epaper.h>
#include "bomb.h" // uncompressed 40x40 image
#include "bart.h" // compressed 128x64 image
#include "smiley.h" // 100x100 compressed image

BBEPAPER bbep(EP75_800x480);
//#define ESPink_V2     //for version v2.6 and earlier
#define ESPink_V3     //for version v3.0 and above

#ifdef ESPink_V2
#define MOSI_PIN  23
#define CS_PIN    5
#define CLK_PIN   18
#define DC_PIN    17 
#define RESET_PIN 16  
#define BUSY_PIN  4 
#define POWER_PIN 2

#elif defined ESPink_V3
#define MOSI_PIN  11
#define CS_PIN    10
#define CLK_PIN   12
#define DC_PIN    48 
#define RESET_PIN 45  
#define BUSY_PIN  36 
#define POWER_PIN 47
#endif

void setup()
{
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);

  bbep.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN, MOSI_PIN, CLK_PIN, 8000000);
  // Do everything with no local buffer
  bbep.fillScreen(BBEP_WHITE);
  bbep.setCursor(0,4); // start a few pixels away from the top edge
  bbep.setFont(FONT_8x8); // use the internal 8x8 font
  bbep.setTextColor(BBEP_BLACK);
  bbep.println("   bb_epaper");
  bbep.println("Icons and Images");
  // without a backbuffer, bb_epaper can only draw graphics in the native memory direction
  for (int x=0; x<=80; x += 40) {
      // draw 3x uncompressed icon at x,24
      bbep.drawSprite(ucBombMask, SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_PITCH, x, 24, BBEP_BLACK);
  }
  for (int y=80; y<272; y+=64) {
      bbep.loadG5Image(bart, 0, y, BBEP_BLACK, BBEP_WHITE); // draw 3x 128x64 compressed Bart image at 0,y
  }
  bbep.refresh(REFRESH_FULL);
  bbep.wait();
  delay(3000);
  // Scaled drawing requires a back buffer
  bbep.allocBuffer();
  bbep.setRotation(270);
  bbep.fillScreen(BBEP_WHITE);
  bbep.setFont(FONT_12x16);
  bbep.setCursor(0,0);
  bbep.print("Scaled G5 needs RAM");
  float f = 0.5;
  for (int i=0; i<5; i++) {
    bbep.loadG5Image(smiley, (int)(i*50), 16+(int)(i*12.5*f), BBEP_TRANSPARENT, BBEP_BLACK, f);
    f += 0.25;
  }
  bbep.writePlane();
  bbep.refresh(REFRESH_FULL);
  bbep.wait();
  bbep.sleep(DEEP_SLEEP);
}

void loop()
{
}
