#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
/*
 * U8glib Example Overview:
 *   Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
 *   Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
 *   U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
 *
 */
// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 21, /* data=*/ 20, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Všechny displeje bez RES-pinu
void setup(void) {
    u8g2.begin();
}
void loop(void) {
    u8g2.clearBuffer();					        // vyčistit vnitřní paměť
    u8g2.setFont(u8g2_font_ncenB14_tr);	// vyber font
    u8g2.drawStr(28,20,"kup me");	      // napiš něco do vnitřní paměti
    u8g2.drawStr(51,40,"na");            // napiš něco do vnitřní paměti
    u8g2.setFont(u8g2_font_ncenB12_tr);  // vyber font
    u8g2.drawStr(0,60,"laskarduino.cz"); // napiš něco do vnitřní paměti
    u8g2.sendBuffer();					         // pošli vnitřní paměť do displeje
    delay(1000);
}
