/* 
 *  Vzorovy priklad pouziti LilyGO T5 4.7"
 *  od https://laskarduino.cz
 *  
 *  ePaper vykreslni logo a 3 vety.
 *  
 *  (@)2021 laskarduino.cz
 *  
 *  Vzorovy kod ja zalozen na kodu od lilygo
 *  https://github.com/Xinyuan-LilyGO/LilyGo-EPD47
 */

#include <Arduino.h>
#include "epd_driver.h"
#include "pic1.h"
#include "firasans.h"

void setup()
{
    Serial.begin(115200);

    // inicializace T5
    epd_init();

    Rect_t area1 = {
        .x = 30,
        .y = 100,
        .width = pic1_width,
        .height =  pic1_height
    };

    // Zapnuti T5
    epd_poweron();

    // Smazani T5
    epd_clear();

    // Vykresleni loga
    epd_draw_grayscale_image(area1, (uint8_t *) pic1_data);
    delay(200);
    //epd_poweroff();

    //epd_poweron();
    int cursor_x = 30;
    int cursor_y = 320;
    char *string1 = "➸ LilyGo T5 4,7\"\n";
    // Vykresleni prvni vety
    writeln((GFXfont *)&FiraSans, string1, &cursor_x, &cursor_y, NULL);
    delay(200);
    cursor_x = 30;
    cursor_y += 50;
    char *string2 = "➸ 16 odstinu sedi\n";
    // Vykresleni druhe vety
    writeln((GFXfont *)&FiraSans, string2, &cursor_x, &cursor_y, NULL);
    delay(200);
    cursor_x = 30;
    cursor_y += 50;
    char *string3 = "➸ exkluzivne na laskarduino.cz";
    // Vykresleni treti vety
    writeln((GFXfont *)&FiraSans, string3, &cursor_x, &cursor_y, NULL);
    delay(200);

    // Vypnuti T5
    epd_poweroff();
    
}

void loop()
{

   
}
