#include "EL100UF1.h"


EL100UF1 epd;
unsigned char * frame_buffer;

EL100UF1::EL100UF1() {
}

EL100UF1::~EL100UF1() {
}

//上电、复位、寄存器初始化
void EL100UF1::EL100UF1_Init(void)
{
    IT8957_IO_Initialize();
    Tcon_PowerCtrl(1);
    IT8957_IO_Reset();
    i80_host_init();
    
}

//下电
void EL100UF1::EL100UF1_Deinit(void)
{
	IT8957_IO_Deinitialize();
  Tcon_PowerCtrl(0);
}

#define PALETTE_T2001_E6_SIZE 10
unsigned char PALETTE_AIO_E6_MAPPING_T2001_4bit[PALETTE_T2001_E6_SIZE] = {
    0x00, 0x40, 0x80, 0xC0, 0x20, 0x60, 0xE0, 0x50, 0xA0, 0xF0
};

#define IT8951_LDIMG_B_ENDIAN 1
#define IT8951_LDIMG_L_ENDIAN 0

//图片传输过程，可能调用多次。
void EL100UF1::EL100UF1_SendPicData(uint8_t * pic_data, uint32_t length)
{
  TLdImgInfo stLdImgInfo;
	//Send Image
	stLdImgInfo.usArg = ((IT8957_8BPP << 4) | (IT8951_LDIMG_L_ENDIAN << 8)); 
  stLdImgInfo.usX = 0;
	stLdImgInfo.usY = 0;
	stLdImgInfo.usW = stI80IT8957DevInfo.usPanelW;
	stLdImgInfo.usH = stI80IT8957DevInfo.usPanelH;
  // memset(pic_data, 0xF0, EPD_FRAME_SIZE);
  it8957_ld_img_area(pic_data, &stLdImgInfo, NULL); //NULL means use default Image buffer address without addional setting 
}

void EL100UF1::EL100UF1_Flip(uint8_t * pic_data) {
	uint8_t *temp_line = (uint8_t *)malloc(EPD_WIDTH);
	int head_line_pos, tail_line_pos;
	uint8_t * head;
	uint8_t * tail;
	for(int i=0; i<EPD_HEIGHT/2; i++) {
		head_line_pos = i*EPD_WIDTH;
		tail_line_pos = EPD_FRAME_SIZE - EPD_WIDTH - head_line_pos;
		head = pic_data + head_line_pos;
		tail = pic_data + tail_line_pos;
		memcpy(temp_line, head, EPD_WIDTH);
		memcpy(head, tail, EPD_WIDTH);
		memcpy(tail, temp_line, EPD_WIDTH);
	}
	free(temp_line);
}

//发送刷屏指令，并等待刷屏结束
void EL100UF1::EL100UF1_Update(void)
{
  it8957_force_set_temperature(25); //example: 25C

  //Set EPD Display Voltage 
	it8957_set_vsx(1, 7000, 7000);//Unit: mV, unsigned  
	it8957_set_vsx(2, 7200, 9750);//Unit: mV, unsigned  
	it8957_set_vsx(3, 15000, 15000);//Unit: mV, unsigned  
	//Set VCom -2.49
	it8957_set_epd_vcom(1640);//Unit: mV, unsigned
	
	//----------------------------------------------------------------
	//   Display with Mode 2
	//----------------------------------------------------------------
	//  gulIT8957ImgBuf - Default image buffer address 
	//  or you can assign another image buffer address for displaying via this display api  
	//-------------------------------------------------------------------------------
	TDWord gulIT8957ImgBuf  = stI80IT8957DevInfo.usImgBufAddrL | (stI80IT8957DevInfo.usImgBufAddrH << 16);
	TDWord gulPanelW = stI80IT8957DevInfo.usPanelW;
	TDWord gulPanelH = stI80IT8957DevInfo.usPanelH;

	//Hybrid WF. MODE 2 For ACVCOM
	it8957_dpy_update(gulIT8957ImgBuf, 2/* WF Mode = 0 for E6*/, 0, 0, (TWord)gulPanelW, (TWord)gulPanelH);
	it8957_wait_dpy_ready();//Optional , you can skip it if you don't want to wait display ready
	delay(100);	//Must Wait 100ms.
	// MODE0 For DCVCOM Picture display.
	it8957_dpy_update(gulIT8957ImgBuf, 0/* WF Mode = 0 for E6*/, 0, 0, (TWord)gulPanelW, (TWord)gulPanelH);
	it8957_wait_dpy_ready();//Optional , you can skip it if you don't want to wait display ready
}




//刷屏结束，进入深休眠
void EL100UF1::EL100UF1_DeepSleep(void)
{

}