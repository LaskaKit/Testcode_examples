#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "delay.h"

void EPD_W21_Init(void)
{

	EPD_W21_CS_M1_1;   //There are only two CS
	EPD_W21_CS_S1_1;
	//EPD_W21_CS_M2_1;   //These two must be cancelled, otherwise there will be incomplete display
  //EPD_W21_CS_S2_1;
	delay_ms(10);
	EPD_W21_RST_M1_1;
	EPD_W21_RST_M2_1;
	delay_ms(10);
	EPD_W21_RST_M1_0;  //Two resets are internally connected together
	EPD_W21_RST_M2_0;
  delay_ms(100);
	EPD_W21_RST_M1_1;
	EPD_W21_RST_M2_1;
  delay_ms(100);
	
}

/***********************************************************
						end file
***********************************************************/

