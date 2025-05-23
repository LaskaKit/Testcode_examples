/* Display test for Good Display GDEM102T91 with 
 * ESP32 Development Kit with E Ink/E-Paper Driver Board - Wireless (WiFi/Bluetooth) - ESP32-L Series
 * and 
 * 24 pin serial ePaper screen HAT adapter board, DESPI-C02
 *
 * Example from Good Display is used
 * 
 * Board:   Good Display ESP32 Development Kit  https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Adapter: Good Display DESPI-C02              https://www.laskakit.cz/good-display-despi-c02-univerzalni-spi-e-paper-adapter/
 * Display: Good Display GDEM102T91             https://www.laskakit.cz/good-display-gdeq102t90-10-2--960x640-epaper-displej/
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
   pinMode(A14, INPUT);  //BUSY
   pinMode(A15, OUTPUT); //RES 
   pinMode(A16, OUTPUT); //DC   
   pinMode(A17, OUTPUT); //CS   
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
   unsigned char i;

#if 1 //Full screen refresh, fast refresh, and partial refresh demostration.
      EPD_HW_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_White(); //Clear screen function.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s. 
     /************Full display(2s)*******************/
      EPD_HW_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_ALL(gImage_1); //To Display one image using full screen refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s. 
            
  #if 1 //Partial refresh demostration.
  //Partial refresh demo support displaying a clock at 5 locations with 00:00.  If you need to perform partial refresh more than 5 locations, please use the feature of using partial refresh at the full screen demo.
  //After 5 partial refreshes, implement a full screen refresh to clear the ghosting caused by partial refreshes.
  //////////////////////Partial refresh time demo/////////////////////////////////////
      EPD_HW_Init(); //Electronic paper initialization  
      EPD_SetRAMValue_BaseMap(gImage_basemap); //Partial refresh background color     
      for(i=0;i<6;i++)
      EPD_Dis_Part_Time(  696+32*1,272,(unsigned char *)&Num[1],            //x-A,y-A,DATA-A
                          696+32*2,272,(unsigned char *)&Num[0],         //x-B,y-B,DATA-B
                          696+32*3,272,(unsigned char *)gImage_numdot,      //x-C,y-C,DATA-C
                          696+32*4,272,(unsigned char *)&Num[0],        //x-D,y-D,DATA-D
                          696+32*5,272,(unsigned char *)&Num[i],32,64); //x-E,y-E,DATA-E,Resolution 32*64
    
      EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
      EPD_HW_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_White(); //Clear screen function.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
  #endif
#endif
    while(1); // The program stops here
}




//////////////////////////////////END//////////////////////////////////////////////////
