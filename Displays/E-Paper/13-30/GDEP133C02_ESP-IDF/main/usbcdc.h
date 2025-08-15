/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "esp_log.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Decode the jpeg ``image.jpg`` embedded into the program file into pixel data.
 *
 * @param pixels A pointer to a pointer for an array of rows, which themselves are an array of pixels.
 *        Effectively, you can get the pixel data by doing ``decode_image(&myPixels); pixelval=myPixels[ypos][xpos];``
 * @return - ESP_ERR_NOT_SUPPORTED if image is malformed or a progressive jpeg file
 *         - ESP_ERR_NO_MEM if out of memory
 *         - ESP_OK on succesful decode
 */


#define _msgActionSucces                 0x00
#define _msgActionSuccesAndFeedbackData  0x01
#define _msgUnknownError                 0xE0
#define _msgCheckSumError                0xE1
#define _msgNoVCOMError                  0xE2

#define _msgOTPOverflowError             0xE3
#define _msgDriverIDError                0xE4
#define _msgEPDCheckError                0xE5

#define _msgSDMountError                 0xE6
#define _msgSDOperationError             0xE7
#define _msgFlashOperationError          0xE8

/*
const unsigned char Error[]="Error! \n\r"; //_msgUnknownError
const unsigned char Error_OTP_Overflow[]="Error! The OTP address overflows. \n\r"; //_msgOTPHasWrittenError
const unsigned char Error_Driver_ID[]="Error! Checking driver IC is errors! Please check the flat cables and the bridge FPC. \n\r"; //
const unsigned char CHECK_Error[]="Error! After checking EPD module, there is an error in EPD module. \n\r";
const unsigned char sramError[]="Error! Read SRAM \n\r";
const unsigned char otpError[]="Error! Read OTP\n\r";
const unsigned char crcError[]="Error! After burning OTP memory, the CRC comparison result is NG! \n\r";

const unsigned char twoBurnError[]="Error! The waveform data cannot support two burn. \n\r";
const unsigned char checkVcomError[]="Error! The VCOMDC comparison result is NG! \n\r";
const unsigned char otpSizeError[]="Error! The otpSizeEmpty is not enough. \r\n";
*/




//====== App command list ======
#define _writeEPD 0x00
#define _getFirmware 0x01
#define _setVCOMDC 0x80
#define _getVCOMLocation 0x81
#define _setVCOMDCValue 0x82
#define _getVCOMDCValue 0x83
#define _readOTP 0xE3
#define _writeOTP 0xE4
#define _writeUserData 0xE5

#define _controlSW2 0xF4
#define _controlSW3 0xF5
#define _controlSW4 0xF6
#define _controlSW5 0xF7

#define _eraseFlashSection 0xFB
#define _readFlash 0xFC
#define _writeFlash 0xFD
#define _readHulkData 0xFF

#define ChipID_Read  0x10

void usbcdc_init(void);
void HextoString(unsigned char* HexBuf, unsigned char* StringBuf, unsigned char number);
uint8_t string_to_number(unsigned char Buf);

extern tinyusb_config_cdcacm_t acm_cfg;

/*--parameter declaration area--*/
#define databuf_size            8192 + 8 // 8 byte (Communication Header)
uint8_t databuf[databuf_size];
uint8_t *EPD_Data_ReadBck;
uint8_t USB_STATUS;
uint8_t FOPT_status;
uint8_t EPD_Command;
uint8_t *Temp_Buf;
uint32_t preSend;
uint32_t data_cmd_cnt;
uint8_t USB_Rx_flag;
const char *TAG;
size_t rx_size;
int itf;

#ifdef __cplusplus
}
#endif
