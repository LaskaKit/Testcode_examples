
/* Display test for Good Display GDEY0154Z90
 * 
 * Board:   LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display: Good Display GDEY0154Z90        https://www.laskakit.cz/good-display-gdey0154z90-1-54--200x200-cerveny-epaper-displej/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_3C.h>
#include "Ap_29demo.h" // 1.54" b/w/r

#define SLEEP_SEC 15         // Measurement interval (seconds)

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

GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(SS, DC, RST, BUSY)); // GDEH0154Z90 200x200, SSD1681

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);

// turn on power to display
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("Display power ON");
  delay(1000);   
  
  display.init(); // inicializace
  
  drawBitmaps3c200x200();
  Serial.println("draw done");

  delay(1000);

  display.powerOff();
  Serial.println("powerOff done");

  delay(1000);
  Serial.println("All done");

}

void loop(){}

struct bitmap_pair
{
  const unsigned char* black;
  const unsigned char* red;
};

void drawBitmaps3c200x200()
{
  display.setFullWindow();
  bitmap_pair bitmap_pairs[] =
  {
    {gImage_BW1, gImage_RW1}
  };
  if (display.epd2.hasColor)
  {
    display.clearScreen(); // use default for white
    int16_t x = (int16_t(display.epd2.WIDTH) - 200) / 2;
    int16_t y = (int16_t(display.epd2.HEIGHT) - 200) / 2;
    for (uint16_t i = 0; i < sizeof(bitmap_pairs) / sizeof(bitmap_pair); i++)
    {
      display.drawImage(bitmap_pairs[i].black, bitmap_pairs[i].red, x, y, 200, 200, false, true, true);
      delay(2000);
    }
 
    //display.refresh();
    delay(2000);
  }
}
