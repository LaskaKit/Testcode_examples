/*
*	Vzorovy kod od laskarduino.cz pro LilyGo TTV 
*	Kod zobrazi napis
*	pote logo laskarduino
*	a nakonec napis laskarduino 
*
*	Instrukce k vytovreni loga:
*	Vytvorte si cernobile logo v programu GIMP a velikosti rovne nebo mensi nez je rozliseni
*	OLED. Soubor ulozte jako JMENO.xbm a otevrete jej v textovem editoru.
*	Pole v *.xbm souboru zkopirujte do tohoto souboru *.ino a upravte parametry 
*	vyska a sirka. 
*
*	Vytvoreno (c) laskarduino.cz 2021
*
*	Potrebne knihovny:
*	u8g2lib
*
*
*/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "utilities.h"

#define laskarduinoTTV_logo_width 77
#define laskarduinoTTV_logo_height 60
// 'laskarduinoTTV_logo', 77x60px
const unsigned char laskarduinoTTV_logo_bits [] PROGMEM = {
   0x00, 0xf0, 0x1f, 0x00, 0x00, 0x00, 0xf8, 0x07, 0x00, 0x00, 0x00, 0xfc,
   0xff, 0x00, 0x00, 0x00, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xff, 0xff, 0x03,
   0x00, 0xc0, 0xff, 0xff, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x07, 0x00, 0xe0,
   0xff, 0xff, 0x01, 0x00, 0xe0, 0xff, 0xff, 0x0f, 0x00, 0xf8, 0xff, 0xff,
   0x07, 0x00, 0xf0, 0xff, 0xff, 0x3f, 0x00, 0xfc, 0xff, 0xff, 0x0f, 0x00,
   0xf8, 0xff, 0xff, 0x3f, 0x00, 0xfe, 0xff, 0xff, 0x0f, 0x00, 0xf8, 0x3f,
   0xf0, 0x7f, 0x00, 0xff, 0x0f, 0xfc, 0x1f, 0x00, 0xfc, 0x07, 0xc0, 0xff,
   0x00, 0xfe, 0x01, 0xe0, 0x3f, 0x00, 0xfc, 0x03, 0x80, 0xff, 0x01, 0xfc,
   0x00, 0xc0, 0x3f, 0x00, 0xfe, 0x01, 0x00, 0xfe, 0x03, 0x78, 0x00, 0x80,
   0x7f, 0x00, 0xfe, 0x00, 0x00, 0xfc, 0x07, 0x30, 0x00, 0x00, 0x7f, 0x00,
   0xff, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00,
   0x00, 0xf0, 0x1f, 0x00, 0x00, 0x00, 0xff, 0x00, 0x7f, 0x00, 0x00, 0xe0,
   0x3f, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x7f, 0x00, 0x00, 0xc0, 0x7f, 0x00,
   0x00, 0x00, 0xfe, 0x00, 0x7f, 0x00, 0x00, 0x80, 0xff, 0x00, 0x00, 0x00,
   0xfe, 0x00, 0x7f, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xfe, 0x00,
   0x7f, 0x00, 0x00, 0x00, 0xfe, 0x03, 0x00, 0x00, 0xfe, 0x00, 0x7f, 0x00,
   0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0xfe, 0x00, 0xff, 0x00, 0x00, 0x00,
   0xf8, 0x0f, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xf0, 0x1f,
   0x00, 0x00, 0xff, 0x00, 0xfe, 0x00, 0x00, 0x00, 0xf0, 0x3f, 0x00, 0x00,
   0x7f, 0x00, 0xfe, 0x01, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0xc0, 0x7f, 0x00,
   0xfc, 0x03, 0x00, 0x00, 0xc0, 0xff, 0x00, 0xc0, 0x3f, 0x00, 0xfc, 0x07,
   0x00, 0x00, 0x80, 0xff, 0x01, 0xf0, 0x3f, 0x00, 0xf8, 0x0f, 0x00, 0x00,
   0x00, 0xff, 0x0f, 0xfc, 0x1f, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0xfe,
   0xff, 0xff, 0x0f, 0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff,
   0x07, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x07, 0x00,
   0xc0, 0xff, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x01, 0x00, 0x80, 0xff,
   0x01, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x03, 0x00,
   0x00, 0x00, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xfe, 0x07, 0x00, 0x00, 0x00,
   0xf8, 0x07, 0x00, 0x00, 0x00, 0xfc, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0,
   0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x01, 0x00, 0x80,
   0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0x00, 0xc0, 0xff, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xfe, 0x07, 0x00, 0xe0, 0x7f, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xfc, 0x0f, 0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xf8, 0x1f, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f,
   0x00, 0xfc, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x00, 0xfe,
   0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x00, 0xff, 0x03, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x81, 0xff, 0x01, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xff, 0xc3, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xfe, 0xe7, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc,
   0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x1f,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x80, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00
   };

#define laskarduinoTTV_width 120
#define laskarduinoTTV_height 33
static unsigned char laskarduinoTTV_bits[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00,
   0x00, 0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
   0x03, 0x00, 0x00, 0x78, 0xc0, 0x0f, 0x80, 0x07, 0x00, 0x00, 0x00, 0x80,
   0x07, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x80, 0x07, 0x00,
   0x00, 0x00, 0x80, 0x07, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00,
   0x80, 0x07, 0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x78, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x78, 0xfe, 0xc1, 0x9f, 0xc7, 0xf3, 0x0f, 0x7f, 0xfc,
   0xe7, 0xf1, 0x3c, 0xff, 0x81, 0x3f, 0x78, 0xfe, 0xe3, 0x9f, 0xe7, 0xf1,
   0x1f, 0x7f, 0xfe, 0xe7, 0xf1, 0x3c, 0xff, 0xc3, 0x7f, 0x78, 0xec, 0xf3,
   0x88, 0xf7, 0x60, 0x1f, 0x7f, 0xff, 0xe7, 0xf1, 0x3c, 0xff, 0xe3, 0x7f,
   0x78, 0xc0, 0xf3, 0x83, 0x7f, 0x00, 0x3c, 0x0f, 0x8f, 0xe7, 0xf1, 0x3c,
   0x8f, 0xe7, 0xf1, 0x78, 0xfc, 0xe7, 0x9f, 0x7f, 0xe0, 0x3f, 0x07, 0x87,
   0xe7, 0xf1, 0x3c, 0x8f, 0xe7, 0xf0, 0x78, 0xfe, 0xc7, 0x9f, 0xff, 0xf0,
   0x3f, 0x07, 0x8f, 0xe7, 0xf1, 0x3c, 0x8f, 0xf7, 0xf0, 0x78, 0x8f, 0x07,
   0xbc, 0xff, 0x79, 0x3c, 0x07, 0x8f, 0xe7, 0xf9, 0x3c, 0x8f, 0xe7, 0xf1,
   0x78, 0xdf, 0xe7, 0xbf, 0xe7, 0xf9, 0x3e, 0x07, 0xff, 0xe7, 0xff, 0x3c,
   0x8f, 0xe7, 0x7f, 0x78, 0xfe, 0xf7, 0x9f, 0xc7, 0xf3, 0x3f, 0x07, 0xfe,
   0xc7, 0xff, 0x3c, 0x8f, 0xc7, 0x7f, 0x78, 0xbc, 0xe7, 0x8f, 0xc7, 0xe7,
   0x3d, 0x07, 0xfc, 0x87, 0xff, 0x3c, 0x8f, 0x87, 0x1f, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00 };

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI u8g2(U8G2_R0,
        /* clock=*/ OLED_SCLK,
        /* data=*/  OLED_MOSI,
        /* cs=*/    OLED_CS,
        /* dc=*/    OLED_DC,
        /* reset=*/ OLED_RST);


void setup(void)
{
    Serial.begin(115200);

    /*Turn on power control*/
    pinMode(PWR_ON, OUTPUT);
    digitalWrite(PWR_ON, HIGH);

    /*Power on the display*/
    pinMode(OLED_PWR, OUTPUT);
    digitalWrite(OLED_PWR, HIGH);

    /*Set touch Pin as input*/
    pinMode(TOUCH_PIN, INPUT);

    u8g2.begin();

    u8g2.enableUTF8Print(); // enable UTF8 support for the Arduino print() function

}

void loop(void)
{
    u8g2.setFont(u8g2_font_unifont_t_chinese2);  // font
    u8g2.setFontDirection(0);
    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    u8g2.print("LilyGO TTV na");
    u8g2.setCursor(0, 40);
    u8g2.print("laskarduino.cz");       
    u8g2.sendBuffer();

    delay(2000);

    u8g2.clearBuffer();
    u8g2.drawXBMP( 25, 0, laskarduinoTTV_logo_width, laskarduinoTTV_logo_height, laskarduinoTTV_logo_bits);
    u8g2.sendBuffer();
    
    delay(2000);

    u8g2.clearBuffer();
    u8g2.drawXBMP( 2, 0, laskarduinoTTV_width, laskarduinoTTV_height, laskarduinoTTV_bits);
    u8g2.sendBuffer();

    delay(10000);
}