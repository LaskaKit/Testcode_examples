/*
  Display test for 2.25" 76x284 IPS ST7789 (SPI)
  Boards:  LaskaKit ESP32-S3-DEVKit (v3.0+)
           LaskaKit ESP32-DEVKit   (v2.6 a starší)
  Display: 2.25" 76x284 ST7789 (SPI)  https://www.laskakit.cz/2-25--76x284-tft-barevny-displej-st7789--spi/
  Board links:
    - ESP32-S3-DEVKit: https://www.laskakit.cz/laskakit-esp32-s3-devkit
    - ESP32-DEVKit:    https://www.laskakit.cz/laskakit-esp32-devkit

  Support: podpora@laskakit.cz
  Web:     laskakit.cz
*/

// =============== USER CONFIG ===============
#define ESP32_S3_DEVKIT     // odkomentujte správnou desku: ESP32_S3_DEVKIT nebo ESP32_DEVKIT
// #define ESP32_DEVKIT

// pokud má váš modul připojený CS, zapněte toto:
#define USE_TFT_CS          // nebo zakomentujte pro variantu bez CS

// zvolte orientaci: 0..3
#define TFT_ROTATION 1
// ==========================================

#include <Adafruit_GFX.h>
#include <Arduino_ST7789.h>
#include <SPI.h>

// --- Pinout podle desky ---
#if defined(ESP32_DEVKIT)
// ESP32 DEVKit (v2.6 a starší)
  #define TFT_MOSI  23
  #define TFT_SCLK  18
  #define TFT_DC    15
  #define TFT_RST   12
  #define POWER      2      // uSUP enable / power switch
  #if defined(USE_TFT_CS)
    #define TFT_CS   5
  #endif

#elif defined(ESP32_S3_DEVKIT)
// ESP32-S3 DEVKit (v3.0 a novější)
  #define TFT_MOSI  11
  #define TFT_SCLK  12
  #define TFT_DC    10
  #define TFT_RST   13
  #define POWER     47      // uSUP enable / power switch
  #if defined(USE_TFT_CS)
    #define TFT_CS  40
  #endif

#else
  #error "Zvolte desku: definujte ESP32_S3_DEVKIT nebo ESP32_DEVKIT."
#endif

// --- Konstruktor ST7789 ---
#if defined(USE_TFT_CS)
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_CS); // s CS
#else
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);         // bez CS
#endif

// Pi jen pro výpis
float p = 3.1415926;

// Prototypy
void testlines(uint16_t color);
void testdrawtext(const char *text, uint16_t color);
void testfastlines(uint16_t color1, uint16_t color2);
void testdrawrects(uint16_t color);
void testfillrects(uint16_t color1, uint16_t color2);
void testfillcircles(uint8_t radius, uint16_t color);
void testdrawcircles(uint8_t radius, uint16_t color);
void testtriangles();
void testroundrects();
void tftPrintTest();
void mediabuttons();

void setup() {
  Serial.begin(115200);
  Serial.println("\nHello! ST7789 TFT Test");

  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);     // zapnout napájení uSUP
  Serial.println("Display power ON");
  delay(300);

  // Inicializace panelu 76x284
    tft.init(76, 284);
  tft.setRotation(1);   // zkoušejte 1 nebo 3
  tft.setAddrWindow(52, 0, 76, 284);  // případně upravte 52 → 26 nebo 0

  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  tft.println("LaskaKit");

  Serial.println("Initialized");

  uint32_t time = millis();
  tft.fillScreen(BLACK);
  time = millis() - time;
  Serial.print("Fill BLACK took: "); Serial.println(time);

  delay(400);

  // textový test
  tft.fillScreen(BLACK);
  testdrawtext(
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
    "Curabitur adipiscing ante sed nibh tincidunt feugiat.", WHITE
  );
  delay(800);

  // tiskové testy
  tftPrintTest();
  delay(1200);

  // středový pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, GREEN);
  delay(300);

  // testy čar a tvarů
  testlines(YELLOW);          delay(300);
  testfastlines(RED, BLUE);   delay(300);
  testdrawrects(GREEN);       delay(300);
  testfillrects(YELLOW, MAGENTA); delay(300);

  tft.fillScreen(BLACK);
  testfillcircles(10, BLUE);
  testdrawcircles(10, WHITE); delay(300);

  testroundrects();           delay(300);
  testtriangles();            delay(300);
  mediabuttons();             delay(300);

  Serial.println("done");
  delay(500);
}

void loop() {
  tft.invertDisplay(true);
  delay(400);
  tft.invertDisplay(false);
  delay(400);
}

void testlines(uint16_t color) {
  tft.fillScreen(BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
  }

  tft.fillScreen(BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
  }

  tft.fillScreen(BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
  }

  tft.fillScreen(BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
  }
}

void testdrawtext(const char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(BLACK);
  for (int16_t y = 0; y < tft.height(); y += 5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x = 0; x < tft.width(); x += 5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawRect(tft.width()/2 - x/2, tft.height()/2 - x/2, x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(BLACK);
  for (int16_t x = tft.width()-1; x > 6; x -= 6) {
    tft.fillRect(tft.width()/2 - x/2, tft.height()/2 - x/2, x, x, color1);
    tft.drawRect(tft.width()/2 - x/2, tft.height()/2 - x/2, x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = radius; x < tft.width(); x += radius*2) {
    for (int16_t y = radius; y < tft.height(); y += radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = 0; x < tft.width()+radius; x += radius*2) {
    for (int16_t y = 0; y < tft.height()+radius; y += radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(BLACK);
  int color = 0xF800;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for (int t = 0; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x -= 4; y += 4; z -= 4; color += 100;
  }
}

void testroundrects() {
  tft.fillScreen(BLACK);
  int color = 100;
  for (int t = 0; t <= 4; t++) {
    int x = 0, y = 0;
    int w = tft.width()-2, h = tft.height()-2;
    for (int i = 0; i <= 16; i++) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x += 2; y += 3; w -= 4; h -= 6; color += 1100;
    }
    color += 100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(RED);    tft.setTextSize(1); tft.println("Hello World!");
  tft.setTextColor(YELLOW); tft.setTextSize(2); tft.println("Hello World!");
  tft.setTextColor(GREEN);  tft.setTextSize(3); tft.println("Hello World!");
  tft.setTextColor(BLUE);   tft.setTextSize(4); tft.print(1234.567);
  delay(1000);

  tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);  tft.setTextSize(1); tft.println("Hello World!");
  tft.setTextColor(GREEN);  tft.print(p, 6); tft.println(" Want pi?\n");
  tft.setTextColor(WHITE);  tft.println("Sketch has been"); tft.println("running for:");
  tft.setTextColor(MAGENTA); tft.print(millis()/1000);
  tft.setTextColor(WHITE);   tft.println(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, RED);
  delay(300);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, GREEN);
  delay(300);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, BLUE);  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, GREEN);
}