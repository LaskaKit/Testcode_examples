/* Display test for Good Display GDEY075T7
 * example from Good Display is used
 * 
 * Board:           LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display:         Good Display GDEY075T7          https://www.laskakit.cz/good-display-gdey075t7-7-5--800x480-epaper-displej-grayscale/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// Display Library example for SPI e-paper panels from Dalian Good Display.

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
////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);     
void SPI_Delay(unsigned char xrate);
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);
//EPD
void EPD_W21_Init(void);
void EPD_init(void);
void PIC_display1(void);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clean(void);
unsigned char HRES,VRES_byte1,VRES_byte2;

void setup() {

  // turn on power to display
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("Display power ON");
  delay(500);   

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
  while(1)
  {
    //PICTURE1
    EPD_init_4Gray(); //EPD init 4Gray
    pic_display_4bit ();
    EPD_refresh();//EPD_refresh   
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(3000);
    
    //PICTURE Clean
    EPD_init(); //EPD init
    PIC_display_Clean();
    EPD_refresh();//EPD_refresh   
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    while(1);

  }
}




///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
/////////////////////delay//////////////////////////////////////
void driver_delay_us(unsigned int xus)  //1us
{
  for(;xus>1;xus--);
}
void driver_delay_xms(unsigned long xms) //1ms
{  
    unsigned long i = 0 , j=0;

    for(j=0;j<xms;j++)
  {
        for(i=0; i<256; i++);
    }
}
void DELAY_S(unsigned int delaytime)     
{
  int i,j,k;
  for(i=0;i<delaytime;i++)
  {
    for(j=0;j<4000;j++)           
    {
      for(k=0;k<222;k++);
                
    }
  }
}
//////////////////////SPI///////////////////////////////////
void SPI_Delay(unsigned char xrate)
{
  unsigned char i;
  while(xrate)
  {
    for(i=0;i<2;i++);
    xrate--;
  }
}


void SPI_Write(unsigned char value)                                    
{                                                           
SPI.transfer(value);
}

void EPD_W21_WriteCMD(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void EPD_W21_WriteDATA(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}



/////////////////EPD settings Functions/////////////////////
void EPD_W21_Init(void)
{
  EPD_W21_RST_0;    // Module reset
  delay(100); //At least 10ms
  EPD_W21_RST_1;
  delay(100);  
}
void EPD_init(void)
{
    unsigned char HRES_byte1=0x03;      //800
    unsigned char HRES_byte2=0x20;
    unsigned char VRES_byte1=0x01;      //480
    unsigned char  VRES_byte2=0xE0;
  
    EPD_W21_Init(); //Electronic paper IC reset

    EPD_W21_WriteCMD(0x01);     //POWER SETTING
    EPD_W21_WriteDATA (0x07);
    EPD_W21_WriteDATA (0x07);    //VGH=20V,VGL=-20V
    EPD_W21_WriteDATA (0x3f);   //VDH=15V
    EPD_W21_WriteDATA (0x3f);   //VDL=-15V

    //Enhanced display drive(Add 0x06 command)
    EPD_W21_WriteCMD(0x06);     //Booster Soft Start 
    EPD_W21_WriteDATA (0x17);
    EPD_W21_WriteDATA (0x17);   
    EPD_W21_WriteDATA (0x28);   
    EPD_W21_WriteDATA (0x17); 
    
    EPD_W21_WriteCMD(0x04);  //Power on
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal
  
    EPD_W21_WriteCMD(0X00);     //PANNEL SETTING
    EPD_W21_WriteDATA(0x1F);   //KW-3f   KWR-2F BWROTP 0f BWOTP 1f

    EPD_W21_WriteCMD(0x61);         //tres      
    EPD_W21_WriteDATA (HRES_byte1);   //source 800
    EPD_W21_WriteDATA (HRES_byte2);
    EPD_W21_WriteDATA (VRES_byte1);   //gate 480
    EPD_W21_WriteDATA (VRES_byte2);
  
    EPD_W21_WriteCMD(0X15);   
    EPD_W21_WriteDATA(0x00);    

    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x10);
    EPD_W21_WriteDATA(0x07);

    EPD_W21_WriteCMD(0X60);     //TCON SETTING
    EPD_W21_WriteDATA(0x22);
}
void EPD_refresh(void)
{
    EPD_W21_WriteCMD(0x12);     //DISPLAY REFRESH   
    driver_delay_xms(100);          //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();
} 
void EPD_sleep(void)
{
    EPD_W21_WriteCMD(0X50);  //VCOM AND DATA INTERVAL SETTING  
    EPD_W21_WriteDATA(0xf7);
       
    EPD_W21_WriteCMD(0X02);   //power off
    lcd_chkstatus();
    EPD_W21_WriteCMD(0X07);   //deep sleep
    EPD_W21_WriteDATA(0xA5);
}


void PIC_display1(void)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<48000;i++)      
    EPD_W21_WriteDATA(0xff); 
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<48000;i++)      
     EPD_W21_WriteDATA(pgm_read_byte(&gImage_1[i]));   
}

void PIC_display_Clean(void)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<48000;i++)       
  {
    EPD_W21_WriteDATA(0xff);
  }
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data    
    for(i=0;i<48000;i++)       
  {
    EPD_W21_WriteDATA(0x00);
  }
}
void lcd_chkstatus(void)
{
  unsigned char busy;
  do
  {
    EPD_W21_WriteCMD(0x71);
    busy = isEPD_W21_BUSY;
    busy =!(busy & 0x01);        
  }
  while(busy);   
  driver_delay_xms(200);                       
}




void EPD_init_4Gray(void)
{
    unsigned char HRES_byte1=0x03;      //800
    unsigned char HRES_byte2=0x20;
    unsigned char VRES_byte1=0x01;      //480
    unsigned char VRES_byte2=0xE0;
  
    EPD_W21_Init();   

    EPD_W21_WriteCMD(0x01);     //POWER SETTING
    EPD_W21_WriteDATA (0x07);
    EPD_W21_WriteDATA (0x17);   
    EPD_W21_WriteDATA (0x3f);   
    EPD_W21_WriteDATA (0x3f); 

    //Enhanced display drive(Add 0x06 command)
    EPD_W21_WriteCMD(0x06);     //Booster Soft Start 
    EPD_W21_WriteDATA (0x17);
    EPD_W21_WriteDATA (0x17);   
    EPD_W21_WriteDATA (0x28);   
    EPD_W21_WriteDATA (0x17); 
    
    EPD_W21_WriteCMD(0x04); //POWER ON
    driver_delay_xms(100);  
    lcd_chkstatus();
  
    EPD_W21_WriteCMD(0X00);     //PANNEL SETTING
    EPD_W21_WriteDATA(0xbF);   //KW-Bf   KWR-AF BWROTP 0f BWOTP 1f

    EPD_W21_WriteCMD(0x30);     //PLL setting
    EPD_W21_WriteDATA (0x06);       //50hz 
    
    EPD_W21_WriteCMD(0x61);         //tres      
    EPD_W21_WriteDATA (HRES_byte1);   //source 800
    EPD_W21_WriteDATA (HRES_byte2);
    EPD_W21_WriteDATA (VRES_byte1);   //gate 480
    EPD_W21_WriteDATA (VRES_byte2);
  
    EPD_W21_WriteCMD(0X15);   
    EPD_W21_WriteDATA(0x00);  

    EPD_W21_WriteCMD(0X60);     //TCON SETTING
    EPD_W21_WriteDATA(0x22);
    
    EPD_W21_WriteCMD(0x82);     //vcom_DC setting
    EPD_W21_WriteDATA (0x12);

    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x10);  //10:KW(0--1)  21:KW(1--0)
    EPD_W21_WriteDATA(0x07);


  }
  //0~3 gray
const unsigned char lut_vcom[] =
{
0x00  ,0x0A ,0x00 ,0x00 ,0x00 ,0x01,
0x60  ,0x14 ,0x14 ,0x00 ,0x00 ,0x01,
0x00  ,0x14 ,0x00 ,0x00 ,0x00 ,0x01,
0x00  ,0x13 ,0x0A ,0x01 ,0x00 ,0x01,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
        
};
//R21
const unsigned char lut_ww[] ={
0x40  ,0x0A ,0x00 ,0x00 ,0x00 ,0x01,
0x90  ,0x14 ,0x14 ,0x00 ,0x00 ,0x01,
0x10  ,0x14 ,0x0A ,0x00 ,0x00 ,0x01,
0xA0  ,0x13 ,0x01 ,0x00 ,0x00 ,0x01,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};
//R22H  r
const unsigned char lut_bw[] ={
0x40  ,0x0A ,0x00 ,0x00 ,0x00 ,0x01,
0x90  ,0x14 ,0x14 ,0x00 ,0x00 ,0x01,
0x00  ,0x14 ,0x0A ,0x00 ,0x00 ,0x01,
0x99  ,0x0C ,0x01 ,0x03 ,0x04 ,0x01,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};
//R23H  w
const unsigned char lut_wb[] ={
0x40  ,0x0A ,0x00 ,0x00 ,0x00 ,0x01,
0x90  ,0x14 ,0x14 ,0x00 ,0x00 ,0x01,
0x00  ,0x14 ,0x0A ,0x00 ,0x00 ,0x01,
0x99  ,0x0B ,0x04 ,0x04 ,0x01 ,0x01,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};
//R24H  b
const unsigned char lut_bb[] ={
0x80  ,0x0A ,0x00 ,0x00 ,0x00 ,0x01,
0x90  ,0x14 ,0x14 ,0x00 ,0x00 ,0x01,
0x20  ,0x14 ,0x0A ,0x00 ,0x00 ,0x01,
0x50  ,0x13 ,0x01 ,0x00 ,0x00 ,0x01,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
};
//LUT download
void lut(void)
{
  unsigned int count;  
  {
    EPD_W21_WriteCMD(0x20);             //vcom
    for(count=0;count<44;count++)
      {EPD_W21_WriteDATA(lut_vcom[count]);}
    
  EPD_W21_WriteCMD(0x21);             //red not use
  for(count=0;count<42;count++)
    {EPD_W21_WriteDATA(lut_ww[count]);}

    EPD_W21_WriteCMD(0x22);             //bw r
    for(count=0;count<42;count++)
      {EPD_W21_WriteDATA(lut_bw[count]);}

    EPD_W21_WriteCMD(0x23);             //wb w
    for(count=0;count<42;count++)
      {EPD_W21_WriteDATA(lut_wb[count]);}

    EPD_W21_WriteCMD(0x24);             //bb b
    for(count=0;count<42;count++)
      {EPD_W21_WriteDATA(lut_bb[count]);}

    EPD_W21_WriteCMD(0x25);             //vcom
    for(count=0;count<42;count++)
      {EPD_W21_WriteDATA(lut_ww[count]);}
  }          
}

  //4 grayscale demo function
/********Color display description
      white  gray1  gray2  black
0x10|  01     01     00     00
0x13|  01     00     01     00
                                   ****************/
void pic_display_4bit (void)
{
  int i,j;
  unsigned char temp1,temp2,temp3;

    //old  data
    EPD_W21_WriteCMD(0x10);        

    for(i=0;i<48000;i++)                 //48000*4  800*480
    { 
      temp3=0;
      for(j=0;j<4;j++)  
      {
        temp1 = gImage_1[i*4+j];
        temp2 = temp1&0xF0 ;
        if(temp2 == 0xF0)
          temp3 |= 0x01;//white
        else if(temp2 == 0x00)
          temp3 |= 0x00;  //black
        else if((temp2>0xA0)&&(temp2<0xF0)) 
          temp3 |= 0x01;  //gray1
        else 
          temp3 |= 0x00; //gray2
        temp3 <<= 1;  
        temp1 <<= 4;
        temp2 = temp1&0xF0 ;
        if(temp2 == 0xF0)  //white
          temp3 |= 0x01;
        else if(temp2 == 0x00) //black
          temp3 |= 0x00;
        else if((temp2>0xA0)&&(temp2<0xF0))
          temp3 |= 0x01; //gray1
        else    
            temp3 |= 0x00;  //gray2 
        if(j!=3)          
        temp3 <<= 1;  
        
      
     }  
        EPD_W21_WriteDATA(temp3);     
    }
    //new  data
    EPD_W21_WriteCMD(0x13);        

    for(i=0;i<48000;i++)                 //48000*4   800*480
    { 
      temp3=0;
      for(j=0;j<4;j++)  
      {
        temp1 = gImage_1[i*4+j];
        temp2 = temp1&0xF0 ;
        if(temp2 == 0xF0)
          temp3 |= 0x01;//white
        else if(temp2 == 0x00)
          temp3 |= 0x00;  //black
        else if((temp2>0xA0)&&(temp2<0xF0)) 
          temp3 |= 0x00;  //gray1
        else 
          temp3 |= 0x01; //gray2
        temp3 <<= 1;  
        temp1 <<= 4;
        temp2 = temp1&0xF0 ;
        if(temp2 == 0xF0)  //white
          temp3 |= 0x01;
        else if(temp2 == 0x00) //black
          temp3 |= 0x00;
        else if((temp2>0xA0)&&(temp2<0xF0)) 
          temp3 |= 0x00;//gray1
        else    
            temp3 |= 0x01;  //gray2
        if(j!=3)        
        temp3 <<= 1;        
      
     }  
        EPD_W21_WriteDATA(temp3);     
    }
    lut(); //Power settings
}
