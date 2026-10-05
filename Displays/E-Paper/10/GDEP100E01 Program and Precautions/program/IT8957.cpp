#include "IT8957.h"

// **************************************************************************
// IT8957_Host_SPI_Display_SampleCode.c
//
// Implementation of example code for IT8957 Display via SPI (Host side)
//
// Copyright (c) 2018 ITE Tech. Inc. All Rights Reserved.
//
// Author: Eric Su, July 25, 2018
//
// **************************************************************************

//***************************************************************************
//    v.1.5 Released Notes
//***************************************************************************
//    1. Renaming the it8957_send_usr_cmd_arg to it8957_read_mailbox_data_cnt
//    2. Renaming the it8957_read_usr_data_cnt to it8957_read_mailbox_data_cnt
//    3. Added mailbox cmd sample code
//    4. Supposed the T1000 HRDY is connected to host gpio , see it8957_wait_bus_ready();

//*************************************************************
//  Notes
//*************************************************************
//   Assumed the SPI API as following:
//   
//   void SPIWrite(TByte* pWBuf, TDWord ulSizeByte,  TByte CS)
//   void SPIRead (TByte* pRBuf, TDWord ulSizeByte,  TByte CS)
//     
//   	CS = L : the CS will keep L when this spi transfder terminated
//      CS = H : the CS will change from L to H when this spi transfder terminated 
//
//   The SPI API function should be check by your host spi controller 
//
//*************************************************************
//  Global vairiable 
//*************************************************************
TDWord gulIT8957MailBoxAddr;
TMBHdr gstHostMBInfo;//v.1.1r changed 
I80IT8957DevInfo stI80IT8957DevInfo;

// #define __HOST_LITTLE_ENDIAN__  //Big or Little Endian for your Host platform

#ifdef __HOST_LITTLE_ENDIAN__
	//Little Endian => its needs to convert for SPI to I80
	// #define  MY_WORD_SWAP(x) (((x & 0xff00)>>8) | ((x & 0x00ff)<<8))
	inline TWord MY_WORD_SWAP(TWord x) {
		TWord temp = (x >> 8) & 0xFF;
		temp += (x << 8) & 0xFF00;
		return temp;
	}
#else
	//Big Endian => No need to convert
	#define  MY_WORD_SWAP(x) (x)
#endif
	

//*************************************************************
//  Function
//*************************************************************
void ite_swap_word_buf(TWord* pWord, TDWord ulWordCnt)
{
	#ifdef __HOST_LITTLE_ENDIAN__
	int i;
	for(i=0;i<ulWordCnt;i++)
	{
		pWord[i] = MY_WORD_SWAP(pWord[i]);
	}
	#endif
}


//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_wait_bus_ready(void)
{
	TDWord ulTimeOut = 0;
	TWord usVal;
	
	do
	{
		usVal = HRDY_GPIO_STATE; //this gpio should be connected to HRDY(MDIO3) of T1000 pin
	}while(usVal == 0);

}

//------------------------------------------------------------------
//   Send Command API 
//------------------------------------------------------------------
void it8957_send_cmd(TByte u8Cmd, TByte u8CSEnd)
{
	it8957_wait_bus_ready();//Wait HRDY
	//Send Command - 1 byte only
	SPIWrite(&u8Cmd, 1, u8CSEnd);
}

//------------------------------------------------------------------
//   Send Data API 
//------------------------------------------------------------------

//send single data (1 words)
void it8957_send_data(TWord usData, TByte u8CSEnd)
{
	//Endian convert 
	MY_WORD_SWAP(usData); 
	
	it8957_wait_bus_ready();//Wait HRDY
	//Send Data
	SPIWrite(&usData, 1, u8CSEnd);
}

//send N data (N words)
void it8957_send_data_cnt(TWord* pwBuf, TDWord ulSizeWord, TByte u8CSEnd)
{
	TDWord uCurlSizeWord;
	TDWord i;
	TByte ucCS;
	//Word Endian swap before sending data 
	ite_swap_word_buf(pwBuf, ulSizeWord);
	
	//--------------------------------------------------------------
	//  Note: the maximum transfer size is 128 words (256 bytes), 
	//        dividsion is necessary if transfer size > 128 words(256 bytes)
	//--------------------------------------------------------------
	
	uCurlSizeWord = (ulSizeWord > MAX_TX_SIZE_WORD)? MAX_TX_SIZE_WORD : ulSizeWord;
	for(i=0;i<ulSizeWord;i=i+MAX_TX_SIZE_WORD)
	{
		//Check Trnasfer size 
		if(i+MAX_TX_SIZE_WORD >= ulSizeWord )
		{
			//Last transfer , CS = L/H by u8CSEnd
			if(i+MAX_TX_SIZE_WORD > ulSizeWord)
			{
				uCurlSizeWord = ulSizeWord % MAX_TX_SIZE_WORD;
			}
			ucCS = u8CSEnd;
		}
		else
		{
			ucCS = CS_L;//CS must be L
		}
		
		
		//Send Data
		it8957_wait_bus_ready();//Wait HRDY
		SPIWrite(pwBuf+i, uCurlSizeWord, ucCS);
	}
	
}

//------------------------------------------------------------------
//   Read Data API 
//------------------------------------------------------------------
TWord it8957_read_data(TByte u8CSEnd)
{
    TWord wRData; 
	TWord wDummy;
	
	it8957_wait_bus_ready();//Wait HRDY
	
	//Read 1 Dummy word and keep CS = L
	SPIRead(&wDummy, 1, CS_L);
	
	//Read SPI
	SPIRead(&wRData, 1, u8CSEnd);
	
	//Endian convert
	wRData = MY_WORD_SWAP(wRData);

	return wRData;
	
}


void it8957_read_data_cnt(TWord* pwBuf, TDWord ulSizeWord, TByte u8CSEnd)
{
	TDWord i;
	TWord wDummy;
	
	it8957_wait_bus_ready();//Wait HRDY
	
	//Read 1 Dummy word and keep CS = L
	SPIRead(&wDummy, 1, CS_L);
	
	//Read SPI
	SPIRead(pwBuf, ulSizeWord, u8CSEnd);
	
	//Endian swap after receiving data 
	ite_swap_word_buf(pwBuf, ulSizeWord);

}

//------------------------------------------------------------------
//   Send mailbox command and Data API 
//------------------------------------------------------------------
void it8957_send_mailbox_cmd_arg(TByte ucCmd, TWord* p16Arg, TWord usArgCnt)
{
	TUDefCmdArg stUDefCmdArg;
	TDWord ulCmdArgSize;

	//Set command - v.1.1 changed
	stUDefCmdArg.usCmdCode = ucCmd;//Bit[15:8] is reserved

	//Set Arguments
	if (usArgCnt > 0 && p16Arg != NULL)
	{
		//memcpy(stUDefCmdArg.WData, p16Arg, usArgCnt * 2);
		for (TWord i = 0; i < usArgCnt; i++)
		{
			stUDefCmdArg.WData[i] = p16Arg[i];
		}
	}

	//Write Command and data to IT8957 Command Table buffer for registry 
	ulCmdArgSize = ((TDWord)&stUDefCmdArg.WData - (TDWord)&stUDefCmdArg.usCmdCode) + usArgCnt*2;//Bytes
	//it8957_wait_bus_ready();
	it8957_mem_burst_write((TWord *)&stUDefCmdArg.usCmdCode, gstHostMBInfo.usCmdAddrL | (gstHostMBInfo.usCmdAddrH << 16), ulCmdArgSize/2);

	//send trigger commmand to IT8957 to perform user defined command  
	//it8957_wait_bus_ready();
	it8957_send_cmd(USDEF_I80_CMD_TRIGGER, CS_H);//Send Trigger command

}

//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_read_mailbox_data_cnt(TWord* pwBuf, TDWord ulSize, TDWord ulOffset)
{
	//Check HRDY(IT8957 Ready)?
	it8957_wait_bus_ready();
	//Read Data from IT8957 read buffer
	it8957_mem_burst_read(pwBuf, gstHostMBInfo.usRDataAddrL | (gstHostMBInfo.usRDataAddrH << 16) + ulOffset, ulSize);
}

//------------------------------------------------------------------
//   Register Write API 
//------------------------------------------------------------------
void it8957_write_reg(TWord usRegAddr, TWord usVal)
{
	it8957_send_cmd(IT8957_TCON_REG_WR, CS_L);
	it8957_send_data(usRegAddr, CS_L);
	it8957_send_data(usVal, CS_H);
}
//------------------------------------------------------------------
//   Register Read API 
//------------------------------------------------------------------
TWord it8957_read_reg(TWord usRegAddr)
{
	TWord val;
	it8957_send_cmd(IT8957_TCON_REG_RD, CS_L);
	it8957_send_data(usRegAddr, CS_L);
	val = it8957_read_data(CS_H);
	return val;
}

//------------------------------------------------------------------
//   
//------------------------------------------------------------------

void it8957_mem_burst_write(TWord* pBuf, TDWord ulMemAddr, TDWord ulSize)
{
	it8957_send_cmd(IT8957_TCON_MEM_BST_WR, CS_L);

	it8957_send_data((TWord)(ulMemAddr & 0xFFFF), CS_L);
	it8957_send_data((TWord)((ulMemAddr >> 16) & 0xFFFF), CS_L);

	it8957_send_data((TWord)(ulSize & 0xFFFF), CS_L);
	it8957_send_data((TWord)((ulSize >> 16) & 0xFFFF), CS_L);
	
	//Send Write Data
	it8957_send_data_cnt(pBuf, ulSize, CS_H);

	it8957_send_cmd(IT8957_TCON_MEM_BST_END, CS_H);
}
//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_mem_burst_read(TWord* pBuf, TDWord ulMemAddr, TDWord ulSize)
{
	//Send Trigger
	it8957_send_cmd(IT8957_TCON_MEM_BST_RD_T, CS_L);
	it8957_send_data((TWord)(ulMemAddr & 0xFFFF), CS_L);
	it8957_send_data((TWord)((ulMemAddr >> 16) & 0xFFFF), CS_L);

	it8957_send_data((TWord)(ulSize & 0xFFFF), CS_L);
	it8957_send_data((TWord)((ulSize >> 16) & 0xFFFF), CS_H);
	
	//Get Read Data
	it8957_send_cmd(IT8957_TCON_MEM_BST_RD_S, CS_L);
	it8957_read_data_cnt(pBuf, ulSize, CS_H);

	it8957_send_cmd(IT8957_TCON_MEM_BST_END, CS_H);
}

//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_set_img_buf_addr(TDWord ulImgBufAddr)
{
	//Set Address Low
	it8957_write_reg(LISAR, ulImgBufAddr & 0xFFFF);

	//Set Address High 
	it8957_write_reg(LISAR+2, (ulImgBufAddr >> 16) & 0xFFFF);
}

//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_ld_img_area(TByte* pImgBuf, TLdImgInfo* pstLdImgInfo, TDWord ulSetImgBufAddr)
{
	TWord wData[5];
	TDWord dwCnt;

	if (ulSetImgBufAddr != NULL)
	{
		it8957_set_img_buf_addr(ulSetImgBufAddr);//Optional setting
	}

	//Send Command 0x21
	it8957_send_cmd(IT8957_TCON_LD_IMG_AREA, CS_L);

	//Ready for Send Arguments
	wData[0] = pstLdImgInfo->usArg;
	wData[1] = pstLdImgInfo->usX;
	wData[2] = pstLdImgInfo->usY;
	wData[3] = pstLdImgInfo->usW;
	wData[4] = pstLdImgInfo->usH;

    //Send Arguments 
	it8957_send_data_cnt(wData, 5, CS_L);
	//load image data
	dwCnt = pstLdImgInfo->usW*pstLdImgInfo->usH / 2;
	it8957_send_data_cnt((TWord*)pImgBuf, dwCnt, CS_H);

	//Send Load Image End - 0x22 Command
	it8957_send_cmd(IT8957_TCON_LD_IMG_END, CS_H);

}
//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_dpy_update(TDWord ulImgBufAddr, TByte ucDpyMode, TWord usX, TWord usY, TWord usW, TWord usH)
{
	TWord usArg[7];
	
	I80_DBG_INFO("ite_dpy_update\r\n");

	//Set Arguments
	usArg[0] = (usX);
	usArg[1] = (usY);
	usArg[2] = (usW);
	usArg[3] = (usH);
	usArg[4] = (TWord)ucDpyMode; //Display waveform mode , Mode 2 (GC)
	usArg[5] = (TWord)(ulImgBufAddr & 0xFFFF);
	usArg[6] = (TWord)((ulImgBufAddr >> 16) & 0xFFFF);

	//Send User defined command - Display and 7 arguments
	it8957_send_mailbox_cmd_arg(USDEF_I80_CMD_DPY_AREA, usArg, 7);
		
}



//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_get_system_info(void* pBuf)
{
	TByte ucSelect;
	int i;
	//Send user define command 0xE0 (without arguments) and trigger
	it8957_send_mailbox_cmd_arg(USDEF_I80_CMD_GET_DEV_INFO, NULL, 0);
	// delay(100);
	//Read the Device information from IT8957 ReadBuffer for User defined command
	it8957_read_mailbox_data_cnt((TWord*)pBuf, (TDWord)(sizeof(I80IT8957DevInfo) / 2), 0 );//offset = 0, read start of rbuf
}

//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_upd_wbf_file(TByte* pWBFBuf, TDWord ulWBFByteSize)
{
	TDWord ulMemAddr;
	
	ulMemAddr = stI80IT8957DevInfo.usWBFBufAddrL | (stI80IT8957DevInfo.usWBFBufAddrH << 16);
	//Write wbf file to wbf buffer of IT8957
	it8957_mem_burst_write((TWord *)pWBFBuf, ulMemAddr, ulWBFByteSize/2 );
	
	//Send User defined command 0x3B to start decode wbf
	it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_UPD_WBF_FILE,  NULL, 0);
	
	//Wait for Wbf parsing ready(optional)
	it8957_wait_bus_ready();
	
	
	
}

//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void it8957_force_set_temperature(char cTemperature)
{
	TWord usArg[2];
	
	usArg[0] = 1; //0-Get, 1-Set 
	usArg[1] = cTemperature;
		
	//Send User defined command 0x3B to start decode wbf
	it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_F_SET_TEMP,  usArg, 2);
		
	//Wait for Wbf parsing ready(optional)
	it8957_wait_bus_ready();
	
}

//------------------------------------------------------------------
//   char* pSetTemp - pointer of variable char Set Value
//   char* pRealTemp - pointer of variable char Real temperature by thermal sensor
//------------------------------------------------------------------
void it8957_get_temperature(char* pSetTemp, char* pRealTemp)
{
	TWord usArg[2];
	TWord usRData[2];
	
	usArg[0] = 0;//0-Get, 1-Set 
	
	//Send User defined command 0x3B to start decode wbf
	it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_F_SET_TEMP,  usArg, 1);
		
    //Wait for Wbf parsing ready(optional)
	//it8957_wait_bus_ready();
	it8957_read_mailbox_data_cnt((TWord*)usRData, 2, 0);
		
	*pRealTemp = (TByte)usRData[0]; //Get the real temperature value by thermal sensor 
	*pSetTemp  = (TByte)usRData[1]; //Get the last force set temperature value 
	
}


//------------------------------------------------------------------
//   
//------------------------------------------------------------------
void i80_host_init(void)
{
	int i;
	TByte ucSelect;
	TWord usVal;
	TDWord ulVal;
	Serial.println("i80_host_init");
	delay(1000);
	//Set Host Packed mode, 0x0004 = 1
	it8957_write_reg(I80CPCR, 0x0001);
	Serial.println("i80_host_init - 1");
	//Get MailBox Address
	it8957_wait_bus_ready();
	delay(100);
	gulIT8957MailBoxAddr = (it8957_read_reg(MBOXAR + 2) << 16) | it8957_read_reg(MBOXAR);
	// gulIT8957MailBoxAddr |= 0x80000000;
	Serial.printf("Get MaixBox addr = 0x%08X\r\n", gulIT8957MailBoxAddr);
	
	if(gulIT8957MailBoxAddr == 0xFFFFFFFF) {
		Serial.println("Waiting for T2001 FW Initial Burning...");
		IT8957_IO_ResetHold();
		while(1);
	}



	//Read MaixlBox information from IT8957
	it8957_mem_burst_read((TWord*)&gstHostMBInfo, gulIT8957MailBoxAddr, sizeof(TMBHdr)/2);
	Serial.println("Host MB Info:");
	Serial.printf("----Signature: %04X\n", gstHostMBInfo.usSignature);
	Serial.printf("----CmdTabAddr: 0x%04X%04X\n", gstHostMBInfo.usCmdAddrH, gstHostMBInfo.usCmdAddrL);
	Serial.printf("----RDataAddr: 0x%04X%04X\n", gstHostMBInfo.usRDataAddrH, gstHostMBInfo.usRDataAddrL);
	Serial.printf("----MaxArgSize: 0x%04X\n", gstHostMBInfo.usMaxArgSize);

	//Get System informaton
	it8957_get_system_info(&stI80IT8957DevInfo);


	//you can skip this flow becuase the image buffer address was also got from it8957_get_system_info already
	//Get Image buffer address by read register? (optional , but it needs IT8957 FW initial set this register first)
	// Serial.printf("Get imagebuffer address again by another way\r\n");
	// stI80IT8957DevInfo.usImgBufAddrL = it8957_read_reg(LISAR);
	// stI80IT8957DevInfo.usImgBufAddrH = it8957_read_reg(LISAR+2);

	//Get it8957 Image Buffer Address
	TDWord gulIT8957ImgBuf  = stI80IT8957DevInfo.usImgBufAddrL | (stI80IT8957DevInfo.usImgBufAddrH << 16);        
	I80_DBG_INFO("Get Image Buffer Address:%X\r\n", gulIT8957ImgBuf);
	TDWord gulPanelW = stI80IT8957DevInfo.usPanelW;
	TDWord gulPanelH = stI80IT8957DevInfo.usPanelH;

	I80_DBG_INFO("Panel (w,h) = (%d,%d)\r\n", gulPanelW, gulPanelH);

	I80_DBG_INFO("FW  Version = %s\r\n", stI80IT8957DevInfo.usFWVersion);
	I80_DBG_INFO("LUT Version = %s\r\n", stI80IT8957DevInfo.usLUTVersion);

	I80_DBG_INFO("Get WBF Buffer Address:%X\r\n", stI80IT8957DevInfo.usWBFBufAddrL | (stI80IT8957DevInfo.usWBFBufAddrH << 16));
	I80_DBG_INFO("Get Image Buffer1 Address:%X\r\n", stI80IT8957DevInfo.usImgBuf1AddrL | (stI80IT8957DevInfo.usImgBuf1AddrH << 16));

	if((stI80IT8957DevInfo.usWBFBufAddrL | (stI80IT8957DevInfo.usWBFBufAddrH << 16)) == 0){
		Serial.println("Waiting for T2001 WBF Initial Burning...");
		while(1);
	}
	it8957_set_img_buf_addr(gulIT8957ImgBuf);

	//Host Buffer allocate
	// gpHostImgBuf = sdk_malloc(gulPanelW*gulPanelH);
	// if (gpHostImgBuf == NULL)
	// {
	// 	Serial.printf("gpHostImgBuf Null!!!\r\n");
	// }
	// else
	// {
	// 	Serial.printf("Host Buffer address: %X\r\n", (TDWord)gpHostImgBuf);
	// }

	

}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
#if 0
void ite_host_example_main(void)
{
	TLdImgInfo stLdImgInfo;
	TByte ucSetTemp;
	TByte ucRealTemp;
	// TByte ucEnReaglEngine = 1;
	TDWord ulRLCurImgBuf;
	TDWord ulProcessedBuf;
	
	//----------------------------------------------------------------
	//Host initial 
	//----------------------------------------------------------------
	i80_host_init();
	
	//----------------------------------------------------------------
	//   Load WBF file to wbf buffer of IT8951
	//----------------------------------------------------------------
	//Please prepare your wbf file here
	
	//Updaate wbf file 
	// it8957_upd_wbf_file(WBFFileBuf, FileSize);
	//----------------------------------------------------------------
	it8957_force_set_temperature(25); //example: 25C
	
	
	//prepare the pixels data in gpHostImgBuf
	memset(gpHostImgBuf, 0xF0,gulPanelW*gulPanelH );//8 bpp image, 0xF0 -White, 0x00 - Black, 0x80 - Gray 8
	
	//Send Image
	stLdImgInfo.usArg = ((PIXEL_8BPP << 4) | (IT8951_LDIMG_B_ENDIAN << 8)); 
  stLdImgInfo.usX = 0;
	stLdImgInfo.usY = 0;
	stLdImgInfo.usW = gulPanelW;
	stLdImgInfo.usH = gulPanelH;
	if(ucEnReaglEngine == 1)
	{
		//Send to Iamge buffer1 first
		ulRLCurImgBuf = (TDWord)(stI80IT8957DevInfo.usImgBuf1AddrL | (stI80IT8957DevInfo.usImgBuf1AddrH << 16));
		it8957_ld_img_area(gpHostImgBuf, &stLdImgInfo, ulRLCurImgBuf ); 
		
		//Do Reagl processing
		// ulProcessedBuf = (TDWord)(stI80IT8957DevInfo.usImgBufAddrL | (stI80IT8957DevInfo.usImgBufAddrH << 16));
		// it8957_enable_reagl_processing(ulRLCurImgBuf, ulProcessedBuf);
		
	}
	else
	{
	    it8957_ld_img_area(gpHostImgBuf, &stLdImgInfo, NULL); //NULL means use default Image buffer address without addional setting 
	}
	
	//Set EPD Display Voltage 
	//VSH3 = 15 and VSL3 = -15.8 
	it8957_set_vsx(3, 15000, 15800);//Unit: mV, unsigned  
	//Set VCom -2.49
	it8957_set_epd_vcom(2490);//Unit: mV, unsigned
	
	//----------------------------------------------------------------
	//   Display with Mode 2
	//----------------------------------------------------------------
	//  gulIT8957ImgBuf - Default image buffer address 
	//  or you can assign another image buffer address for displaying via this display api  
	//-------------------------------------------------------------------------------
	it8957_dpy_update(gulIT8957ImgBuf, 2, 0, 0, (TWord)gulPanelW, (TWord)gulPanelH);
	it8957_wait_dpy_ready();//Optional , you can skip it if you don't want to wait display ready
	
	
}
#endif


void it8957_wait_dpy_ready(void)
{
    TDWord ulTimeOut = 0;

	//Wait SPI Bus Ready
    it8957_wait_bus_ready();
	
    while(it8957_read_reg(INF_DARCR0) & (1<<6));		

}

//------------------------------------------------------------------
//   Set EPD power VCom
//   TWord usVCommV  => ABS(VCommV)
//   Unit: mV   (e.g. -2.52  Set => 2520 mV)
//------------------------------------------------------------------
TByte it8957_set_epd_vcom(TWord usVCommV)
{
	TEPDPwrSet stEPDPwrSet;

	stEPDPwrSet.usOpCode = 1;
	stEPDPwrSet.usVSH1 = 0;//0- No Set, it will not set by IT8957
	stEPDPwrSet.usVSL1 = 0;
	stEPDPwrSet.usVSH2 = 0;
	stEPDPwrSet.usVSL2 = 0;
	stEPDPwrSet.usVSH3 = 0;
	stEPDPwrSet.usVSL3 = 0;
	stEPDPwrSet.usVCom = usVCommV;

	it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_SET_EPD_PWR_VOL, (TWord*)&stEPDPwrSet, 8);

	return 1;
}
//------------------------------------------------------------------
//   Set EPD VSHx and VSLx
//   ucSetNo => Index of VSH and VSL   
//   TWord usVCommV  => ABS(VCommV)
//   Unit: mV   (e.g. -2.52  Set => 2520 mV)
//------------------------------------------------------------------
TByte it8957_set_vsx(TByte ucSetNo, TWord usVSHx, TWord usVSLx )
{
	TEPDPwrSet stEPDPwrSet;

	//Clear All 0
	memset((void*)&stEPDPwrSet, 0x00, sizeof(TEPDPwrSet));

	//Set Op Code
	stEPDPwrSet.usOpCode = 1; //???

	if (ucSetNo == 1)
	{
		stEPDPwrSet.usVSH1 = usVSHx;//0- No Set, it will not set by IT8957
		stEPDPwrSet.usVSL1 = usVSLx;
	}
	else if (ucSetNo == 2)
	{
		stEPDPwrSet.usVSH2 = usVSHx;
		stEPDPwrSet.usVSL2 = usVSLx;
	}
	else if (ucSetNo == 3)
	{
		stEPDPwrSet.usVSH3 = usVSHx;
		stEPDPwrSet.usVSL3 = usVSLx;
	}

	//Others are set 0

	it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_SET_EPD_PWR_VOL, (TWord*)&stEPDPwrSet, 8);

	return 1;
}


//----------------------------------------------------------------
//    Set Mipi Mode
//----------------------------------------------------------------
TWord it8957_set_mipi_mode(TByte ucMode)
{
	TWord usArg[1];
	
	//Set Arguments
	usArg[0] = ucMode;

	//Send mailbox cmd - 0x61 Set mipi mode 
	it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_SET_MIPI_MODE, usArg, 1);
}
//----------------------------------------------------------------
//    Get Mipi Mode
//----------------------------------------------------------------
TByte it8957_get_mipi_mode(void)
{
	TWord usArg[1];
	TWord usRData[1];
	//Set Arguments
	// usArg[0] = ucMode;

	//Send mailbox cmd - 0x60 Get mipi mode 
	// it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_GET_MIPI_MODE, usArg, 1);
	it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_GET_MIPI_MODE, NULL, 0);
	
	//Wait for Get mode ready(optional)
	it8957_wait_bus_ready();
	
	//Read mailbox
	it8957_read_mailbox_data_cnt((TWord*)usRData, 1, 0);
	
	return (TByte)usRData[0] ;
}


// ****************************************************************************************
// Function name: host_get_checksum()
// 
// Description: 
//   caculate the checksum api function
//   the caculation is the same with T1000 
// Arguments:
//   TDWord ulMemAddr - T1000 Temp buffer Memory address (TBD or image buffer address)
//   TDWord ulSize - size of upgrade file  (unit: bytes) 
//
// Return Values:
//   NULL.
// Note: 
//   using CheckSum 8bits (1's complement)
//   
// ****************************************************************************************
TByte host_get_checksum(TDWord ulMemAddr, TDWord ulSize)
{
	TByte ucCheckSum = 0;
	TDWord i;
	TByte* pByte;

	pByte = (TByte*)ulMemAddr;

	//sum
	for (i = 0; i < ulSize; i++)
	{
		ucCheckSum += pByte[i];
	}

	//1's complement 
	ucCheckSum = 0xFF - ucCheckSum;

	Serial.printf("Host Cal CheckSum = %02X\r\n", ucCheckSum);

	return ucCheckSum;
}
// ****************************************************************************************
// Function name: ite_fw_wf_upgrade()
// 
// Description: 
//   fw upgrade api function
// Arguments:
//   TByte ucUpdItem:  0 - FW upgrade, 1- wbf upgrade
//   TByte* pSrcBuf - SourceData buffer
//   TDWord ulMemAddr - T1000 Temp buffer Memory address (TBD or image buffer address)
//   TDWord ulSFIAddr - SPI Flash target address (e.g FW- 0x00000000,  wbf- 0x00200000)
//   TDWord ulSize - size of upgrade file  (unit: bytes) 
//
// Return Values:
//   NULL.
// Note: 
//   
// ****************************************************************************************
#if 0
TByte ite_fw_wf_upgrade(TByte ucUpdItem, TByte* pSrcBuf ,TDWord ulMemAddr, TDWord ulSFIAddr, TDWord ulSize)
{
	TWord usArg[1];
	TWord usReadData[1];
	TByte ucGetChkSum;
	TByte ucHostChkSum;
	TByte ucRet;

	//---------------------------------------
	// Write FW/Wbf Binary to T1000 Memory
	//---------------------------------------
	it8957_mem_burst_write(pSrcBuf, ulMemAddr, ulSize / 2);

	//---------------------------------------
	// Get Check Sum and check
	//---------------------------------------
	//Send mailbox cmd - 0x62 Set mipi mode 
	usArg[0] = FW_UPD_MEM_ADDR_SIZE_CHECKSUM;
	usArg[1] = (TWord)ulMemAddr & 0xFFFF;
	usArg[2] = (TWord)(ulMemAddr >> 16) & 0xFFFF;
	usArg[3] = (TWord)ulSize & 0xFFFF;
	usArg[4] = (TWord)(ulSize >> 16) & 0xFFFF;

	it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_SET_FW_UPD_FUN, usArg, 5);
	//Get CheckSum
	it8957_read_mailbox_data_cnt(usReadData, 1, 0);
	ucGetChkSum = (TByte)(usReadData[0] & 0xFF);

	//Compare CheckSum
	ucHostChkSum = host_get_checksum((TDWord)pSrcBuf, ulSize);
	if (ucHostChkSum != ucGetChkSum)
	{
		//the caculated CheckSum is not matched with T1000's
		Serial.printf("CheckSum is not matched Host = %02X, T1000: %02X\r\n", ucHostChkSum, ucGetChkSum);
		ucRet = 0x0F;
	}
	else
	{
		//---------------------------------------
		//Start FW/Wbf upgrade
		//---------------------------------------
		usArg[0] = (ucUpdItem == FW_UPDATE) ? FW_UPD_START : WBF_UPD_START;
		usArg[1] = (TWord)ulMemAddr & 0xFFFF;
		usArg[2] = (TWord)(ulMemAddr >> 16) & 0xFFFF;
		usArg[3] = (TWord)ulSize & 0xFFFF;
		usArg[4] = (TWord)(ulSize >> 16) & 0xFFFF;
		usArg[5] = (TWord)ulSFIAddr & 0xFFFF;
		usArg[6] = (TWord)(ulSFIAddr >> 16) & 0xFFFF;

		it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_SET_FW_UPD_FUN, usArg, 7);

		//Wait HRDY Ready (L->H when fw upgrade finished)
		it8957_wait_bus_ready();

		//---------------------------------------
		//  Get Upgrade status
		//---------------------------------------
		usArg[0] = FW_UPD_GET_STATUS;
		it8957_send_mailbox_cmd_arg(USDEF_HSPI_CMD_SET_FW_UPD_FUN, usArg, 1);
		//Get Status
		it8957_read_mailbox_data_cnt(usReadData, 1, 0);

		if ((usReadData[0] & 0xFF) == 0x00)
		{
			//Pass
			Serial.printf("Update Passed, Status = %X\r\n", usReadData[0]);
			ucRet = 0x00;
		}
		else
		{
			//Update error
			Serial.printf("Update Failed, Status = %X\r\n", usReadData[0]);
			ucRet = 0x0F;
		}

		
	}

	return ucRet;
}

#endif



