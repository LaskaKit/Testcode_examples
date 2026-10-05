/* Display test for Good Display GDEH0576T81
 * example from GxEPD2 library is used
 *
 * Board:   LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display: Good Display GDEH0576T81        https://www.laskakit.cz/good-display-gdeh0576t81-5-76--920--680-epaper-displej/
 *
 * Requires: Adafruit_GFX and GxEPD2 v1.6.9 or newer (https://github.com/ZinggJM/GxEPD2), author Jean-Marc Zingg
 *
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_BW_SHM.h>
#include <Fonts/FreeMonoBold9pt7b.h>

//#define ESPink_V2     // for version v2.6 and earlier
#define ESPink_V3       // for version v3.0 and above

#if defined(ESPink_V2)
  // MOSI/SDI 23, CLK/SCK 18, SS/CS 5
  #define DC    17
  #define RST   16
  #define BUSY  4
  #define POWER 2
#elif defined(ESPink_V3)
  // MOSI/SDI 11, CLK/SCK 12, SS/CS 10
  #define DC    48
  #define RST   45
  #define BUSY  36
  #define POWER 47
#endif

// GDEH0576T81 920x680, SSD2677
GxEPD2_BW_SHM<GxEPD2_576_GDEH0576T81, GxEPD2_576_GDEH0576T81::HEIGHT / 2> display(GxEPD2_576_GDEH0576T81(SS, DC, RST, BUSY));

const char HelloWorld[]   = "Hello World!";
const char HelloArduino[] = "Hello Arduino!";
const char HelloEpaper[]  = "Hello E-Paper!";

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  // turn on power to display
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);
  Serial.println("Display power ON");
  delay(1000);

  display.init();

  helloWorld();                 // first update should be full refresh
  delay(1000);
  helloFullScreenPartialMode();
  delay(1000);
  helloArduino();
  delay(1000);
  helloEpaper();
  delay(1000);
  showFont("FreeMonoBold9pt7b", &FreeMonoBold9pt7b);
  delay(1000);
  if (display.epd2.hasPartialUpdate)
  {
    showPartialUpdate();
    delay(1000);
  }
  deepSleepTest();
  display.powerOff();

  Serial.println("setup done");
}

void loop()
{
}

// Note: partial window size and position is on byte boundary in physical x direction,
// setPartialWindow() increases the window if x or w are not multiple of 8.

void helloWorld()
{
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(HelloWorld);
  }
  while (display.nextPage());
}

void helloFullScreenPartialMode()
{
  const char fullscreen[] = "full screen update";
  const char* updatemode;
  if (display.epd2.hasFastPartialUpdate)  updatemode = "fast partial mode";
  else if (display.epd2.hasPartialUpdate) updatemode = "slow partial mode";
  else                                    updatemode = "no partial mode";

  display.setPartialWindow(0, 0, display.width(), display.height());
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(fullscreen, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t utx = ((display.width() - tbw) / 2) - tbx;
  uint16_t uty = ((display.height() / 4) - tbh / 2) - tby;
  display.getTextBounds(updatemode, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t umx = ((display.width() - tbw) / 2) - tbx;
  uint16_t umy = ((display.height() * 3 / 4) - tbh / 2) - tby;
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t hwx = ((display.width() - tbw) / 2) - tbx;
  uint16_t hwy = ((display.height() - tbh) / 2) - tby;
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(hwx, hwy);
    display.print(HelloWorld);
    display.setCursor(utx, uty);
    display.print(fullscreen);
    display.setCursor(umx, umy);
    display.print(updatemode);
  }
  while (display.nextPage());
}

// partial update of one text line, aligned with centered HelloWorld
// yPos: vertical position as fraction of display height (1/4 or 3/4)
void helloLine(const char* text, uint16_t yNum, uint16_t yDen)
{
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t y = ((display.height() * yNum / yDen) - tbh / 2) - tby; // y is base line
  // window big enough to overwrite descenders of previous text
  uint16_t wh = FreeMonoBold9pt7b.yAdvance;
  uint16_t wy = (display.height() * yNum / yDen) - wh / 2;
  display.setPartialWindow(0, wy, display.width(), wh);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(text);
  }
  while (display.nextPage());
}

void helloArduino()
{
  helloLine(HelloArduino, 1, 4);
}

void helloEpaper()
{
  helloLine(HelloEpaper, 3, 4);
}

// print two centered lines at 1/3 and 2/3 of display height
void drawTwoLines(const char* line1, const char* line2)
{
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(line1, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x1 = (display.width() - tbw) / 2;
  uint16_t y1 = ((display.height() / 3) - tbh / 2) - tby;
  display.getTextBounds(line2, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x2 = (display.width() - tbw) / 2;
  uint16_t y2 = ((display.height() * 2 / 3) - tbh / 2) - tby;
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x1, y1);
    display.print(line1);
    display.setCursor(x2, y2);
    display.print(line2);
  }
  while (display.nextPage());
}

void deepSleepTest()
{
  const char hibernating[] = "hibernating ...";
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(hibernating, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(hibernating);
  }
  while (display.nextPage());
  display.hibernate();
  delay(5000);
  drawTwoLines("woke up", "from deep sleep");
  delay(5000);
  drawTwoLines(hibernating, "again");
  display.hibernate();
}

void showFont(const char name[], const GFXfont* f)
{
  display.setFullWindow();
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 0);
    display.println();
    display.println(name);
    display.println(" !\"#$%&'()*+,-./");
    display.println("0123456789:;<=>?");
    display.println("@ABCDEFGHIJKLMNO");
    display.println("PQRSTUVWXYZ[\\]^_");
    display.println("`abcdefghijklmno");
    display.println("pqrstuvwxyz{|}~ ");
  }
  while (display.nextPage());
}

// partial update test; values intentionally not multiples of 8
void showPartialUpdate()
{
  helloWorld(); // some background
  uint16_t box_x = 10;
  uint16_t box_y = 15;
  uint16_t box_w = 70;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + box_h - 6;
  float value = 13.95;
  uint16_t incr = display.epd2.hasFastPartialUpdate ? 1 : 3;
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  // show where the update box is
  for (uint16_t r = 0; r < 4; r++)
  {
    display.setRotation(r);
    display.setPartialWindow(box_x, box_y, box_w, box_h);
    display.firstPage();
    do
    {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
    }
    while (display.nextPage());
    delay(2000);
    display.firstPage();
    do
    {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    }
    while (display.nextPage());
    delay(1000);
  }
  // show updates in the update box
  for (uint16_t r = 0; r < 4; r++)
  {
    display.setRotation(r);
    display.setPartialWindow(box_x, box_y, box_w, box_h);
    for (uint16_t i = 1; i <= 10; i += incr)
    {
      display.firstPage();
      do
      {
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        display.setCursor(box_x, cursor_y);
        display.print(value * i, 2);
      }
      while (display.nextPage());
      delay(500);
    }
    delay(1000);
    display.firstPage();
    do
    {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    }
    while (display.nextPage());
    delay(1000);
  }
}
