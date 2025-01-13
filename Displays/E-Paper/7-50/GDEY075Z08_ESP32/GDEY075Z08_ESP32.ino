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
  unsigned char i;
#if 1 //Full screen refresh, fast refresh, and partial refresh demostration.
			EPD_Init(); //Full screen refresh initialization.
			EPD_WhiteScreen_ALL(gImage_BW1,gImage_RW1); //To Display one image using full screen refresh.
			EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
			delay(3000); //Delay for 3s.
			
			/************Fast refresh mode(16s)*******************/
	#if 1 //Fast refresh demostration.	
			EPD_Init_Fast(); //Fast refresh initialization.
			EPD_WhiteScreen_ALL(gImage_BW1,gImage_RW1); //To Display one image using full screen refresh.
			EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
			delay(3000); //Delay for 3s.
	#endif				
	#if 1 //Partial refresh demostration.
	//Partial refresh demo support displaying a clock at 5 locations with 00:00.  
	//After 5 partial refreshes, implement a full screen refresh to clear the ghosting caused by partial refreshes.
	//////////////////////Partial refresh time demo/////////////////////////////////////
			EPD_Init(); //Electronic paper initialization.	
			EPD_SetRAMValue_BaseMap(gImage_BW1,gImage_RW1); //Please do not delete the background color function, otherwise it will cause unstable display during partial refresh.
			EPD_Init_Part(); //Pa refresh initialization.
			for(i=0;i<6;i++)
			{
				EPD_Dis_Part_Time(376,24,Num[1],Num[0],gImage_numdot,Num[0],Num[i],5,104,48); //x,y,DATA-A~E,Resolution 48*104              		
			}				
			EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
			delay(2000); //Delay for 2s.
			EPD_Init(); //Full screen refresh initialization.
			EPD_WhiteScreen_White(); //Clear screen function.
			EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
			delay(2000); //Delay for 2s.
	#endif	
	
	#if 0 //Demonstration of full screen refresh with 180-degree rotation, to enable this feature, please change 0 to 1.
			/************Full display(2s)*******************/
			EPD_Init_180(); //Full screen refresh initialization.
		  EPD_WhiteScreen_ALL(gImage_BW1,gImage_RW1); //To Display one image using full screen refresh.
		  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
			delay(3000); //Delay for 3s.
	#endif			
	
#endif
  while(1); // The program stops here   
}




//////////////////////////////////END//////////////////////////////////////////////////
