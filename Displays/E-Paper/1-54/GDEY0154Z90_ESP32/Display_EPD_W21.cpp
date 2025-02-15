#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

void Epaper_READBUSY(void)
{
  while(1)
  {  //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  } 
}


//SS1680
void EPD_HW_Init(void)
{ 
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_READBUSY();   
  EPD_W21_WriteCMD(0x12);  //SWRESET
  Epaper_READBUSY();   
    
  EPD_W21_WriteCMD(0x01); //Driver output control      
  EPD_W21_WriteDATA((EPD_HEIGHT-1)%256);   
  EPD_W21_WriteDATA((EPD_HEIGHT-1)/256);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x11); //data entry mode       
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0x44); //set Ram-X address start/end position   
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(EPD_WIDTH/8-1);    

  EPD_W21_WriteCMD(0x45); //set Ram-Y address start/end position          
  EPD_W21_WriteDATA((EPD_HEIGHT-1)%256);   
  EPD_W21_WriteDATA((EPD_HEIGHT-1)/256);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00); 

  EPD_W21_WriteCMD(0x3C); //BorderWavefrom
  EPD_W21_WriteDATA(0x05);  
      
  EPD_W21_WriteCMD(0x18); //Read built-in temperature sensor
  EPD_W21_WriteDATA(0x80);  

  EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X199;    
  EPD_W21_WriteDATA((EPD_HEIGHT-1)%256);    
  EPD_W21_WriteDATA((EPD_HEIGHT-1)/256);
  Epaper_READBUSY();
}


  
void EPD_DeepSleep(void)
{
  EPD_W21_WriteCMD(0x10); //enter deep sleep
  EPD_W21_WriteDATA(0x01); 
  delay(100);
}
//Full screen refresh update function
void EPD_Update(void)
{   
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xF7);   
  EPD_W21_WriteCMD(0x20);  //Activate Display Update Sequence
  Epaper_READBUSY(); 
}


void EPD_WhiteScreen_ALL(const unsigned char* datasBW,const unsigned char* datasRW)
{
    unsigned int i;
    //Write Data
    EPD_W21_WriteCMD(0x24);        //Transfer old data
    for(i=0;i<EPD_ARRAY;i++)    
    { 
      EPD_W21_WriteDATA(datasBW[i]);  //Transfer the actual displayed data
    } 
    EPD_W21_WriteCMD(0x26);        //Transfer new data
    for(i=0;i<EPD_ARRAY;i++)       
    {
      EPD_W21_WriteDATA(~datasRW[i]);  //Transfer the actual displayed data
    }
    EPD_Update();     

}
void EPD_WhiteScreen_White(void)
{
    unsigned int i;
    //Write Data
    EPD_W21_WriteCMD(0x24);        //Transfer old data
    for(i=0;i<EPD_ARRAY;i++)    
    { 
      EPD_W21_WriteDATA(0xFF); 
    }
    EPD_W21_WriteCMD(0x26);        //Transfer new data
    for(i=0;i<EPD_ARRAY;i++)       
    {
      EPD_W21_WriteDATA(0x00);  //Transfer the actual displayed data
    }
    EPD_Update();     
}


////////////////////////////////Other newly added functions////////////////////////////////////////////
//Display rotation 180 degrees initialization
void EPD_HW_Init_180(void)
{ 
  EPD_W21_RST_0;  // Module reset   
  delay(10); //At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_READBUSY();   
  EPD_W21_WriteCMD(0x12);  //SWRESET
  Epaper_READBUSY();   
  
  EPD_W21_WriteCMD(0x3C); //BorderWavefrom
  EPD_W21_WriteDATA(0x05);
  
  EPD_W21_WriteCMD(0x01); //Driver output control      
  EPD_W21_WriteDATA((EPD_HEIGHT-1)%256);    
  EPD_W21_WriteDATA((EPD_HEIGHT-1)/256);
  EPD_W21_WriteDATA(0x00); 

  EPD_W21_WriteCMD(0x11); //data entry mode       
  EPD_W21_WriteDATA(0x02);

  EPD_W21_WriteCMD(0x44); //set Ram-X address start/end position   
  EPD_W21_WriteDATA(EPD_WIDTH/8-1);    
  EPD_W21_WriteDATA(0x00);  

  EPD_W21_WriteCMD(0x45); //set Ram-Y address start/end position          
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00); 
  EPD_W21_WriteDATA((EPD_HEIGHT-1)%256);  
  EPD_W21_WriteDATA((EPD_HEIGHT-1)/256);

  
  EPD_W21_WriteCMD(0x18); //Read built-in temperature sensor
  EPD_W21_WriteDATA(0x80);  

  EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
  EPD_W21_WriteDATA(EPD_WIDTH/8-1);    
  EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X199;    
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);
  Epaper_READBUSY();
}








/***********************************************************
            end file
***********************************************************/
