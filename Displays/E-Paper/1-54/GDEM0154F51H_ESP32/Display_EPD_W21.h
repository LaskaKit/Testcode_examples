#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

//2bit
#define black   0x00  /// 00
#define white   0x01  /// 01
#define yellow  0x02  /// 10
#define red     0x03  /// 11


#define Source_BITS     200
#define Gate_BITS   200
#define ALLSCREEN_BYTES   Source_BITS*Gate_BITS/4


//EPD
void EPD_init(void);
void PIC_display (const unsigned char* picData);
void EPD_sleep(void);
void EPD_update(void);
void lcd_chkstatus(void);

void Display_All_Black(void);
void Display_All_White(void);
void Display_All_Yellow(void);
void Display_All_Red(void);

void Acep_color(unsigned char color);
void EPD_init_Fast(void);  

#endif
/***********************************************************
            end file
***********************************************************/
