

#ifndef __GDEP133C02_H__
#define __GDEP133C02_H__

#define BLACK         0x00
#define WHITE         0x11
#define YELLOW        0x22
#define RED           0x33
#define BLUE          0x55
#define GREEN         0x66


#define PSR             0x00
#define PWR             0x01
#define POF             0x02
#define PON             0x04
#define BTST_N          0x05
#define BTST_P          0x06
#define DTM             0x10
#define DRF             0x12
#define CDI             0x50
#define TCON            0x60
#define TRES            0x61
#define PTLW            0x83
#define AN_TM           0x74
#define AGID            0x86
#define BUCK_BOOST_VDDN 0xB0
#define TFT_VCOM_POWER  0xB1
#define EN_BUF          0xB6
#define BOOST_VDDP_EN   0xB7
#define CCSET           0xE0
#define PWS             0xE3
#define CMD66           0xF0

#define FIRST_DATA_PACKET	1
#define NOT_FIRST_DATA_PACKET	0

#define PTLW_ENABLE  0x01
#define PTLW_DISABLE 0x00

// Image buffer for sending image
#define EPD_IMAGE_DATA_BUFFER 8192 // MCU RAM Size (800*720/2) reserve for one driver IC

#endif //#ifndef __GDEP133C02_H__

#ifdef __GDEP133C02_C__
#define __GDEP133C02_EXTERN__
#else
#define __GDEP133C02_EXTERN__ extern
#endif

__GDEP133C02_EXTERN__ unsigned char epdImageDataBuffer[EPD_IMAGE_DATA_BUFFER];

__GDEP133C02_EXTERN__ const unsigned char AN_TM_V[9];
__GDEP133C02_EXTERN__ const unsigned char CMD66_V[6];
__GDEP133C02_EXTERN__ const unsigned char PSR_V[2];
__GDEP133C02_EXTERN__ const unsigned char CDI_V[1];
__GDEP133C02_EXTERN__ const unsigned char TCON_V[2];
__GDEP133C02_EXTERN__ const unsigned char AGID_V[1];
__GDEP133C02_EXTERN__ const unsigned char PWS_V[1];
__GDEP133C02_EXTERN__ const unsigned char CCSET_V[1];
__GDEP133C02_EXTERN__ const unsigned char TRES_V[4];
__GDEP133C02_EXTERN__ const unsigned char PWM_V[6];
__GDEP133C02_EXTERN__ const unsigned char EN_BUF_V[1];
__GDEP133C02_EXTERN__ const unsigned char BTST_P_V[2];
__GDEP133C02_EXTERN__ const unsigned char BOOST_VDDP_EN_V[1];
__GDEP133C02_EXTERN__ const unsigned char BTST_N_V[2];
__GDEP133C02_EXTERN__ const unsigned char BUCK_BOOST_VDDN_V[1];
__GDEP133C02_EXTERN__ const unsigned char TFT_VCOM_POWER_V[1];
__GDEP133C02_EXTERN__ const unsigned char POF_V[1];
__GDEP133C02_EXTERN__ const unsigned char DRF_V[1];



__GDEP133C02_EXTERN__ void epdHardwareReset(void);
__GDEP133C02_EXTERN__ void setPinCsAll(unsigned int setLevel);
__GDEP133C02_EXTERN__ void setPinCs(unsigned char csNumber, unsigned int setLevel);
__GDEP133C02_EXTERN__ void checkBusyHigh(void);
__GDEP133C02_EXTERN__ void checkBusyLow(void);
__GDEP133C02_EXTERN__ void initEPD(void);
__GDEP133C02_EXTERN__ void writeEpd(unsigned char epdCommand, unsigned char *epdData, unsigned int epdDataLength);
__GDEP133C02_EXTERN__ void readEpd(unsigned char epdCommand, unsigned char *epdData, unsigned int epdDataLength);
__GDEP133C02_EXTERN__ void writeEpdCommand(unsigned char epdCommand);
__GDEP133C02_EXTERN__ void writeEpdData(unsigned char *epdData, unsigned int epdDataLength);
__GDEP133C02_EXTERN__ void epdDisplay(void);
__GDEP133C02_EXTERN__ void epdDisplayColor(unsigned char colorSelect);
__GDEP133C02_EXTERN__ void writeEpdImage(unsigned char csx, unsigned char const *imageData, unsigned long imageDataLength);
__GDEP133C02_EXTERN__ void epdDisplayColorBar(void);
__GDEP133C02_EXTERN__ unsigned char checkDriverICStatus(void);
__GDEP133C02_EXTERN__ char partialWindowUpdateWithImageData(unsigned char csx, unsigned char const *imageData, unsigned long dataSize, unsigned int xStart, unsigned int yStart, unsigned int xPixel, unsigned int yLine, unsigned char displayEnable);
__GDEP133C02_EXTERN__ char partialWindowUpdateWithoutImageData(unsigned char csx, unsigned int xStart, unsigned int yStart, unsigned int xPixel, unsigned int yLine, unsigned char epdDisplayEnable);


__GDEP133C02_EXTERN__ void pic_display_test(const unsigned char *num);
__GDEP133C02_EXTERN__ void draw_checkerboard();