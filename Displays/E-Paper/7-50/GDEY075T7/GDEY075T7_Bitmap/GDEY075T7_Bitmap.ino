/* Display test for Good Display GDEY075T7
 * example from Good Display is used
 * 
 * Board:   LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display: Good Display GDEY075T7          https://www.laskakit.cz/good-display-gdey075t7-7-5--800x480-epaper-displej-grayscale/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// ! For the board settings see Display_EPD_W21_spi.h

#include <SPI.h>

#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"  

void setup() {

  // turn on power to display
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);   // turn the Display Power on (HIGH is the voltage level)
  delay(100);   

  pinMode(BUSY, INPUT);
  pinMode(RST, OUTPUT);
  pinMode(DC, OUTPUT);
  pinMode(CS, OUTPUT);
  //SPI
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
  SPI.begin (SCK, MISO, MOSI, CS);  

  //Full screen refresh, fast refresh

#if 1
  /************Full display(2s)*******************/

  EPD_Init(); //Full screen refresh initialization.
  EPD_WhiteScreen_ALL(Bitmap800x480_1); //To Display one image using full screen refresh.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000); //Delay for 2s.

  EPD_Init(); //Full screen refresh initialization.
  EPD_WhiteScreen_ALL(Bitmap800x480_2); //To Display one image using full screen refresh.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000); //Delay for 2s.

  EPD_Init(); //Full screen refresh initialization.
  EPD_WhiteScreen_ALL(Bitmap800x480_3); //To Display one image using full screen refresh.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000); //Delay for 2s.

  EPD_Init(); //Full screen refresh initialization.
  EPD_WhiteScreen_ALL(Bitmap800x480_4); //To Display one image using full screen refresh.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000); //Delay for 2s.

#endif

#if 1
  /************Fast refresh mode(1.5s)*******************/

  EPD_Init_Fast(); //Fast refresh initialization.
  EPD_WhiteScreen_ALL_Fast(Bitmap800x480_1); //To display one image using fast refresh.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000); //Delay for 2s.

  EPD_Init_Fast(); //Fast refresh initialization.
  EPD_WhiteScreen_ALL_Fast(Bitmap800x480_2); //To display one image using fast refresh.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000); //Delay for 2s

  EPD_Init_Fast(); //Fast refresh initialization.
  EPD_WhiteScreen_ALL_Fast(Bitmap800x480_3); //To display one image using fast refresh.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000); //Delay for 2s.

  EPD_Init_Fast(); //Fast refresh initialization.
  EPD_WhiteScreen_ALL_Fast(Bitmap800x480_4); //To display one image using fast refresh.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  delay(2000); //Delay for 2s.

#endif

  /************Esrase screen*******************/
  EPD_Init(); //Full screen refresh initialization.
  EPD_WhiteScreen_White(); //Clear screen function.
  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  while(1);  // The program stops here   
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
void loop() {}
