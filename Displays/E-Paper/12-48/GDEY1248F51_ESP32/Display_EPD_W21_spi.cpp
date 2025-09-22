#include "Display_EPD_W21_spi.h"

void GPIO_1248_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);
	//SCL--PE0  SDA--PE1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	//PB9--CSB_S2   PB8--CSB--M1 	PB5--RST_M2
	//PB7--BUSY_M2  PB6--BUSY_M1

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 |GPIO_Pin_9;    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//PD7--RST_M1   PD6--DC_M2  PD5--DC_M1  PD4--CSB-M2  PD3--CSB-S1    
	//PD2--BUSY_S2  PD1--BUSY_S1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3| GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7;    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
						
}

void GPIO_IO(unsigned char i)//GPIO mode flip  0:INPUT,1:OUTPUT
{
	 GPIO_InitTypeDef GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	if(i==0)
	{		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;      //SDA IN
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	}
	else
	{
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1 ;    //SDA OUT
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	}
}
void SPI_Delay(unsigned char xrate)
{
	unsigned char i;
	while(xrate)
	{
		for(i=0;i<EPD_W21_SPI_SPEED;i++);
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
	EPD_W21_DC_M1_0;		// command write
	SPI_Write(command);
	EPD_W21_CS_M1_1;
}


void EPD_W21_WriteDATA_M1(unsigned char command)
{
	EPD_W21_CS_M1_0;
	EPD_W21_DC_M1_1;		// command write
	SPI_Write(command);
	EPD_W21_CS_M1_1;
}


void EPD_W21_WriteCMD_S1(unsigned char command)
{
	EPD_W21_CS_S1_0;
	EPD_W21_DC_M1_0;		// command write
	SPI_Write(command);
	EPD_W21_CS_S1_1;

}


void EPD_W21_WriteDATA_S1(unsigned char command)
{
	EPD_W21_CS_S1_0;
	EPD_W21_DC_M1_1;		// command write
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
	EPD_W21_DC_M1_0;		// command write
	EPD_W21_DC_M2_0;		// command write
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
	EPD_W21_DC_M1_1;		// command write
	EPD_W21_DC_M2_1;		// command write
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
	EPD_W21_DC_M1_0;		// command write
	EPD_W21_DC_M2_0;		// command write
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
	EPD_W21_DC_M1_1;		// command write
	EPD_W21_DC_M2_1;		// command write
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
	EPD_W21_DC_M2_0;		// command write
	SPI_Delay(1);
	SPI_Write(command);
	SPI_Delay(1);
	EPD_W21_CS_M2_1;
}


void EPD_W21_WriteDATA_M2(unsigned char command)
{

	EPD_W21_CS_M2_0;
	EPD_W21_DC_M2_1;		// command write
	SPI_Write(command);
	EPD_W21_CS_M2_1;
}




void EPD_W21_WriteCMD_S2(unsigned char command)
{
	EPD_W21_CS_S2_0;
	EPD_W21_DC_M2_0;		// command write
	SPI_Write(command);
	EPD_W21_CS_S2_1;
}


void EPD_W21_WriteDATA_S2(unsigned char command)
{
	EPD_W21_CS_S2_0;
	EPD_W21_DC_M2_1;		// command write
	SPI_Write(command);
	EPD_W21_CS_S2_1;
}


//Read data for EPD
unsigned char EPD_W21_ReadDATA_M1_temp(void)
{
	unsigned char i,j;
	GPIO_IO(0); //GPIO mode flip  0:INPUT,1:OUTPUT
	EPD_W21_CS_M1_0;
	//EPD_W21_CS_S1_1;
	//EPD_W21_CS_M2_1;
	//EPD_W21_CS_S2_1;
	EPD_W21_DC_M1_1;		// command write
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

/***********************************************************
						end file
***********************************************************/
