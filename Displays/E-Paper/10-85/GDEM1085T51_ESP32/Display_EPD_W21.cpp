#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

unsigned char partFlag=1;
void lcd_chkstatus(void)
{
  while(isEPD_W21_BUSY==0);
                   
}


void EPD_Init(void)
{ 
  delay(100);//At least 10ms delay 
  EPD_W21_RST_0;    // Module reset
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10);//At least 10ms delay 
  lcd_chkstatus();

  EPD_W21_WriteCMD(0xF8);  // Enter FITI Command
  EPD_W21_WriteDATA(0x7C); 
  EPD_W21_WriteDATA(0x00); 

  EPD_W21_WriteCMD(0xF8);  // Enter FITI Command
  EPD_W21_WriteDATA(0x60); 
  EPD_W21_WriteDATA(0xA5); 

  EPD_W21_WriteCMD(0xF8);  
  EPD_W21_WriteDATA(0x92);
  EPD_W21_WriteDATA(0x08);
            
  EPD_W21_WriteCMD(0xF8);   
  EPD_W21_WriteDATA(0x93);
  EPD_W21_WriteDATA(0x18); 
      
  EPD_W21_WriteCMD(0xF8);   
  EPD_W21_WriteDATA(0x7E);
  EPD_W21_WriteDATA(0x01); 

  EPD_W21_WriteCMD(0x01);  // power setting
  EPD_W21_WriteDATA(0x03);  // 1-0=11: internal power
  EPD_W21_WriteDATA(0x00);  // 1-0=11: internal power
  EPD_W21_WriteDATA(0x3F);  // 1-0=11: internal power
  EPD_W21_WriteDATA(0x3F);  // 1-0=11: internal power
  EPD_W21_WriteDATA(0x28);  // 1-0=11: internal power

  EPD_W21_WriteCMD(0x06);  // Booster Setting 
  EPD_W21_WriteDATA(0x57);
  EPD_W21_WriteDATA(0x63);
  EPD_W21_WriteDATA(0x34); ////////////20091009
  

  EPD_W21_WriteCMD(0x60);  // TCON setting
  EPD_W21_WriteDATA(0x04);  //  00  S-G G-S     
      
  EPD_W21_WriteCMD(0x00);  // Panel setting   
  EPD_W21_WriteDATA(0x9F); //otp


  EPD_W21_WriteCMD(0x30);   // OSC Setting
  EPD_W21_WriteDATA(0x3C);  // 1-0=11: internal power


  EPD_W21_WriteCMD(0x61);  // Resolution setting
  EPD_W21_WriteDATA(0x02);
  EPD_W21_WriteDATA(0xA8);
  EPD_W21_WriteDATA(0x01);
  EPD_W21_WriteDATA(0xE0);
      
  EPD_W21_WriteCMD(0x82);  // VCOM DC Setting
  EPD_W21_WriteDATA(0x1F);  // 1-0=11: internal power


  EPD_W21_WriteCMD(0x50);  // VCOM and DATA interval setting    
  EPD_W21_WriteDATA(0x97);
      
  EPD_W21_WriteCMD(0x26);  // Set VCOM/Red states   
  EPD_W21_WriteDATA(0x00);  // 0F 1-0=11: Red 10 states ; 3-2=11 VCOM 10 states 

  EPD_W21_WriteCMD(0xE8);  
  EPD_W21_WriteDATA(0x40);  

  EPD_W21_WriteCMD(0xE0);  
  EPD_W21_WriteDATA(0x01);    

  EPD_W21_WriteCMD(0x04);  //power on  
  delay(200);  
  lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}

void EPD_Init_Part(void)
{
  delay(100);//At least 100ms delay 
  EPD_W21_RST_0;    // Module reset
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10);//At least 10ms delay 
  lcd_chkstatus();

  EPD_W21_WriteCMD(0xF8);  // Enter FITI Command
  EPD_W21_WriteDATA(0x7C); 
  EPD_W21_WriteDATA(0x00); 

  EPD_W21_WriteCMD(0xF8);  // Enter FITI Command
  EPD_W21_WriteDATA(0x60); 
  EPD_W21_WriteDATA(0xA5); 

  EPD_W21_WriteCMD(0xF8);  
  EPD_W21_WriteDATA(0x92);
  EPD_W21_WriteDATA(0x08);
            
  EPD_W21_WriteCMD(0xF8);   
  EPD_W21_WriteDATA(0x93);
  EPD_W21_WriteDATA(0x18); 
      
  EPD_W21_WriteCMD(0xF8);   
  EPD_W21_WriteDATA(0x7E);
  EPD_W21_WriteDATA(0x01); 

  EPD_W21_WriteCMD(0x01);  // power setting
  EPD_W21_WriteDATA(0x03);  // 1-0=11: internal power
  EPD_W21_WriteDATA(0x00);  // 1-0=11: internal power
  EPD_W21_WriteDATA(0x3F);  // 1-0=11: internal power
  EPD_W21_WriteDATA(0x3F);  // 1-0=11: internal power
  EPD_W21_WriteDATA(0x28);  // 1-0=11: internal power

  EPD_W21_WriteCMD(0x06);  // Booster Setting 
  EPD_W21_WriteDATA(0x57);
  EPD_W21_WriteDATA(0x63);
  EPD_W21_WriteDATA(0x34); ////////////20091009

  EPD_W21_WriteCMD(0x60);  // TCON setting
  EPD_W21_WriteDATA(0x04);  //  00  S-G G-S     
      
  EPD_W21_WriteCMD(0x00);  // Panel setting   
  EPD_W21_WriteDATA(0x9F); //otp
  
  EPD_W21_WriteCMD(0x30);   // OSC Setting
  EPD_W21_WriteDATA(0x3C);  // 1-0=11: internal power
  
  EPD_W21_WriteCMD(0x61);  // Resolution setting
  EPD_W21_WriteDATA(0x02);
  EPD_W21_WriteDATA(0xA8);
  EPD_W21_WriteDATA(0x01);
  EPD_W21_WriteDATA(0xE0);
      
  EPD_W21_WriteCMD(0x82);  // VCOM DC Setting
  EPD_W21_WriteDATA(0x1F);  // 1-0=11: internal power
    
  EPD_W21_WriteCMD(0x26);  // Set VCOM/Red states   
  EPD_W21_WriteDATA(0x00);  // 0F 1-0=11: Red 10 states ; 3-2=11 VCOM 10 states 

  EPD_W21_WriteCMD(0xE8);  
  EPD_W21_WriteDATA(0x40);  
  
  EPD_W21_WriteCMD(0xE0);  
  EPD_W21_WriteDATA(0x03);  //

  
  EPD_W21_WriteCMD(0xE5);  //100du
  EPD_W21_WriteDATA(0x64); 
  
  EPD_W21_WriteCMD(0x50);  // VCOM and DATA interval setting    
  EPD_W21_WriteDATA(0x97);
    

  EPD_W21_WriteCMD(0x04); //POWER ON
  delay(100);  
  lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal

}
  
void EPD_DeepSleep(void)
{

  EPD_W21_WriteCMD(0x02); //power off 
  delay(100);  
  lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal
    
  EPD_W21_WriteCMD(0X07);   //deep sleep
  EPD_W21_WriteDATA(0xA5);
}
//Full screen refresh update function
void EPD_Update(void)
{   
  //Refresh
  EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
  delay(1);              //!!!The delay here is necessary, 200uS at least!!!     
  lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
}


void EPD_WhiteScreen_ALL(const unsigned char *datas)
{
    unsigned int i,j;
    //Master
    EPD_W21_WriteCMD1(0x10);         //Transfer old data
    for(i=0;i<EPD_ARRAY;i++){   
      EPD_W21_WriteDATA1(0xFF);}  
    EPD_W21_WriteCMD1(0x13);         //Transfer old data
    for(j=0;j<480;j++)
    for(i=0;i<85;i++){    
      EPD_W21_WriteDATA1(datas[170*j+i]);}  

    //Slave
    EPD_W21_WriteCMD2(0x10);         //Transfer old data
    for(i=0;i<EPD_ARRAY;i++){   
      EPD_W21_WriteDATA2(0xFF);}  
    EPD_W21_WriteCMD2(0x13);         //Transfer old data
    for(j=0;j<480;j++)
    for(i=85;i<85*2;i++){   
      EPD_W21_WriteDATA2(datas[170*j+i]);}    
  
    EPD_Update();     
}
void EPD_WhiteScreen_ALL180(const unsigned char *datas)
{
    unsigned int i,j;
    //Master
    EPD_W21_WriteCMD1(0x10);         //Transfer old data
    for(i=0;i<EPD_ARRAY;i++){   
      EPD_W21_WriteDATA1(0xFF);}  
    EPD_W21_WriteCMD1(0x13);         //Transfer old data
    for(j=0;j<480;j++)
    for(i=85;i<85*2;i++){   
      EPD_W21_WriteDATA1(datas[170*j+i]);}   
          
    //Slave
    EPD_W21_WriteCMD2(0x10);         //Transfer old data
    for(i=0;i<EPD_ARRAY;i++){   
      EPD_W21_WriteDATA2(0xFF);}  
    EPD_W21_WriteCMD2(0x13);         //Transfer old data
    for(j=0;j<480;j++)
    for(i=0;i<85;i++){    
      EPD_W21_WriteDATA2(datas[170*j+i]);}  

    EPD_Update();     
}
void EPD_WhiteScreen_ALL_P(const unsigned char *oldDatas,const unsigned char *newDatas)
{
    unsigned int i,j;
    //Master
    EPD_W21_WriteCMD1(0x10);         //Transfer old data 
    if(partFlag==1){ 
          for(j=0;j<480;j++)
          for(i=0;i<85;i++){  
          EPD_W21_WriteDATA1(0xFF);}
        }
    else{
          for(j=0;j<480;j++)
          for(i=0;i<85;i++){    
          EPD_W21_WriteDATA1(oldDatas[170*j+i]);}  
          }   
      
    EPD_W21_WriteCMD1(0x13);         //Transfer old data
    for(j=0;j<480;j++)
    for(i=0;i<85;i++){    
      EPD_W21_WriteDATA1(newDatas[170*j+i]);}  

    //Slave
    EPD_W21_WriteCMD2(0x10);         //Transfer old data
    if(partFlag==1){ 
          partFlag=0;
          for(j=0;j<480;j++)
          for(i=85;i<85*2;i++){ 
          EPD_W21_WriteDATA2(0xFF);}
        }
    else{
          for(j=0;j<480;j++)
          for(i=85;i<85*2;i++){   
          EPD_W21_WriteDATA2(oldDatas[170*j+i]);}  
          }   
    EPD_W21_WriteCMD2(0x13);         //Transfer old data
    for(j=0;j<480;j++)
    for(i=85;i<85*2;i++){   
      EPD_W21_WriteDATA2(newDatas[170*j+i]);}    
  
    EPD_Update();     


}


void EPD_WhiteScreen_White(void)
{
    unsigned int i;
   //Master
   EPD_W21_WriteCMD1(0x10);        //Transfer old data
   for(i=0;i<EPD_ARRAY;i++){    
    EPD_W21_WriteDATA1(0xFF);}
   EPD_W21_WriteCMD1(0x13);        //Transfer old data
   for(i=0;i<EPD_ARRAY;i++){    
    EPD_W21_WriteDATA1(0xFF);}
   //Slave
   EPD_W21_WriteCMD2(0x10);        //Transfer old data
   for(i=0;i<EPD_ARRAY;i++){    
    EPD_W21_WriteDATA2(0xFF);}
   EPD_W21_WriteCMD2(0x13);        //Transfer old data
   for(i=0;i<EPD_ARRAY;i++){    
    EPD_W21_WriteDATA2(0xFF);}
   
    EPD_Update();     
}
void EPD_WhiteScreen_Black(void)
{
    unsigned int i;
   //Master
   EPD_W21_WriteCMD1(0x10);        //Transfer old data
   for(i=0;i<EPD_ARRAY;i++){    
    EPD_W21_WriteDATA1(0xFF);}
   EPD_W21_WriteCMD1(0x13);        //Transfer old data
   for(i=0;i<EPD_ARRAY;i++){    
    EPD_W21_WriteDATA1(0x00);}
   //Slave
   EPD_W21_WriteCMD2(0x10);        //Transfer old data
   for(i=0;i<EPD_ARRAY;i++){    
    EPD_W21_WriteDATA2(0xFF);}
   EPD_W21_WriteCMD2(0x13);        //Transfer old data
   for(i=0;i<EPD_ARRAY;i++){    
    EPD_W21_WriteDATA2(0x00);}
   
    EPD_Update();     
}

////////////////////////////////Other newly added functions////////////////////////////////////////////
//Display rotation 180 degrees initialization
void EPD_Init_180(void)
{ 
    EPD_W21_RST_0;    // Module reset
    delay(10);//At least 10ms delay 
    EPD_W21_RST_1;
    delay(10);//At least 10ms delay 
    lcd_chkstatus();

    EPD_W21_WriteCMD(0x06);         //boost soft start
    EPD_W21_WriteDATA (0x17);   //A
    EPD_W21_WriteDATA (0x17);   //B
    EPD_W21_WriteDATA (0x17);   //C       

    EPD_W21_WriteCMD(0x04);  
    lcd_chkstatus();//waiting for the electronic paper IC to release the idle signal

    EPD_W21_WriteCMD(0x00);     //panel setting
    EPD_W21_WriteDATA(0x13);    //LUT from OTP£¬128x296
  
    EPD_W21_WriteCMD(0x16);
    EPD_W21_WriteDATA(0x00);        //KW-BF   KWR-AF  BWROTP 0f 
    
    EPD_W21_WriteCMD(0xF8);         //boost setting
    EPD_W21_WriteDATA (0x60);
    EPD_W21_WriteDATA(0xa5);

    EPD_W21_WriteCMD(0xF8);         //boost setting
    EPD_W21_WriteDATA (0x73);
    EPD_W21_WriteDATA (0x23);

    EPD_W21_WriteCMD(0xF8);         //boost setting
    EPD_W21_WriteDATA (0x7C);
    EPD_W21_WriteDATA(0x00);
    
    EPD_W21_WriteCMD(0X50);
    EPD_W21_WriteDATA(0x97);    //WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}

void EPD_Display(unsigned char *Image)
{
    unsigned int Width, Height,i,j;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_W21_WriteCMD(0x10);
    for (j = 0; j < Height; j++) {
        for ( i = 0; i < Width; i++) {
            EPD_W21_WriteDATA(0xff);
        }
    }

    EPD_W21_WriteCMD(0x13);
    for ( j = 0; j < Height; j++) {
        for ( i = 0; i < Width; i++) {
           EPD_W21_WriteDATA(Image[i + j * Width]);
        }
    }
    EPD_Update();      
}









/***********************************************************
            end file
***********************************************************/
