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
  ///////////////////////////Normal picture display/////////////////////////////////////////////////////////////////
    /************Normal picture display*******************/
    EPD_init(); //EPD init
    PIC_display(gImage_BW1,gImage_R1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(5000); //5s   
    //Clear
    EPD_init(); //EPD init 
    PIC_display_Clear();//EPD Clear
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!! 
    while(1);
  }
}




///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
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
void EPD_W21_WriteDATA(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}



/////////////////EPD settings Functions/////////////////////
void EPD_W21_Init(void)
{
  EPD_W21_RST_0;    // Module reset
  delay(10); //At least 10ms
  EPD_W21_RST_1;
  delay(10);  
}
void EPD_init(void)
{
    EPD_W21_Init();   //Electronic paper IC reset 

    EPD_W21_WriteCMD(0x04); //POWER ON 
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal
  
    EPD_W21_WriteCMD(0x00);
    EPD_W21_WriteDATA(0x0F); 
  
    EPD_W21_WriteCMD(0x50); 
    EPD_W21_WriteDATA(0x20);  
    EPD_W21_WriteDATA(0x07); 
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


void PIC_display(const unsigned char* picData_old,const unsigned char* picData_new)
{
    unsigned int i;
    //Write Data
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<48000;i++)    
    {
     EPD_W21_WriteDATA(picData_old[i]);  
    }
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<48000;i++)       
    {
     EPD_W21_WriteDATA(picData_new[i]); 

    } 
   //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(1);   //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal


}

void PIC_display_Clear(void)
{
    unsigned int i;
    //Write Data
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<48000;i++)    
    { 
    EPD_W21_WriteDATA(0x00); 
    }
    EPD_W21_WriteCMD(0x13);    //Transfer new data
    for(i=0;i<48000;i++)       
    {
    EPD_W21_WriteDATA(0x00);  //Transfer the actual displayed data
    } 
    //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(1);              //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
      
}
void lcd_chkstatus(void)
{
  while(!isEPD_W21_BUSY);    //0 BUSY£¬1 FREE¡£
                    
}
