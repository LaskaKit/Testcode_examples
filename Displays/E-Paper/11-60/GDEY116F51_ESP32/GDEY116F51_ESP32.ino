/* Display test for Good Display GDEM075F52
 * example from Good Display is used
 * 
 * Board:   LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display: Good Display GDEM075F52          https://www.laskakit.cz/good-display-gdem075f52-7-5--800x480-epaper-displej-4-barvy/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// ! For the board settings see Display_EPD_W21_spi.h


#include <SPI.h>
//EPD
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
  		/************Fast display(16s)*******************/
		EPD_init_Fast(); //Full screen update initialization.
		PIC_display(gImage_1);//To Display one image using full screen update.
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
