#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"  


//IO settings

#define SCK   18          
#define MOSI  23
#define BUSY  4
#define CS   5
#define CS2   27
#define RES   16
#define DC    17
#define USUP_POWER  2

void setup() {
  Serial.begin(115200);
// turn on power to display
  pinMode(USUP_POWER, OUTPUT);
  digitalWrite(USUP_POWER, HIGH);   // turn the EPD
  Serial.println("Display power ON");
  delay(1000);   

   pinMode(BUSY, INPUT);  //BUSY
   pinMode(RES, OUTPUT); //RES 
   pinMode(DC, OUTPUT); //DC   
   pinMode(CS, OUTPUT); //CS
   pinMode(CS2, OUTPUT); //CS2    
   //SPI
   SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
   SPI.begin ( SCK, -1, MOSI, CS);     
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

     /************Full display(2s)*******************/
      EPD_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_ALL(gImage_1); //To Display one image using full screen refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
  


  #if 1    //Demo of using partial refresh to update the full screen, to enable this feature, please change 0 to 1.
  //After 5 partial refreshes, implement a full screen refresh to clear the ghosting caused by partial refreshes.
  //////////////////////Partial refresh time demo/////////////////////////////////////
      EPD_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_White(); //Clear screen function.   
      EPD_Init_Part();
      EPD_WhiteScreen_ALL_P(gImage_p1,gImage_p1);
      EPD_WhiteScreen_ALL_P(gImage_p1,gImage_p2);
      EPD_WhiteScreen_ALL_P(gImage_p2,gImage_p3); 
      EPD_DeepSleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
      
      EPD_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_White(); //Clear screen function.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
  #endif
  
  #if 0 //Demonstration of full screen refresh with 180-degree rotation, to enable this feature, please change 0 to 1.
      /************Full display(2s)*******************/
      EPD_Init_180(); //Full screen refresh initialization.
      EPD_WhiteScreen_ALL180(gImage_1); //To Display one image using full screen refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
  #endif        
  
#endif
 while(1);  // The program stops here   
}




//////////////////////////////////END//////////////////////////////////////////////////
