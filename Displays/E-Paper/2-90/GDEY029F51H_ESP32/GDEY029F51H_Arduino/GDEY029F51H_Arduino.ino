/* Display test for Good Display GDEY029F51H
 * example from Good Display is used
 * 
 * Board:           LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display:         Good Display GDEY029F51H        https://www.laskakit.cz/good-display-gdey029f51h-2-9--384x168-epaper-displej-4-barvy/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// Display Library example for SPI e-paper panels from Dalian Good Display.

#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"  

//IO settings
int BUSY_Pin = 4;
int RES_Pin = 16;
int DC_Pin = 17;
int CS_Pin = 5;
//SCLK--GPIO18
//MOSI---GPIO23

void setup() {
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);

// turn on power to display
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("Display power ON");
  delay(1000);   

   pinMode(BUSY_Pin, INPUT);  //BUSY
   pinMode(RES_Pin, OUTPUT); //RES 
   pinMode(DC_Pin, OUTPUT); //DC   
   pinMode(CS_Pin, OUTPUT); //CS   
   //SPI
   SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
   SPI.begin ();  
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
    while(1); // The program stops here         
   
}




//////////////////////////////////END//////////////////////////////////////////////////
