#include <SPI.h>
#include"Ap_29demo.h"

//IO settings
int BUSY_Pin = 4;
int RES_Pin = 16;
int DC_Pin = 17;
int CS_Pin = 5;
//SCLK--GPIO18
//MOSI---GPIO23

#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)

//2bit
#define black   0x00  /// 00
#define white   0x01  /// 01
#define yellow  0x02  /// 10
#define red     0x03  /// 11


#define Source_BITS     184
#define Gate_BITS   360
#define ALLSCREEN_BYTES   Source_BITS*Gate_BITS/4

////////FUNCTION//////   
void SPI_Write(unsigned char value);
void EPD_W21_WriteCMD(unsigned char command);
void EPD_W21_WriteDATA(unsigned char datas);
//EPD
//EPD
void EPD_init(void);
void Acep_color(unsigned char color);
void PIC_display (const unsigned char* picData);
void Display_All_White(void);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);


void setup() {
  
// turn on power to display
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("Display power ON");
  delay(1000);   

   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);    
   //SPI
   SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
   SPI.begin ();  
}

//Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */
  
void loop() {

  ///////////////////////////Normal picture display/////////////////////////////////////////////////////////////////
    /************Normal picture display*******************/
    EPD_init(); //EPD init
    PIC_display(gImage_1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(5000); //5s  
    
    EPD_init(); //EPD init
    Display_All_White();
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    while(1);   
  
}












///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
/////////////////////delay//////////////////////////////////////

//////////////////////SPI///////////////////////////////////

void SPI_Write(unsigned char value)                                    
{                                                           
  SPI.transfer(value);
}

void EPD_W21_WriteCMD(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void EPD_W21_WriteDATA(unsigned char datas)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(datas);
  EPD_W21_CS_1;
}

/////////////////EPD settings Functions/////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//JD
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
//////////////////////////////All screen update////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////
void  EPD_refresh(void)
{
  EPD_W21_WriteCMD(0x12); //Display Update Control
  EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();   
}

void EPD_sleep(void)
{  
  EPD_W21_WriteCMD(0X02);   //power off
  EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();          //waiting for the electronic paper IC to release the idle sign   
 
  EPD_W21_WriteCMD(0X07);   //deep sleep
  EPD_W21_WriteDATA(0xA5);
}
void lcd_chkstatus(void)
{ 
  while(1)
  {   //=0 BUSY
     if(isEPD_W21_BUSY==1) break;
  }  
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
  unsigned char data_H1,data_H2,data_L1,data_L2,datas;
   
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
      
      datas=data_H1|data_H2|data_L1|data_L2;
      EPD_W21_WriteDATA(datas);
    }
  } 
  
   //Refresh
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


//////////////////////////////////END//////////////////////////////////////////////////
