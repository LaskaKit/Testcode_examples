//#include "calendar.h"
#include "price.h"
//#include "food.h"

//IO settings
int EPD_W21_SDA = 11; 
int EPD_W21_SCL =12;
int EPD_W21_CS_S2 = 41; 
int EPD_W21_CS_M1 = 10; 
int EPD_W21_RST_M2 = 39; 
int EPD_W21_RST_M1 = 7; 
int EPD_W21_DC_M2 = 6; 
int EPD_W21_DC_M1 = 15; 
int EPD_W21_CS_M2 = 48; 
int EPD_W21_CS_S1 = 18;

//BUSY 
int EPD_W21_BUSY_M1=8;

//CS
#define EPD_W21_CS_M1_0  ( digitalWrite(EPD_W21_CS_M1,LOW)) 
#define EPD_W21_CS_M1_1  (digitalWrite(EPD_W21_CS_M1,HIGH))

#define EPD_W21_CS_S1_0  (digitalWrite(EPD_W21_CS_S1,LOW))
#define EPD_W21_CS_S1_1  (digitalWrite(EPD_W21_CS_S1,HIGH))

#define EPD_W21_CS_M2_0  (digitalWrite(EPD_W21_CS_M2,LOW))
#define EPD_W21_CS_M2_1  (digitalWrite(EPD_W21_CS_M2,HIGH))

#define EPD_W21_CS_S2_0  (digitalWrite(EPD_W21_CS_S2,LOW))
#define EPD_W21_CS_S2_1  (digitalWrite(EPD_W21_CS_S2,HIGH))

//RST
#define EPD_W21_RST_M1_0  (digitalWrite(EPD_W21_RST_M1,LOW))
#define EPD_W21_RST_M1_1  (digitalWrite(EPD_W21_RST_M1,HIGH))

#define EPD_W21_RST_M2_0  (digitalWrite(EPD_W21_RST_M2,LOW)) 
#define EPD_W21_RST_M2_1  (digitalWrite(EPD_W21_RST_M2,HIGH))
//DC
#define EPD_W21_DC_M1_0  (digitalWrite(EPD_W21_DC_M1,LOW))
#define EPD_W21_DC_M1_1  (digitalWrite(EPD_W21_DC_M1,HIGH))

#define EPD_W21_DC_M2_0  (digitalWrite(EPD_W21_DC_M2,LOW))
#define EPD_W21_DC_M2_1  (digitalWrite(EPD_W21_DC_M2,HIGH))
//SCL
#define EPD_W21_SCL_0   (digitalWrite(EPD_W21_SCL,LOW))
#define EPD_W21_SCL_1   (digitalWrite(EPD_W21_SCL,HIGH))
//SDA
#define EPD_W21_SDA_0   (digitalWrite(EPD_W21_SDA,LOW))
#define EPD_W21_SDA_1   (digitalWrite(EPD_W21_SDA,HIGH))

//read SDA
#define EPD_W21_ReadDATA_SDA (digitalRead(EPD_W21_SDA)) 
//BUSY
#define EPD_W21_ReadDATA_M1 (digitalRead(EPD_W21_BUSY_M1)) 


////////FUNCTION//////
void SPI_Write(unsigned char value);
void EPD_W21_WriteCMD_M1(unsigned char command);
void EPD_W21_WriteCMD_S1(unsigned char command);
void EPD_W21_WriteCMD_M2(unsigned char command);
void EPD_W21_WriteCMD_S2(unsigned char command);
void EPD_W21_WriteDATA_M1(unsigned char command);
void EPD_W21_WriteDATA_S1(unsigned char command);
void EPD_W21_WriteDATA_M2(unsigned char command);
void EPD_W21_WriteDATA_S2(unsigned char command);
void EPD_W21_WriteCMD_ALL(unsigned char command);
void EPD_W21_WriteDATA_ALL(unsigned char command);
void EPD_W21_WriteCMD_M1M2(unsigned char command);
void EPD_W21_WriteDATA_M1M2(unsigned char command);
//EPD init
void EPD_W21_Init(void);
//EPD Read
unsigned char EPD_W21_ReadDATA_M1_temp(void);

//EPD
void EPD_init(unsigned char mode);
void pic_display(void);
void pic_display_white(void);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus_M1(void);
void lcd_chkstatus_S1(void);
void lcd_chkstatus_M2(void);
void lcd_chkstatus_S2(void);

void setup() {
   pinMode(EPD_W21_SDA, OUTPUT);  
   pinMode(EPD_W21_SCL, OUTPUT);    
   pinMode(EPD_W21_CS_S2, OUTPUT);    
   pinMode(EPD_W21_CS_M1, OUTPUT);    
   pinMode(EPD_W21_RST_M2, OUTPUT);    
   pinMode(EPD_W21_RST_M1, OUTPUT);
   pinMode(EPD_W21_DC_M2, OUTPUT);
   pinMode(EPD_W21_DC_M1, OUTPUT);
   pinMode(EPD_W21_CS_M2, OUTPUT);
   pinMode(EPD_W21_CS_S1, OUTPUT);

   pinMode(EPD_W21_BUSY_M1, INPUT);
  
   
}


//Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */


void loop() {  
  while(1)
  {
      //picture1
      EPD_init(0); //EPD init    
      pic_display();//picture1
      EPD_refresh();//EPD refresh
      EPD_sleep();  //EPD sleep
      delay(5000);
      //all white
      //EPD_init(1); //EPD init    
      //pic_display_white();//picture  white
      //EPD_refresh();//EPD refresh
      //EPD_sleep();  //EPD sleep
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
  unsigned char i;
  for(i=0; i<8; i++)
  {
    EPD_W21_SCL_0;
    SPI_Delay(1);
      if(value & 0x80)
        EPD_W21_SDA_1;
      else
        EPD_W21_SDA_0;
      value = (value << 1);
    SPI_Delay(1);
    EPD_W21_SCL_1;
    SPI_Delay(1);
  }

}

void EPD_W21_WriteCMD_M1(unsigned char command)
{
  EPD_W21_CS_M1_0;
  EPD_W21_DC_M1_0;    // command write
  SPI_Write(command);
  EPD_W21_CS_M1_1;
}


void EPD_W21_WriteDATA_M1(unsigned char command)
{
  EPD_W21_CS_M1_0;
  EPD_W21_DC_M1_1;    // command write
  SPI_Write(command);
  EPD_W21_CS_M1_1;
}


void EPD_W21_WriteCMD_S1(unsigned char command)
{
  EPD_W21_CS_S1_0;
  EPD_W21_DC_M1_0;    // command write
  SPI_Write(command);
  EPD_W21_CS_S1_1;

}


void EPD_W21_WriteDATA_S1(unsigned char command)
{
  EPD_W21_CS_S1_0;
  EPD_W21_DC_M1_1;    // command write
  SPI_Write(command);
  EPD_W21_CS_S1_1;
}






void EPD_W21_WriteCMD_ALL(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_M1_0;
  EPD_W21_CS_S1_0;
  EPD_W21_CS_M2_0;
  EPD_W21_CS_S2_0;
  SPI_Delay(1);
  EPD_W21_DC_M1_0;    // command write
  EPD_W21_DC_M2_0;    // command write
  SPI_Delay(1);
  SPI_Write(command);
  SPI_Delay(1);
  EPD_W21_CS_S2_1;
  EPD_W21_CS_M2_1;
  EPD_W21_CS_S1_1;
  EPD_W21_CS_M1_1;

}


void EPD_W21_WriteDATA_ALL(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_M1_0;
  EPD_W21_CS_S1_0;
  EPD_W21_CS_M2_0;
  EPD_W21_CS_S2_0;
  SPI_Delay(1);
  EPD_W21_DC_M1_1;    // command write
  EPD_W21_DC_M2_1;    // command write
  SPI_Delay(1);
  SPI_Write(command);
  SPI_Delay(1);
  EPD_W21_CS_S2_1;
  EPD_W21_CS_M2_1;
  EPD_W21_CS_S1_1;
  EPD_W21_CS_M1_1;
}



void EPD_W21_WriteCMD_M1M2(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_M1_0;
  EPD_W21_CS_M2_0;
  SPI_Delay(1);
  EPD_W21_DC_M1_0;    // command write
  EPD_W21_DC_M2_0;    // command write
  SPI_Delay(1);
  SPI_Write(command);
  SPI_Delay(1);
  EPD_W21_CS_M1_1;
  EPD_W21_CS_M2_1;
}


void EPD_W21_WriteDATA_M1M2(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_M1_0;
  EPD_W21_CS_M2_0;
  SPI_Delay(1);
  EPD_W21_DC_M1_1;    // command write
  EPD_W21_DC_M2_1;    // command write
  SPI_Delay(1);
  SPI_Write(command);
  SPI_Delay(1);
  EPD_W21_CS_M1_1;
  EPD_W21_CS_M2_1;
}


void EPD_W21_WriteCMD_M2(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_M2_0;
  SPI_Delay(1);
  EPD_W21_DC_M2_0;    // command write
  SPI_Delay(1);
  SPI_Write(command);
  SPI_Delay(1);
  EPD_W21_CS_M2_1;
}


void EPD_W21_WriteDATA_M2(unsigned char command)
{

  EPD_W21_CS_M2_0;
  EPD_W21_DC_M2_1;    // command write
  SPI_Write(command);
  EPD_W21_CS_M2_1;
}




void EPD_W21_WriteCMD_S2(unsigned char command)
{
  EPD_W21_CS_S2_0;
  EPD_W21_DC_M2_0;    // command write
  SPI_Write(command);
  EPD_W21_CS_S2_1;
}


void EPD_W21_WriteDATA_S2(unsigned char command)
{
  EPD_W21_CS_S2_0;
  EPD_W21_DC_M2_1;    // command write
  SPI_Write(command);
  EPD_W21_CS_S2_1;
}






void GPIO_IO(unsigned char i)//GPIO mode flip  0:INPUT,1:OUTPUT
{
  if(i==0)
  {   
    //SDA IN
    pinMode(EPD_W21_SDA, INPUT);    
  }
  else
  {
    //SDA OUT
    pinMode(EPD_W21_SDA, OUTPUT);    
  }
}
//Read data for EPD
unsigned char EPD_W21_ReadDATA_M1_temp(void)
{
  unsigned char i,j;
  GPIO_IO(0); //GPIO mode flip  0:INPUT,1:OUTPUT
  EPD_W21_CS_M1_0;
  EPD_W21_CS_S1_1;
  EPD_W21_CS_M2_1;
  EPD_W21_CS_S2_1;
  EPD_W21_DC_M1_1;    // command write
  SPI_Delay(1);

  j=0;

  SPI_Delay(1);
   for(i=0; i<8; i++)
  {
     EPD_W21_SCL_0;
    SPI_Delay(1);
    j=(j<<1);
    if(EPD_W21_ReadDATA_SDA==1)
      j|=0x01;
    else
      j&=0xfe;
    SPI_Delay(1);
    EPD_W21_SCL_1;
    SPI_Delay(1);
  }

   EPD_W21_CS_M1_1;
   GPIO_IO(1); //GPIO mode flip  0:INPUT,1:OUTPUT

   return j;
}




















/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void EPD_W21_Init(void)
{
  EPD_W21_CS_M1_1;
  EPD_W21_CS_S1_1;
  EPD_W21_CS_M2_1;
  EPD_W21_CS_S2_1;

  EPD_W21_RST_M1_0;
  EPD_W21_RST_M2_0;
  delay(100);
  EPD_W21_RST_M1_1;
  EPD_W21_RST_M2_1;
  delay(100);
  
}
void EPD_init(unsigned char mode) //0:Brush drawing  1£ºClear screen in solid color
{
  unsigned char temp;
  EPD_W21_Init();
  if(mode==0)
  {
    EPD_W21_WriteCMD_M1(0x00);      //panel setting
    EPD_W21_WriteDATA_M1(0x0f);   //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f
    EPD_W21_WriteCMD_S1(0x00);      //panel setting
    EPD_W21_WriteDATA_S1(0x0f);   //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f
    //M2S2 turn  180
    EPD_W21_WriteCMD_M2(0x00);      //panel setting
    EPD_W21_WriteDATA_M2(0x03);   //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f
    EPD_W21_WriteCMD_S2(0x00);      //panel setting
    EPD_W21_WriteDATA_S2(0x03);   //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f
  }
  else
  {
    EPD_W21_WriteCMD_M1(0x00);      //panel setting
    EPD_W21_WriteDATA_M1(0x07);   //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f
    EPD_W21_WriteCMD_S1(0x00);      //panel setting
    EPD_W21_WriteDATA_S1(0x07);   //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f
    //M2S2 turn  180
    EPD_W21_WriteCMD_M2(0x00);      //panel setting
    EPD_W21_WriteDATA_M2(0x07);   //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f
    EPD_W21_WriteCMD_S2(0x00);      //panel setting
    EPD_W21_WriteDATA_S2(0x07);   //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f   
  }
  EPD_W21_WriteCMD_M1(0x06);         //booster soft start
  EPD_W21_WriteDATA_M1 (0x17);    //A
  EPD_W21_WriteDATA_M1 (0x17);    //B
  EPD_W21_WriteDATA_M1 (0x39);    //C
  EPD_W21_WriteDATA_M1 (0x17);
  EPD_W21_WriteCMD_M2(0x06);         //booster soft start
  EPD_W21_WriteDATA_M2 (0x17);    //A
  EPD_W21_WriteDATA_M2 (0x17);    //B
  EPD_W21_WriteDATA_M2 (0x39);    //C
  EPD_W21_WriteDATA_M2 (0x17);

  EPD_W21_WriteCMD_M1(0x61);      //resolution setting
  EPD_W21_WriteDATA_M1 (0x02);
  EPD_W21_WriteDATA_M1 (0x88);       //source 648
  EPD_W21_WriteDATA_M1 (0x01);    //gate 492
  EPD_W21_WriteDATA_M1 (0xEC);
  EPD_W21_WriteCMD_S1(0x61);      //resolution setting
  EPD_W21_WriteDATA_S1 (0x02);
  EPD_W21_WriteDATA_S1 (0x90);       //source 656
  EPD_W21_WriteDATA_S1 (0x01);    //gate 492
  EPD_W21_WriteDATA_S1 (0xEC);
  EPD_W21_WriteCMD_M2(0x61);      //resolution setting
  EPD_W21_WriteDATA_M2 (0x02);
  EPD_W21_WriteDATA_M2 (0x90);       //source 656
  EPD_W21_WriteDATA_M2 (0x01);    //gate 492
  EPD_W21_WriteDATA_M2 (0xEC);
  EPD_W21_WriteCMD_S2(0x61);      //resolution setting
  EPD_W21_WriteDATA_S2 (0x02);
  EPD_W21_WriteDATA_S2 (0x88);       //source 648
  EPD_W21_WriteDATA_S2 (0x01);    //gate 492
  EPD_W21_WriteDATA_S2 (0xEC);


  EPD_W21_WriteCMD_ALL(0x15);     //DUSPI
  EPD_W21_WriteDATA_ALL(0x20);    

  EPD_W21_WriteCMD_ALL(0x50);     //Vcom and data interval setting
  EPD_W21_WriteDATA_ALL(0x11);    //Border KW
  EPD_W21_WriteDATA_ALL(0x07);


  EPD_W21_WriteCMD_ALL(0x60);//TCON
  EPD_W21_WriteDATA_ALL(0x22);

  EPD_W21_WriteCMD_ALL(0xE3);
  EPD_W21_WriteDATA_ALL(0x00);
  
  
  //temperature
  EPD_W21_WriteCMD_M1(0x40);
  //lcd_chkstatus_M1();
   delay(300);
  driver_delay_xms(300);
  temp=EPD_W21_ReadDATA_M1_temp();
  
  EPD_W21_WriteCMD_ALL(0xe0);//Cascade setting
  EPD_W21_WriteDATA_ALL(0x03);
  EPD_W21_WriteCMD_ALL(0xe5);//Force temperature
  EPD_W21_WriteDATA_ALL(temp);

}
void EPD_refresh(void)
{
  EPD_W21_WriteCMD_M1M2(0x04);
  lcd_chkstatus_M1();
  delay(300);
  EPD_W21_WriteCMD_ALL(0x12);     //DISPLAY REFRESH
 // delay(30000);//delay  20s
  lcd_chkstatus_M1();
 /* lcd_chkstatus_S1();
  lcd_chkstatus_M2();
  lcd_chkstatus_S2();*/
}
  //////////Enter sleep//////////////////////////////
void EPD_sleep(void)
{
  EPD_W21_WriteCMD_ALL(0X02);   //power off
  delay(300);
  lcd_chkstatus_M1();
  delay(1000);
  EPD_W21_WriteCMD_ALL(0X07);   //deep sleep
  EPD_W21_WriteDATA_ALL(0xA5);
}

//picture//
void pic_display(void)
{
  unsigned int column,row;
    ////////M1 part//////////648*492
    EPD_W21_WriteCMD_M1(0x10);
    for(column=492;column<984;column++)
     for(row=0;row<81;row++)
    {
    EPD_W21_WriteDATA_M1(~gImage_BW1[row+column*163]);
    }
    EPD_W21_WriteCMD_M1(0x13);
    for(column=492;column<984;column++)
     for(row=0;row<81;row++)
    {
    EPD_W21_WriteDATA_M1(gImage_R1[row+column*163]);
    }
    
    ////////S1 part//////////656*492
    EPD_W21_WriteCMD_S1(0x10);
    for(column=492;column<984;column++)
     for(row=81;row<163;row++)
    {
    EPD_W21_WriteDATA_S1(~gImage_BW1[row+column*163]);
    }
    EPD_W21_WriteCMD_S1(0x13);
    for(column=492;column<984;column++)
     for(row=81;row<163;row++)
    {                                                                                                         
    EPD_W21_WriteDATA_S1(gImage_R1[row+column*163]);
    }
    
    ////////M2 part//////////656*492
    EPD_W21_WriteCMD_M2(0x10);
    for(column=0;column<492;column++)
     for(row=81;row<163;row++)
    {
    EPD_W21_WriteDATA_M2(~gImage_BW1[row+column*163]);
    }
    EPD_W21_WriteCMD_M2(0x13);
    for(column=0;column<492;column++)
     for(row=81;row<163;row++)
    {
    EPD_W21_WriteDATA_M2(gImage_R1[row+column*163]);
    }
    ////////S2 part//////////648*492
    EPD_W21_WriteCMD_S2(0x10);
    for(column=0;column<492;column++)
     for(row=0;row<81;row++)
    {
    EPD_W21_WriteDATA_S2(~gImage_BW1[row+column*163]);
    }
    EPD_W21_WriteCMD_S2(0x13);
    for(column=0;column<492;column++)
     for(row=0;row<81;row++)
    {
    EPD_W21_WriteDATA_S2(gImage_R1[row+column*163]);
    }

}
//picture white//
void pic_display_white(void)
{
  unsigned int column,row;
    ////////M1 part//////////648*492
    EPD_W21_WriteCMD_M1(0x10);
    for(column=492;column<984;column++)
     for(row=0;row<81;row++)
    {
    EPD_W21_WriteDATA_M1(0xff);
    }
    EPD_W21_WriteCMD_M1(0x13);
    for(column=492;column<984;column++)
     for(row=0;row<81;row++)
    {
    EPD_W21_WriteDATA_M1(0x00);
    }
    
    ////////S1 part//////////656*492
    EPD_W21_WriteCMD_S1(0x10);
    for(column=492;column<984;column++)
     for(row=81;row<163;row++)
    {
    EPD_W21_WriteDATA_S1(0xff);
    }
    EPD_W21_WriteCMD_S1(0x13);
    for(column=492;column<984;column++)
     for(row=81;row<163;row++)
    {                                                                                                         
    EPD_W21_WriteDATA_S1(0x00);
    }
    
    ////////M2 part//////////656*492
    EPD_W21_WriteCMD_M2(0x10);
    for(column=0;column<492;column++)
     for(row=81;row<163;row++)
    {
    EPD_W21_WriteDATA_M2(0xff);
    }
    EPD_W21_WriteCMD_M2(0x13);
    for(column=0;column<492;column++)
     for(row=81;row<163;row++)
    {
    EPD_W21_WriteDATA_M2(0x00);
    }
    ////////S2 part//////////648*492
    EPD_W21_WriteCMD_S2(0x10);
    for(column=0;column<492;column++)
     for(row=0;row<81;row++)
    {
    EPD_W21_WriteDATA_S2(0xff);
    }
    EPD_W21_WriteCMD_S2(0x13);
    for(column=0;column<492;column++)
     for(row=0;row<81;row++)
    {
    EPD_W21_WriteDATA_S2(0x00);
    }

}

void lcd_chkstatus_M1(void)
{
  while(!EPD_W21_ReadDATA_M1);

}
/*
void lcd_chkstatus_M2(void)
{
  unsigned char busy;
  do
    {
    EPD_W21_WriteCMD_M2(0x71);
    busy = EPD_W21_ReadDATA_M2;
    busy =!(busy & 0x01);
    }
  while(busy);
  driver_delay_xms(200);
}
void lcd_chkstatus_S1(void)
{
  unsigned char busy;
  do
    {
    EPD_W21_WriteCMD_S1(0x71);
    busy = EPD_W21_ReadDATA_S1;
    busy =!(busy & 0x01);
    }
  while(busy);
  driver_delay_xms(200);
}

void lcd_chkstatus_S2(void)
{
  unsigned char busy;
  do
    {
    EPD_W21_WriteCMD_S2(0x71);
    busy = EPD_W21_ReadDATA_S2;
    busy =!(busy & 0x01);
    }
  while(busy);
  driver_delay_xms(200);
}*/
