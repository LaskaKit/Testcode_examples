/* Display test with touch for 3.5" 320x480 TFT display ILI9488, shield Arduino Uno
 * 
 * Board:   LaskaKit UNO R3, ATmega328P                   https://www.laskakit.cz/arduino-uno-r3--atmega328p--precizni-klon/
      or:   Arduino Uno rev3, original                    https://www.laskakit.cz/arduino-uno-rev3--original/
      or:   LaskaKit Mega2560 rev3                        https://www.laskakit.cz/arduino-mega2560/
 * Display: 3.5" 320x480 TFT display ILI9488, shield Arduino Uno  https://www.laskakit.cz/3-5--320x480-tft-displej-ili9488--shield-arduino-uno
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */
// Requires library https://github.com/prenticedavid/MCUFRIEND_kbv
//              and https://github.com/adafruit/Adafruit_TouchScreen

#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft; 

#define YP A3 
#define XM A2 
#define YM 9 
#define XP 8 
#define TS_MINX 100 
#define TS_MAXX 920 
#define TS_MINY 70 
#define TS_MAXY 920 
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); 

void setup() { 
  tft.begin(0x9488); 
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK); 
  for (int i = 0; i < 10; i++) { 
    int randomX = random(20, tft.width() - 20); 
    int randomY = random(20, tft.height() - 20); 
    tft.fillCircle(randomX, randomY, 10, TFT_WHITE); // Průměr 20px, proto poloměr je 10px 
  } 
} 
void loop() {
  TSPoint p = ts.getPoint(); 
  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT); 
  if (p.z > ts.pressureThreshhold) { 
    int x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0); 
    int y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height()); 
    tft.fillCircle(x, y, 10, TFT_YELLOW); // Když se dotknete, vykreslí žlutou tečku 
  } delay(100); // Krátká prodleva pro debouncing 
}