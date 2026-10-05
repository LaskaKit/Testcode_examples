#ifndef __EL100UF1_H__
#define __EL100UF1_H__
#include "IT8957_IO.h"
#include "IT8957.h"


#define EPD_WIDTH   1600
#define EPD_HEIGHT  1200
#define EPD_FRAME_SIZE (EPD_HEIGHT * EPD_WIDTH)

class EL100UF1{
  public:
    EL100UF1(void);
    ~EL100UF1(void);
    void EL100UF1_Init(void);
    void EL100UF1_SendPicData(uint8_t * pic_data, uint32_t length);
    void EL100UF1_Flip(uint8_t * pic_data);
    void EL100UF1_Update(void);
    void EL100UF1_DeepSleep(void);
    void EL100UF1_Deinit(void);
    // 标志：正在执行刷新，避免在刷新进行中断电
    volatile bool display_busy;
};

extern EL100UF1 epd;
extern unsigned char * frame_buffer;
#endif
