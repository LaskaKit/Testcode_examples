#ifndef __IT8957_H__
#define __IT8957_H__

#include "IT8957_IO.h"
#define CS_L 0
#define CS_H 1
#define I80_DBG_INFO Serial.printf

// //typedef for variables
// typedef uint8_t TByte; //1 byte
// typedef uint16_t TWord; //2 bytes
// typedef uint32_t TDWord; //4 bytes


//***************************************************************
//Definition Base Address and Offset 
//for IT8957 Display Controller Registers 
//2018.09.26 updated
//***************************************************************
//Registers of Display Controller
#define DISPLAY_REG_BASE         0x1000     //For HSPI low 16-bits only


//Base Address of Basic LUT Registers
#define LUT0EWHR    (DISPLAY_REG_BASE + 0x00)    //LUT0 Engine Width Height Reg
#define LUT0XYR     (DISPLAY_REG_BASE + 0x40)    //LUT0 XY Reg 
#define LUT0BADDR   (DISPLAY_REG_BASE + 0x80)    //LUT0 Base Address Reg

#define UP0SR       (DISPLAY_REG_BASE + 0x134)   //Update Parameter0 Setting Reg
#define UP1SR       (DISPLAY_REG_BASE + 0x138)   //Update Parameter1 Setting Reg
#define UPBBADDR    (DISPLAY_REG_BASE + 0x17C)   //Update Buffer Base Address

#define INF_DARCR0      (DISPLAY_REG_BASE + 0x3C0)

//Display Engine Status Registers
#define INF_PAESR0      (DISPLAY_REG_BASE + 0x3D8)
#define INF_PAESR1      (DISPLAY_REG_BASE + 0x3DC)
#define INF_PAESR2      (DISPLAY_REG_BASE + 0x3E0)
#define INF_PAESR3      (DISPLAY_REG_BASE + 0x3E4)

//-------System Registers----------------
#define SYS_REG_BASE  0x0000

//Base address of System Registers
//#define SCTR        (SYS_REG_BASE + 0x00) 
#define I80CPCR     (SYS_REG_BASE + 0x04)
//#define PMSR        (SYS_REG_BASE + 0x24) 
//#define UDDR        (SYS_REG_BASE + 0x28) 

//-------Memory Converter Registers----------------
#define MCSR_BASE_ADDR 0x0200

#define MCSR    (MCSR_BASE_ADDR + 0x0000)
#define IRR     (MCSR_BASE_ADDR + 0x0004)//Set Image Resolution Reg
#define LISAR   (MCSR_BASE_ADDR + 0x0008)//Set Image Buffer Address
#define MBOXAR  (MCSR_BASE_ADDR + 0x0030)//Set MailBox Buffer Address 

//For Load(Update) Area , if Par Packed mode disable Eric 2010/04/26 edited
#define PRXSR   (MCSR_BASE_ADDR + 0x000C)//X for Load Area
#define PRYSR   (MCSR_BASE_ADDR + 0x000E)//Y for Load Area
#define PRWR    (MCSR_BASE_ADDR + 0x0010)//W for Load Area
#define PRHR    (MCSR_BASE_ADDR + 0x0012)//H for Load Area
//Burst RW
#define MBSAR   (MCSR_BASE_ADDR + 0x0014)//Memory Burst Start Address
#define MBCR    (MCSR_BASE_ADDR + 0x0018)//Burst Count(unit:Word)

#define USCER   (MCSR_BASE_ADDR + 0x001C)//For User Define command,I80 Host Set
#define CRFSR   (MCSR_BASE_ADDR + 0x0020)
#define HIRR    (MCSR_BASE_ADDR + 0x0024)//Read this Reg for polling HRDY

#define RGB2YUVR  (MCSR_BASE_ADDR + 0x0028) //Setting RGB format
#define RGB2YUVFR (MCSR_BASE_ADDR + 0x002C) //Setting Convert Factor
#define RDSAR     (MCSR_BASE_ADDR + 0x0030) //Rotate DMA Source Address


//Pixel mode ,BPP - Bit per Pixel
#define IT8957_2BPP         0
#define IT8957_3BPP         1 
#define IT8957_4BPP         2 
#define IT8957_8BPP         3

//---------------------------------------------------------------
// IT8957 SPI HW Commands Code - 8 bits
//---------------------------------------------------------------
#define IT8957_TCON_SYS_RUN          0x01
#define IT8957_TCON_STANDBY          0x02
#define IT8957_TCON_SLEEP            0x03

#define IT8957_TCON_REG_RD           0x10
#define IT8957_TCON_REG_WR           0x11
#define IT8957_TCON_MEM_BST_RD_T     0x12
#define IT8957_TCON_MEM_BST_RD_S     0x13
#define IT8957_TCON_MEM_BST_WR       0x14
#define IT8957_TCON_MEM_BST_END      0x15

#define IT8957_TCON_LD_IMG           0x20
#define IT8957_TCON_LD_IMG_AREA      0x21
#define IT8957_TCON_LD_IMG_END       0x22

//New command for IT8957 for QUAD mode only
#define IT8957_TCON_LD_IMG_QUAD           0x40
#define IT8957_TCON_LD_IMG_AREA_QUAD      0x41
#define IT8957_TCON_MEM_BST_RD_T_QUAD     0x42
#define IT8957_TCON_MEM_BST_RD_S_QUAD     0x43
#define IT8957_TCON_MEM_BST_WR_QUAD       0x44
#define IT8957_TCON_REG_RD_QUAD           0x46
#define IT8957_TCON_REG_WR_QUAD           0x47


//***************************************************************
//User Define Command code 
//***************************************************************
#define USDEF_I80_CMD_GET_DEV_INFO           0xE0
#define USDEF_I80_CMD_TRIGGER                0xE2 

#define USDEF_HSPI_CMD_F_SET_TEMP            0x33
#define USDEF_I80_CMD_DPY_AREA               0x34
//#define USDEF_I80_CMD_PWR_SW_SEQ             0x38
#define USDEF_HSPI_CMD_PWR_SET_VCOM           0x39
#define USDEF_HSPI_CMD_UPD_WBF_FILE           0x3B
//v.1.2 above
#define USDEF_HSPI_CMD_SET_TCON_CFG           0x3C
#define USDEF_HSPI_CMD_EN_REAGL               0x3E

#define USDEF_HSPI_CMD_RGL_RD_REG             0x40
#define USDEF_HSPI_CMD_RGL_WR_REG             0x41  

#define USDEF_HSPI_CMD_SET_EPD_PWR_VOL        0x42 

//MIPI Control 
#define USDEF_HSPI_CMD_GET_MIPI_MODE           0x60
#define USDEF_HSPI_CMD_SET_MIPI_MODE           0x61

#define USDEF_HSPI_CMD_SET_FW_UPD_FUN          0x62


//***************************************************************
//  Constant Value
//***************************************************************
#define MAX_TX_SIZE_WORD  (128)   //128 words => 256 bytes

enum
{
	FW_UPDATE = 0,
	WBF_UPDATE

}UpdItem;

//*********************************************************************************
//    typedef structure
//*********************************************************************************

//--------------------------------------------
//
//--------------------------------------------
typedef struct
{
	TWord  usSignature;
	TWord  usCmdAddrL;
	TWord  usCmdAddrH;
	TWord  usRDataAddrL;
	TWord  usRDataAddrH;
	TWord  usMaxArgSize;
	TWord  usReserved[2]; //Double word alignment
}TMBHdr;


#define MAX_SW_RDATA_BUF_SIZE  64
#define MAX_ARG_CNT  16
typedef struct
{
	TWord usCmdCode;  //v.1.1 Changed 
	TWord usReserved;
	TWord WData[MAX_ARG_CNT];

}TUDefCmdArg;
//--------------------------------------------
//
//--------------------------------------------
typedef struct
{
	TWord usPanelW;
	TWord usPanelH;
	TWord usImgBufAddrL;
	TWord usImgBufAddrH;
	TWord usFWVersion[8];  //16 Bytes
	TWord usLUTVersion[8];  //16 Bytes
	TWord usWBFBufAddrL; //v.1.1 new added 
	TWord usWBFBufAddrH;
	TWord usImgBuf1AddrL;//v.1.2a new added
	TWord usImgBuf1AddrH;
}I80IT8957DevInfo;

//--------------------------------------------
//     See command User defined command :0x34
//--------------------------------------------
typedef struct
{
    TWord usX;
    TWord usY;
    TWord usW;
    TWord usH;
    TWord usDpyMode;
	TWord usDpyImgBufAddrL;
	TWord usDpyImgBufAddrH;
    
}I80DisplayInfo;
//--------------------------------------------
//     See command HW command :0x21 (LD_IMG_AREA)
//--------------------------------------------
typedef struct
{
	TWord usArg; 
    TWord usX;
    TWord usY;
    TWord usW;
    TWord usH;
}TLdImgInfo;

//TSWCmdMailBox
typedef struct
{
	TWord usOpCode;
	TWord usVSH1;
	TWord usVSL1;
	TWord usVSH2;
	TWord usVSL2;
	TWord usVSH3;
	TWord usVSL3;
	TWord usVCom;

}TEPDPwrSet;

extern TMBHdr gstHostMBInfo;//v.1.1r changed 
extern I80IT8957DevInfo stI80IT8957DevInfo;

void it8957_wait_bus_ready(void);
void it8957_send_cmd(TByte u8Cmd, TByte u8CSEnd);
void it8957_send_data(TWord usData, TByte u8CSEnd);
void it8957_send_data_cnt(TWord* pwBuf, TDWord ulSizeWord, TByte u8CSEnd);
TWord it8957_read_data(TByte u8CSEnd);
void it8957_read_data_cnt(TWord* pwBuf, TDWord ulSizeWord, TByte u8CSEnd);
void it8957_send_mailbox_cmd_arg(TByte ucCmd, TWord* p16Arg, TWord usArgCnt);
void it8957_read_mailbox_data_cnt(TWord* pwBuf, TDWord ulSize, TDWord ulOffset);
void it8957_write_reg(TWord usRegAddr, TWord usVal);
TWord it8957_read_reg(TWord usRegAddr);
void it8957_mem_burst_write(TWord* pBuf, TDWord ulMemAddr, TDWord ulSize);
void it8957_mem_burst_read(TWord* pBuf, TDWord ulMemAddr, TDWord ulSize);
void it8957_set_img_buf_addr(TDWord ulImgBufAddr);
void it8957_ld_img_area(TByte* pImgBuf, TLdImgInfo* pstLdImgInfo, TDWord ulSetImgBufAddr);
void it8957_dpy_update(TDWord ulImgBufAddr, TByte ucDpyMode, TWord usX, TWord usY, TWord usW, TWord usH);
void it8957_get_system_info(void* pBuf);
void it8957_upd_wbf_file(TByte* pWBFBuf, TDWord ulWBFByteSize);
void it8957_force_set_temperature(char cTemperature);
void it8957_get_temperature(char* pSetTemp, char* pRealTemp);

void i80_host_init(void);
void it8957_wait_dpy_ready(void);


void ite_host_example_main(void);

TByte it8957_set_epd_vcom(TWord usVCommV);
TByte it8957_set_vsx(TByte ucSetNo, TWord usVSHx, TWord usVSLx );
TWord it8957_set_mipi_mode(TByte ucMode);
TByte it8957_get_mipi_mode(void);
TByte host_get_checksum(TDWord ulMemAddr, TDWord ulSize);
TByte ite_fw_wf_upgrade(TByte ucUpdItem, TByte* pSrcBuf ,TDWord ulMemAddr, TDWord ulSFIAddr, TDWord ulSize);

#endif
