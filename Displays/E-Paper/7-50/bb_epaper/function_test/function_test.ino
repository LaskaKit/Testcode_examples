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
// Library function test
// Test functions and boundary conditions
//
#include <bb_epaper.h>
BBEPAPER epd(EP75_800x480);
#include "Roboto_Thin_48.h"

#define ESPink_V2     //for version v2.6 and earlier
//#define ESPink_V3     //for version v3.0 and above

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
#define BUSY_PIN  38 
#define POWER_PIN 47
#endif

// Custom font clipping test
void test1()
{
  Serial.print("Custom font upper edge clipping test: ");
  epd.fillScreen(BBEP_WHITE);
  epd.setTextColor(BBEP_BLACK, BBEP_WHITE);
  epd.setCursor(0, 10); // text needs to be clipped above the top of the display area
  epd.setFont(Roboto_Thin_48);
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
  Serial.print("Custom font right edge clipping test: ");
  epd.setCursor(epd.width()-30, 10); // text needs to be clipped above the top of the display area
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
  Serial.print("Custom font lower right corner clipping test: ");
  epd.setCursor(epd.width()-30, epd.height()+10);
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
}
// Built-in font clipping test
void test2()
{
  Serial.print("Buit-in font upper edge clipping test: ");
  epd.fillScreen(BBEP_WHITE);
  epd.setTextColor(BBEP_BLACK, BBEP_WHITE);
  epd.setCursor(0, -10); // text needs to be clipped above the top of the display area
  epd.setFont(FONT_12x16);
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
  Serial.print("Buit-in font right edge clipping test: ");
  epd.setCursor(epd.width()-30, 0); // text needs to be clipped above the top of the display area
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
  Serial.print("Buit-in font lower right corner clipping test: ");
  epd.setCursor(epd.width()-30, epd.height()-8);
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
}

void test3()
{

}
void setup()
{
  Serial.begin(115200);
  delay(3000); // wait for CDC-Serial
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  delay(100); // allow time to settle
  
  Serial.println("bb_epaper function tests");
  epd.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN, MOSI_PIN, CLK_PIN);
  epd.allocBuffer();
  test1();
  test2();
  test3();
} /* setup() */

void loop()
{
} /* loop() */

