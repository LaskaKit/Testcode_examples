 /* Display test for Good Display GDEM075F52
 * example from Good Display is used
 * 
 * Board:   LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display: Good Display GDEY133F91         
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// ! For the board settings see Display_EPD_W21_spi.h


#include <SPI.h>
#include "esp_sleep.h"
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

void setup() {
   Serial.begin(115200);

   // Zapnuti napajeni e-paperu (VSENSOR)
   pinMode(EPD_POWER, OUTPUT);
   digitalWrite(EPD_POWER, HIGH);
   delay(100);

   pinMode(EPD_BUSY, INPUT);   //BUSY
   pinMode(EPD_RST, OUTPUT);   //RES
   pinMode(EPD_DC, OUTPUT);    //DC
   pinMode(EPD_CS, OUTPUT);    //CS
   digitalWrite(EPD_CS, HIGH);

   //SPI - na ESP32-S3 je nutne zadat piny explicitne, CS ridime rucne
   SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, -1);
   SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
}
//Tips//
/*
1.Flickering is normal when EPD is performing a full screen update to clear ghosting from the previous image so to ensure better clarity and legibility for the new image.
2.There will be no flicker when EPD performs a partial refresh.
3.Please make sue that EPD enters sleep mode when refresh is completed and always leave the sleep mode command. Otherwise, this may result in a reduced lifespan of EPD.
4.Please refrain from inserting EPD to the FPC socket or unplugging it when the MCU is being powered to prevent potential damage.)
5.Re-initialization is required for every full screen update.
6.When porting the program, set the BUSY pin to input mode and other pins to output mode.
*/
void loop() {
#if 1//Full screen refresh demostration.

   /************Full display*******************/
    EPD_init(); //Full screen refresh initialization.
    PIC_display(gImage_1);//To Display one image using full screen refresh.
    EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    delay(5000); //Delay for 5s.

  #if 0//Demonstration of Display 4 colors, to enable this feature, please change 0 to 1.
   /************Full display*******************/
    EPD_init(); //Full screen refresh initialization.
    Display_All_Black();//To Display one image using full screen refresh.
    EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    delay(3000); //Delay for 3s.

    EPD_init(); //Full screen refresh initialization.
    Display_All_Yellow();//To Display one image using full screen refresh.
    EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    delay(3000); //Delay for 3s.

    EPD_init(); //Full screen refresh initialization.
    Display_All_Red();//To Display one image using full screen refresh.
    EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    delay(3000); //Delay for 3s.
  #endif
    EPD_init(); //Full screen refresh initialization.
    Display_All_White(); //Clear screen function.
    EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen..

#endif

    // Konec dema: vypnout napajeni displeje a uspat ESP32-S3
    SPI.endTransaction();
    delay(100);
    digitalWrite(EPD_POWER, LOW);
    Serial.println("Hotovo, deep sleep.");
    esp_deep_sleep_start(); // probudi se jen tlacitkem RESET
}


//////////////////////////////////END//////////////////////////////////////////////////
