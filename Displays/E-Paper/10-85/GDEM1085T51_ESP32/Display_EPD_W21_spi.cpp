#include "Display_EPD_W21_spi.h"
#include <SPI.h>

//SPI write byte
void SPI_Write(unsigned char value)
{				   			 
   SPI.transfer(value);
}

//SPI write command
void EPD_W21_WriteCMD(unsigned char command)
{
  EPD_W21_CS_1;
  EPD_W21_CS2_1;
  EPD_W21_CS_0;  
  EPD_W21_CS2_0;        
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
  EPD_W21_CS2_1;
}
//SPI write data
void EPD_W21_WriteDATA(unsigned char datas)
{
  EPD_W21_CS_1;
  EPD_W21_CS2_1;
  EPD_W21_CS_0;  
  EPD_W21_CS2_0;                  
  EPD_W21_DC_1;   // data write
  SPI_Write(datas);
  EPD_W21_CS_1;
  EPD_W21_CS2_1;
}

void EPD_W21_WriteCMD1(unsigned char command)
{
  EPD_W21_CS2_1;
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void EPD_W21_WriteDATA1(unsigned char datas)
{
  EPD_W21_CS2_1;
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // data write
  SPI_Write(datas);
  EPD_W21_CS_1;
}

void EPD_W21_WriteCMD2(unsigned char command)
{
  EPD_W21_CS_1;
  EPD_W21_CS2_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS2_1;
}
void EPD_W21_WriteDATA2(unsigned char datas)
{
  EPD_W21_CS_1;
  EPD_W21_CS2_0;                   
  EPD_W21_DC_1;   // data write
  SPI_Write(datas);
  EPD_W21_CS2_1;
}
