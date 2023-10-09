/* Display test for Good Display GDEY029T94
 * example from Good Display is used
 * 
 * Board:           LaskaKit ESPink ESP32 e-Paper   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Display:         Good Display GDEY029T94         https://www.laskakit.cz/good-display-gdey029t94-2-9--296x128-epaper-displej-grayscale/
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
int SCK_Pin = 18; 
int SDI_Pin = 23; 
#define EPD_W21_MOSI_0  digitalWrite(SDI_Pin,LOW)
#define EPD_W21_MOSI_1  digitalWrite(SDI_Pin,HIGH) 

#define EPD_W21_CLK_0 digitalWrite(SCK_Pin,LOW)
#define EPD_W21_CLK_1 digitalWrite(SCK_Pin,HIGH)

#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)

#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
//296*128///////////////////////////////////////

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3

#define MAX_LINE_BYTES 16// =128/8
#define MAX_COLUMN_BYTES 296

#define ALLSCREEN_GRAGHBYTES 4736

////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay(unsigned long xms);
void DELAY_S(unsigned int delaytime);     
void SPI_Delay(unsigned char xrate);


//EPD
void Epaper_READBUSY(void);
void SPI_Write(unsigned char value);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char datas);

void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Part_Init(void); //Local refresh initialization

void EPD_Part_Update(void); 
void EPD_Update(void);

void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 
void EPD_WhiteScreen_ALL(const unsigned char * datas);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_Part_myself(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        );
void EPD_HW_Init_4GRAY(void);    
void EPD_Update_4GRAY(void);                         
void EPD_WhiteScreen_ALL_4GRAY(const unsigned char *datas);

void setup() {
   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);    
   pinMode(SCK_Pin, OUTPUT);    
   pinMode(SDI_Pin, OUTPUT);    
}
////////Partial refresh schematic////////////////

/////Y/// (0,0)        /---/(x,y)
          //                 /---/
          //                /---/  
          //x
          //
          //
//Tips//
/*
1. When refreshing the electronic paper in full screen, the picture flickers is a normal phenomenon, the main function is to clear the residual image displayed in the previous picture.
2. When performing partial refresh, the screen will not flicker. It is recommended to use full-screen refresh to clear the screen after 5 partial refreshes to reduce screen image retention.
3. After the e-paper is refreshed, it needs to enter the sleep mode, please do not delete the sleep command.
4. Do not remove the e-paper when turning on.
5. Wake up from sleep, need to re-initialize the electronic paper.
6. When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is in input mode, and the other pins are in output mode.
*/
void loop() {
  unsigned char fen_L,fen_H,miao_L,miao_H;
  ///////////////////////////Normal picture display/////////////////////////////////////////////////////////////////
    /************Normal picture display*******************/
    EPD_HW_Init(); //EPD init
    EPD_WhiteScreen_ALL(gImage_1);//EPD_picture1
    EPD_DeepSleep();//EPD_DeepSleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  

    EPD_HW_Init_4GRAY(); //EPD init 4gray
    EPD_WhiteScreen_ALL_4GRAY(gImage_11); 
    EPD_DeepSleep();//EPD_DeepSleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  
    
    //Clear
    EPD_HW_Init(); //EPD init
    EPD_WhiteScreen_White();//EPD Clear
    EPD_DeepSleep();//EPD_DeepSleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  
    
/*It is recommended to use full-screen refresh to clear the screen after 5 partial refreshes to reduce screen image retention.    */
//////////////////////Partial refresh time demo/////////////////////////////////////
    EPD_HW_Init(); //Electronic paper initialization  
    EPD_SetRAMValue_BaseMap(gImage_basemap); //Partial refresh background color     
    for(fen_H=0;fen_H<6;fen_H++)
    {
    for(fen_L=0;fen_L<10;fen_L++)
    {
    for(miao_H=0;miao_H<6;miao_H++)   
    {
    for(miao_L=0;miao_L<10;miao_L++)
    {
        EPD_Dis_Part_myself(32,92,(unsigned char *)&Num[miao_L],         //x-A,y-A,DATA-A
                            32,124,(unsigned char *)&Num[miao_H],         //x-B,y-B,DATA-B
                            32,164,(unsigned char *)gImage_numdot,       //x-C,y-C,DATA-C
                            32,206,(unsigned char *)&Num[fen_L],         //x-D,y-D,DATA-D
                            32,238,(unsigned char *)&Num[fen_H],32,64);  //x-E,y-E,DATA-E,Resolution 32*64
                            
                            if((fen_L==0)&&(miao_H==0)&&(miao_L==5))
                            goto Clear;
      }
    }
    }
    
    }
    
////////////////////////////////////////////////////////////////////////  
      //Clear screen
    Clear:
    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_White(); //Show all white
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!! 
  while(1); 
  
} 













///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
/////////////////////delay//////////////////////////////////////
void driver_delay_us(unsigned int xus)  //1us
{
  for(;xus>1;xus--);
}
void driver_delay(unsigned long xms) //1ms
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
   SPI_Delay(1);
    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
       SPI_Delay(1);
       if(value & 0x80)
          EPD_W21_MOSI_1;
        else
          EPD_W21_MOSI_0;   
        value = (value << 1); 
       SPI_Delay(1);
       driver_delay_us(1);
        EPD_W21_CLK_1; 
        SPI_Delay(1);
    }
}

void Epaper_Write_Command(unsigned char cmd)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(cmd);
  EPD_W21_CS_1;
}
void Epaper_Write_Data(unsigned char datas)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   //data write
  SPI_Write(datas);
  EPD_W21_CS_1;
}

/////////////////EPD settings Functions/////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void EPD_HW_Init(void)
{
  EPD_W21_RST_0;  // Module reset      
  delay(10); //At least 10ms delay 
  EPD_W21_RST_1; 
  delay(10); //At least 10ms delay 
  
  Epaper_READBUSY();   
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
    
  Epaper_Write_Command(0x01); //Driver output control      
  Epaper_Write_Data(0x27);
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x11); //data entry mode       
  Epaper_Write_Data(0x01);

  Epaper_Write_Command(0x44); //set Ram-X address start/end position   
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x0F);    //0x0F-->(15+1)*8=128

  Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
  Epaper_Write_Data(0x27);   //0x0127-->(295+1)=296
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00); 

  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x05);  
      
  Epaper_Write_Command(0x21); //  Display update control
  Epaper_Write_Data(0x00);    
  Epaper_Write_Data(0x80);          
      
  Epaper_Write_Command(0x18); //Read built-in temperature sensor
  Epaper_Write_Data(0x80);  

  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;    
  Epaper_Write_Data(0x27);
  Epaper_Write_Data(0x01);
  Epaper_READBUSY();
  
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char * datas)
{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   EPD_Update();   
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{   
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xF7);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();  

}
void EPD_Part_Update(void)
{
  Epaper_Write_Command(0x22);//Display Update Control 
  Epaper_Write_Data(0xFF);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();      
}
void EPD_DeepSleep(void)
{  
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  delay(100);
}
void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  }  
}
///////////////////////////Part update//////////////////////////////////////////////
void EPD_SetRAMValue_BaseMap( const unsigned char * datas)
{
  unsigned int i;   
  const unsigned char  *datas_flag;   
  datas_flag=datas;
  Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   datas=datas_flag;
  Epaper_Write_Command(0x26);   //Write Black and White image to RAM
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   EPD_Update();   
}


void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  x_start=x_start/8;//
  x_end=x_start+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_start;
  if(y_start>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_start+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
//Reset
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
    
  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x80);  
//  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_start);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    // ????=0 


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_start); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   } 
   EPD_Part_Update();

}




/////////////////////////////////Single display////////////////////////////////////////////////


void EPD_WhiteScreen_White(void)

{
   unsigned int i,k;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
  for(k=0;k<296;k++)
  {
    for(i=0;i<16;i++)
    {
      Epaper_Write_Data(0xff);
      }
  }
  EPD_Update();
}

/////////////////////////////////////TIME///////////////////////////////////////////////////
void EPD_Dis_Part_myself(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        )
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  
  //Data A////////////////////////////
  x_startA=x_startA/8;//Convert to byte
  x_end=x_startA+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startA-1;
  if(y_startA>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startA+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   

//Reset
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
    
  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x80);  
//
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startA);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startA); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(pgm_read_byte(&datasA[i]));
   } 
  //Data B/////////////////////////////////////
  x_startB=x_startB/8;//Convert to byte
  x_end=x_startB+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startB-1;
  if(y_startB>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startB+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startB);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startB); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(pgm_read_byte(&datasB[i]));
   }   
   
  //Data C//////////////////////////////////////
  x_startC=x_startC/8;//Convert to byte
  x_end=x_startC+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startC-1;
  if(y_startC>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startC+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startC);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startC); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(pgm_read_byte(&datasC[i]));
   }     
   
  //Data D//////////////////////////////////////
  x_startD=x_startD/8;//Convert to byte
  x_end=x_startD+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startD-1;
  if(y_startD>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startD+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startD);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);        // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startD); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(pgm_read_byte(&datasD[i]));
   } 
  //Data E//////////////////////////////////////
  x_startE=x_startE/8;//Convert to byte
  x_end=x_startE+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startE-1;
  if(y_startE>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startE+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startE);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startE); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(pgm_read_byte(&datasE[i]));
   }    
   EPD_Part_Update();

}


//4GRAY/////////////////////////////////////////////////////
const unsigned char LUT_DATA_4Gray[159] =    //159bytes

{                      
0x40, 0x48, 0x80, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
0x8,  0x48, 0x10, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
0x2,  0x48, 0x4,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
0x20, 0x48, 0x1,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
0xA,  0x19, 0x0,  0x3,  0x8,  0x0,  0x0,          
0x14, 0x1,  0x0,  0x14, 0x1,  0x0,  0x3,          
0xA,  0x3,  0x0,  0x8,  0x19, 0x0,  0x0,          
0x1,  0x0,  0x0,  0x0,  0x0,  0x0,  0x1,          
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,          
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,          
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,          
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,          
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,          
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,          
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,          
0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,          
0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0,  0x0,  0x0,      
0x22, 0x17, 0x41, 0x0,  0x32, 0x1C};    


void EPD_select_LUT(const unsigned char * wave_data)
{        
     unsigned char count;
     Epaper_Write_Command(0x32);
   for(count=0;count<153;count++)Epaper_Write_Data(wave_data[count]);

} 
void EPD_HW_Init_4GRAY(void)
{
  EPD_W21_RST_0;     
  delay(10); 
  EPD_W21_RST_1; //hard reset  
  delay(10);  

  Epaper_READBUSY();
  Epaper_Write_Command(0x12); // soft reset
  Epaper_READBUSY();

  Epaper_Write_Command(0x74); //set analog block control       
  Epaper_Write_Data(0x54);
  Epaper_Write_Command(0x7E); //set digital block control          
  Epaper_Write_Data(0x3B);

  Epaper_Write_Command(0x01); //Driver output control      
  Epaper_Write_Data(0x27);
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x11); //data entry mode       
  Epaper_Write_Data(0x01);

  Epaper_Write_Command(0x44); //set Ram-X address start/end position   
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x0F);    //0x0F-->(15+1)*8=128

  Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
  Epaper_Write_Data(0x27);   //0x0127-->(295+1)=296
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00); 

  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x00);  


  Epaper_Write_Command(0x2C);     //VCOM Voltage
  Epaper_Write_Data(LUT_DATA_4Gray[158]);    //0x1C


  Epaper_Write_Command(0x3F); //EOPQ    
  Epaper_Write_Data(LUT_DATA_4Gray[153]);
  
  Epaper_Write_Command(0x03); //VGH      
  Epaper_Write_Data(LUT_DATA_4Gray[154]);

  Epaper_Write_Command(0x04); //      
  Epaper_Write_Data(LUT_DATA_4Gray[155]); //VSH1   
  Epaper_Write_Data(LUT_DATA_4Gray[156]); //VSH2   
  Epaper_Write_Data(LUT_DATA_4Gray[157]); //VSL   
   
  EPD_select_LUT(LUT_DATA_4Gray); //LUT

  Epaper_Write_Command(0x21); //  Display update control
  Epaper_Write_Data(0x00);    
  Epaper_Write_Data(0x80);  
  
  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;    
  Epaper_Write_Data(0x27);
  Epaper_Write_Data(0x01);
  Epaper_READBUSY();
}
unsigned char In2bytes_Out1byte_RAM1(unsigned char data1,unsigned char data2)
{
  unsigned int i; 
  unsigned char TempData1,TempData2;
  unsigned char outdata=0x00;
  TempData1=data1;
  TempData2=data2;
  
    for(i=0;i<4;i++)
     { 
        outdata=outdata<<1;
        if( ((TempData1&0xC0)==0xC0) || ((TempData1&0xC0)==0x40))
           outdata=outdata|0x01;
        else 
          outdata=outdata|0x00;

        TempData1=TempData1<<2;

        //delay_us(5) ; 
     }

    for(i=0;i<4;i++)
     { 
        outdata=outdata<<1;
         if((TempData2&0xC0)==0xC0||(TempData2&0xC0)==0x40)
           outdata=outdata|0x01;
        else 
          outdata=outdata|0x00;

        TempData2=TempData2<<2;

       // delay_us(5) ; 
     }
     return outdata;
}
unsigned char In2bytes_Out1byte_RAM2(unsigned char data1,unsigned char data2)
{
  unsigned int i; 
  unsigned char TempData1,TempData2;
  unsigned char outdata=0x00;
TempData1=data1;
TempData2=data2;
  
    for(i=0;i<4;i++)
     { 
        outdata=outdata<<1;
        if( ((TempData1&0xC0)==0xC0) || ((TempData1&0xC0)==0x80))
           outdata=outdata|0x01;
        else 
          outdata=outdata|0x00;

        TempData1=TempData1<<2;

       // delay_us(5) ; 
     }

    for(i=0;i<4;i++)
     { 
        outdata=outdata<<1;
         if((TempData2&0xC0)==0xC0||(TempData2&0xC0)==0x80)
           outdata=outdata|0x01;
        else 
          outdata=outdata|0x00;

        TempData2=TempData2<<2;

        //delay_us(5) ; 
     }
     return outdata;
}
//296*128
void EPD_WhiteScreen_ALL_4GRAY(const unsigned char *datas)
{
   unsigned int i;
   unsigned char tempOriginal;   
  
  
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<4736*2;i+=2)
   {               
    tempOriginal= In2bytes_Out1byte_RAM1( *(datas+i),*(datas+i+1));
     Epaper_Write_Data(~tempOriginal); 
   }
   
   Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<4736*2;i+=2)
   {               
    tempOriginal= In2bytes_Out1byte_RAM2( *(datas+i),*(datas+i+1));
     Epaper_Write_Data(~tempOriginal); 
   }
   EPD_Update_4GRAY();   
}
void EPD_Update_4GRAY(void)
{   
  Epaper_Write_Command(0x22); 
  Epaper_Write_Data(0xC7);   
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();  

}




//////////////////////////////////END//////////////////////////////////////////////////
