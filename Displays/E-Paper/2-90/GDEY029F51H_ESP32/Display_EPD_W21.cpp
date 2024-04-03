#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

void EPD_init(void)
{
  delay(20);//At least 20ms delay   
  EPD_W21_RST_0;    // Module reset
  delay(40);//At least 40ms delay 
  EPD_W21_RST_1;
  delay(50);//At least 50ms delay 
  
  lcd_chkstatus();
  EPD_W21_WriteCMD(0x4D);
  EPD_W21_WriteDATA(0x78);

  EPD_W21_WriteCMD(0x00); //PSR
  EPD_W21_WriteDATA(0x0F);
  EPD_W21_WriteDATA(0x29);

  EPD_W21_WriteCMD(0x01); //PWRR
  EPD_W21_WriteDATA(0x07);
  EPD_W21_WriteDATA(0x00);
  
  EPD_W21_WriteCMD(0x03); //POFS
  EPD_W21_WriteDATA(0x10);
  EPD_W21_WriteDATA(0x54);
  EPD_W21_WriteDATA(0x44);
  
  EPD_W21_WriteCMD(0x06); //BTST_P
  EPD_W21_WriteDATA(0x05);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x3F);
  EPD_W21_WriteDATA(0x0A);
  EPD_W21_WriteDATA(0x25);
  EPD_W21_WriteDATA(0x12);
  EPD_W21_WriteDATA(0x1A); 

  EPD_W21_WriteCMD(0x50); //CDI
  EPD_W21_WriteDATA(0x37);
  
  EPD_W21_WriteCMD(0x60); //TCON
  EPD_W21_WriteDATA(0x02);
  EPD_W21_WriteDATA(0x02);
  
  EPD_W21_WriteCMD(0x61); //TRES
  EPD_W21_WriteDATA(Source_BITS/256);   // Source_BITS_H
  EPD_W21_WriteDATA(Source_BITS%256);   // Source_BITS_L
  EPD_W21_WriteDATA(Gate_BITS/256);     // Gate_BITS_H
  EPD_W21_WriteDATA(Gate_BITS%256);     // Gate_BITS_L  
  
  EPD_W21_WriteCMD(0xE7);
  EPD_W21_WriteDATA(0x1C);
  
  EPD_W21_WriteCMD(0xE3); 
  EPD_W21_WriteDATA(0x22);
  
  EPD_W21_WriteCMD(0xB4);
  EPD_W21_WriteDATA(0xD0);
  EPD_W21_WriteCMD(0xB5);
  EPD_W21_WriteDATA(0x03);
  
  EPD_W21_WriteCMD(0xE9);
  EPD_W21_WriteDATA(0x01); 

  EPD_W21_WriteCMD(0x30);
  EPD_W21_WriteDATA(0x08);  
  
  EPD_W21_WriteCMD(0x04);
  lcd_chkstatus();   
}
  
void EPD_sleep(void)
{   
  EPD_W21_WriteCMD(0X02);   //power off
  lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
  delay(100);   //!!!The delay here is necessary,100mS at least!!!     
 
  EPD_W21_WriteCMD(0X07);   //deep sleep
  EPD_W21_WriteDATA(0xA5);
}
void EPD_refresh(void)
{   
  EPD_W21_WriteCMD(0x12); //Display Update Control
  EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();   
}

void lcd_chkstatus(void)
{ 
  while(1)
  {  //=0 BUSY
     if(isEPD_W21_BUSY==1) break;
  }  
}


void Display_All_Black(void)
{
  unsigned long i; 

  EPD_W21_WriteCMD(0x10);
  {
    for(i=0;i<ALLSCREEN_BYTES;i++)
    {
      EPD_W21_WriteDATA(0x00);
    }
  } 
  EPD_refresh();  
  
}

void Display_All_White(void)
{
  unsigned long i;
 
  EPD_W21_WriteCMD(0x10);
  {
    for(i=0;i<ALLSCREEN_BYTES;i++)
    {
      EPD_W21_WriteDATA(0x55);
    }
  } 
   EPD_refresh(); 
}

void Display_All_Yellow(void)
{
  unsigned long i;
 
  EPD_W21_WriteCMD(0x10);
  {
    for(i=0;i<ALLSCREEN_BYTES;i++)
    {
      EPD_W21_WriteDATA(0xaa);
    }
  }
   EPD_refresh(); 
}


void Display_All_Red(void)
{
  unsigned long i;
 
  EPD_W21_WriteCMD(0x10);
  {
    for(i=0;i<ALLSCREEN_BYTES;i++)
    {
      EPD_W21_WriteDATA(0xff);
    }
  } 
   EPD_refresh(); 
}



unsigned char Color_get(unsigned char color)
{
  unsigned datas;
  switch(color)
  {
    case 0x00:
      datas=white;  
      break;    
    case 0x01:
      datas=yellow;
      break;
    case 0x02:
      datas=red;
      break;    
    case 0x03:
      datas=black;
      break;      
    default:
      break;      
  }
   return datas;
}



void PIC_display(const unsigned char* picData)
{
  unsigned int i,j;
  unsigned char temp1;
  unsigned char data_H1,data_H2,data_L1,data_L2,data;
   
  EPD_W21_WriteCMD(0x10);        
  for(i=0;i<Gate_BITS;i++)  //Source_BITS*Gate_BITS/4
  { 
    for(j=0;j<Source_BITS/4;j++)
    {   
      temp1=picData[i*Source_BITS/4+j]; 

      data_H1=Color_get(temp1>>6&0x03)<<6;      
      data_H2=Color_get(temp1>>4&0x03)<<4;
      data_L1=Color_get(temp1>>2&0x03)<<2;
      data_L2=Color_get(temp1&0x03);
      
      data=data_H1|data_H2|data_L1|data_L2;
      EPD_W21_WriteDATA(data);
    }
  } 
  
   //Refresh
    EPD_refresh();  
}






/***********************************************************
            end file
***********************************************************/
