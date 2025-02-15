#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_


#define EPD_WIDTH   200
#define EPD_HEIGHT  200
#define EPD_ARRAY  EPD_WIDTH*EPD_HEIGHT/8  
//Full screen refresh display
void EPD_HW_Init(void); 
void EPD_HW_Init_180(void);  
void EPD_WhiteScreen_ALL(const unsigned char* datasBW,const unsigned char* datasRW);
void EPD_WhiteScreen_White(void);
void EPD_WhiteScreen_Black(void);
void EPD_DeepSleep(void);

  
#endif
