#include <Arduino.h>
#include <U8g2lib.h>      //Using u8g2 library

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(  /* clock=*/ 21,   /* data=*/ 20, /* reset=*/ 8);   // OLEDs without Reset of the Display
void setup(void)
{
    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0,0,"LASKARDUINO.CZ");
    u8x8.drawString(0,1,"od bastlire ");
    u8x8.drawString(0,2,"bastlirovi");
    u8x8.drawString(0,4,"za fakt ");
    u8x8.drawString(0,5,"skvely ceny");
}
void loop(void)
{
}

