/* Display test for Good Display GDEM0189F66
 * example from Good Display is used
 * 
 * Board:   LaskaKit ESPink v3.6   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 *          LaskaKit microESPink   https://www.laskakit.cz/laskakit-microespink-esp32-s3-mini-e-paper/
 * Display: Good Display GDEM0189F66
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"  

void setup() {
   // turn on power to ePaper (GPIO47, active HIGH)
   pinMode(POWER, OUTPUT);
   digitalWrite(POWER, HIGH);
   delay(100);

   pinMode(BUSY, INPUT);   //BUSY
   pinMode(RST, OUTPUT);   //RES
   pinMode(DC, OUTPUT);    //DC
   pinMode(CS, OUTPUT);    //CS
   digitalWrite(CS, HIGH);
   //SPI
   SPI.begin(SCK, MISO, MOSI, CS);
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
    	/************Fast update mode(12s)*******************/
	#if 1 //Fast update demostration.	
		EPD_init_Fast(); //Fast screen update initialization.
		PIC_display(gImage_1);//To Display one image using full screen update.
		EPD_sleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
		 delay(5000); //Delay for 5s.
	#endif	
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
