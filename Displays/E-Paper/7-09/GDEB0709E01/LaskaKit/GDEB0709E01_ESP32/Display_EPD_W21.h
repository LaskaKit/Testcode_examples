#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

// 4 bit per pixel, 2 pixels per byte
#define black   0x0
#define white   0x1
#define yellow  0x2
#define red     0x3
#define blue    0x5
#define green   0x6

#define EPD_WIDTH        1200   // pixels (portrait)
#define EPD_HEIGHT       1600
#define EPD_ROW_BYTES    (EPD_WIDTH / 2)      // 600 bytes per row
#define EPD_HALF_BYTES   (EPD_ROW_BYTES / 2)  // 300 bytes per row and driver IC
#define ALLSCREEN_BYTES  (EPD_ROW_BYTES * EPD_HEIGHT)   // 960000

void EPD_init(void);
void EPD_refresh(void);
void EPD_sleep(void);
void lcd_chkstatus(void);
void EPD_check_driver_ICs(void);

void PIC_display(const unsigned char *picData);
void Display_All(unsigned char color);
void Display_All_White(void);
void Display_Color_Bar(void);
void Display_Checkerboard(void);
void Display_Vertical_Bars(void);

#endif
