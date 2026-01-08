/* Display test for Good Display GDEH0169E01
 * example from Good Display is used
 * 
 * Board:   LaskaKit ESP32-S3-DEVKit   https://www.laskakit.cz/laskakit-esp32-s3-devkit/
 * Display: Good Display GDEH0169E01        
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>


#include"image.h"
// Pin Definition
#define BUSY_PIN    4     // Busy信号
#define NRST_PIN    5     // nRST
#define NDC_PIN     6     // nDC
#define CSB_PIN     7     // CSB (Master CS)
#define SCLK_PIN    15    // SCLK
#define MOSI_PIN    16    // MOSI (也用作MISO)x
#define CSB2_PIN    17    // CSB2 (Slave CS)
#define MS_PIN      18    // MS

#define POWER 47


// Mode definition (corrected definition to match function logic: 0 - Master IC exclusive, 1 - Slave IC exclusive, 2 - Master and slave ICs share)
#define MASTER_ONLY         0
#define SLAVE_ONLY          1
#define MASTER_SLAVE        2

#define TEMPTR_ON           0xFF
#define TEMPTR_OFF          0

#define WHITE               0x11
#define BLACK               0x00
#define RED                 0x33
#define YELLOW              0x22
#define BLUE                0x55
#define GREEN               0x66

#define PIC_HALF            0xFC        // Half an image
#define PIC_A               0xFD        // Full image
#define STRIPE              0xFE
#define IMAGE               0xFF

// Global variables
unsigned char User_Read_Data[128] = {0};
unsigned char Temptr_Cur = 0;
unsigned char OTP_PWR[5] = {0};
unsigned char temp;

// Pin control macro
#define SDA_H      digitalWrite(MOSI_PIN, HIGH)
#define SDA_L      digitalWrite(MOSI_PIN, LOW)
#define SCLK_H     digitalWrite(SCLK_PIN, HIGH)
#define SCLK_L     digitalWrite(SCLK_PIN, LOW)
#define nRST_H     digitalWrite(NRST_PIN, HIGH)
#define nRST_L     digitalWrite(NRST_PIN, LOW)
#define nDC_H      digitalWrite(NDC_PIN, HIGH)
#define nDC_L      digitalWrite(NDC_PIN, LOW)
#define CSB_H      digitalWrite(CSB_PIN, HIGH)
#define CSB_L      digitalWrite(CSB_PIN, LOW)
#define CSB2_H     digitalWrite(CSB2_PIN, HIGH)
#define CSB2_L     digitalWrite(CSB2_PIN, LOW)
#define MS_H       digitalWrite(MS_PIN, HIGH)
#define MS_L       digitalWrite(MS_PIN, LOW)

// Delay function
void Delay_ms(int time) {
  delay(time);
}

void Delay_S(int time) {
  delay(time * 1000);
}

// System initialization
void Sys_init(void) {
  pinMode(NRST_PIN, OUTPUT);
  pinMode(NDC_PIN, OUTPUT);
  pinMode(CSB_PIN, OUTPUT);
  pinMode(CSB2_PIN, OUTPUT);
  pinMode(MS_PIN, OUTPUT);
  pinMode(BUSY_PIN, INPUT);
  pinMode(SCLK_PIN, OUTPUT);
  pinMode(MOSI_PIN, OUTPUT);
  
// Initial state
  nRST_H;
  nDC_H;
  CSB_H;
  CSB2_H;
  MS_H;
  SCLK_L;
  SDA_H;
  
// Ensure all pins are initially set to the correct state
  digitalWrite(NRST_PIN, HIGH);
  digitalWrite(NDC_PIN, HIGH);
  digitalWrite(CSB_PIN, HIGH);
  digitalWrite(CSB2_PIN, HIGH);
  digitalWrite(MS_PIN, HIGH);
  digitalWrite(SCLK_PIN, LOW);
  digitalWrite(MOSI_PIN, HIGH);
}

void ReadBusy(void) {
  unsigned long timeout = millis() + 10000; // 5-second timeout
  while(1) {
    if(digitalRead(BUSY_PIN)) { // High level indicates idle
      Serial.println("Busy signal cleared");
      break;
    }
    Delay_ms(10);
    if(millis() > timeout) {
      Serial.println("Error: Busy signal timeout");
      break;
    }
  }      
}

void Reset(void) {						
  nRST_H;
  Delay_ms(30);
  nRST_L;		
  Delay_ms(30);	
  nRST_H;
  Delay_ms(100); // Increase the waiting time after reset
  Serial.println("Reset completed");
}

void SPI4W_WriteCom(unsigned char init_com) {
  unsigned char temp_com;
  unsigned char scnt;
  
  pinMode(MOSI_PIN, OUTPUT); // Ensure it is set to output before writing.
  temp_com = init_com;
  SCLK_L;
  delayMicroseconds(2);
  nDC_L;
  delayMicroseconds(2);
  
  for(scnt = 0; scnt < 8; scnt++) {
    if(temp_com & 0x80)
      SDA_H;
    else
      SDA_L;
    
    delayMicroseconds(1);
    SCLK_H;
    delayMicroseconds(2); // Increase SCLK high-level time
    SCLK_L;
    temp_com = temp_com << 1;
  }
  delayMicroseconds(2);
}

void SPI4W_WriteData(unsigned char init_data) {
  unsigned char temp_data;
  unsigned char scnt;
  
  pinMode(MOSI_PIN, OUTPUT); // 确保写入前设置为输出
  temp_data = init_data;
  SCLK_L;
  delayMicroseconds(2);
  nDC_H;
  delayMicroseconds(2);
  
  for(scnt = 0; scnt < 8; scnt++) {
    if(temp_data & 0x80)
      SDA_H;
    else
      SDA_L;
    
    delayMicroseconds(1);
    SCLK_H;
    delayMicroseconds(2); // 增加SCLK高电平时间
    SCLK_L;
    temp_data = temp_data << 1;
  }
  delayMicroseconds(2);
}

unsigned char SPI4W_ReadData(void) {
  unsigned char scnt, temp = 0;   
  
  pinMode(MOSI_PIN, INPUT); // 读取时设置为输入
  delayMicroseconds(1);
  SCLK_L;
  delayMicroseconds(2);
  nDC_H;
  delayMicroseconds(2);
  
  for(scnt = 0; scnt < 8; scnt++) {    
    SCLK_H;
    delayMicroseconds(2);
    if(digitalRead(MOSI_PIN))
      temp = (temp << 1) | 0x01;
    else
      temp = temp << 1;
    
    delayMicroseconds(1);
    SCLK_L;
    delayMicroseconds(2);
  }
  delayMicroseconds(2);
  
  pinMode(MOSI_PIN, OUTPUT); // 读取后恢复为输出
  return temp;   
}

void MsDev_WriteCom(unsigned char MS_opt, unsigned char init_com) {
  if(0 == MS_opt) {
    CSB_L; CSB2_H;
  } else if(1 == MS_opt) {
    CSB_H; CSB2_L;
  } else {
    CSB_L; CSB2_L;
  }
  
  delayMicroseconds(10); // 增加CS切换后的延时
  SPI4W_WriteCom(init_com);
  delayMicroseconds(10);
  CSB_H; CSB2_H;
  delayMicroseconds(10);
}

void MsDev_WriteData(unsigned char MS_opt, unsigned char init_data) {
  if(0 == MS_opt) {
    CSB_L; CSB2_H;
  } else if(1 == MS_opt) {
    CSB_H; CSB2_L;
  } else {
    CSB_L; CSB2_L;
  }
  
  delayMicroseconds(10); // 增加CS切换后的延时
  SPI4W_WriteData(init_data);
  delayMicroseconds(10);
  CSB_H; CSB2_H;
  delayMicroseconds(10);
}

unsigned char MsDev_ReadData(unsigned char MS_opt) {
  unsigned char temp = 0x00;
  
  if(0 == MS_opt) {
    CSB_L; CSB2_H;
  } else if(1 == MS_opt) {
    CSB_H; CSB2_L;
  } else {
    CSB_L; CSB2_L;
  }
  
  delayMicroseconds(10); // 增加CS切换后的延时
  temp = SPI4W_ReadData();
  delayMicroseconds(10);
  CSB_H; CSB2_H;
  delayMicroseconds(10);
  
  return temp;
}

unsigned char ReadTemptr(void) {
  unsigned char temptr_intgr = 0;
  unsigned char temptr_decml = 0;
  
  MsDev_WriteCom(MASTER_ONLY, 0x40);
  Delay_ms(100);
  ReadBusy();
  temptr_intgr = MsDev_ReadData(MASTER_ONLY);
  temptr_decml = MsDev_ReadData(MASTER_ONLY);
  
  Temptr_Cur = temptr_intgr;
  
  return temptr_intgr;
}

void WriteTemptr(unsigned char temptr_lock) {
  MsDev_WriteCom(MASTER_SLAVE, 0xE0);
  MsDev_WriteData(MASTER_SLAVE, 0x03);
  
  MsDev_WriteCom(MASTER_SLAVE, 0xE5);
  MsDev_WriteData(MASTER_SLAVE, temptr_lock);
  ReadBusy();
}

void Read_OTP_PWR(unsigned char temptr_opt) {
  unsigned long i, j;
  unsigned char OTP_VCOM;
  unsigned char temptr_val = 0;
  
  MS_H;
  Reset();
  MS_L;
  
  MsDev_WriteCom(MASTER_SLAVE, 0x00);
  MsDev_WriteData(MASTER_SLAVE, 0x0F);
  MsDev_WriteData(MASTER_SLAVE, 0x69);
  
  ReadTemptr();
  
  if(temptr_opt > 0 && temptr_opt != TEMPTR_ON) {
    temptr_val = temptr_opt;
  } else {
    temptr_val = Temptr_Cur;
  }
  
  MS_H;
  delayMicroseconds(10);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x00);
  MsDev_WriteData(MASTER_SLAVE, 0x0F);
  MsDev_WriteData(MASTER_SLAVE, 0x69);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x01);
  MsDev_WriteData(MASTER_SLAVE, 0x00);
  
  WriteTemptr(temptr_val);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x04);
  ReadBusy();
  Delay_ms(10);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x02);
  MsDev_WriteData(MASTER_SLAVE, 0x00);
  ReadBusy();
  Delay_ms(10);
  
  MS_L;
  delayMicroseconds(10);
  
  MsDev_WriteCom(MASTER_SLAVE, 0xF0);
  MsDev_ReadData(MASTER_ONLY);
  
  for(j = 0; j < 207; j++)
    MsDev_ReadData(MASTER_ONLY);
  
  OTP_VCOM = MsDev_ReadData(MASTER_ONLY);
  
  MsDev_WriteCom(MASTER_SLAVE, 0xF5);
  MsDev_WriteData(MASTER_SLAVE, 0xA5);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x94);
  MsDev_ReadData(MASTER_ONLY);
  
  for(i = 0; i < 5; i++) {
    OTP_PWR[i] = MsDev_ReadData(MASTER_ONLY);
  }
  
  MsDev_WriteCom(MASTER_SLAVE, 0xF5);
  MsDev_WriteData(MASTER_SLAVE, 0x00);
  
  MS_H;
  Reset();
  MS_L;

  MsDev_WriteCom(MASTER_SLAVE, 0x66);
  MsDev_WriteData(MASTER_SLAVE, 0x49);
  MsDev_WriteData(MASTER_SLAVE, 0x55);
  MsDev_WriteData(MASTER_SLAVE, 0x13);
  MsDev_WriteData(MASTER_SLAVE, 0x5D);
  MsDev_WriteData(MASTER_SLAVE, 0x05);
  MsDev_WriteData(MASTER_SLAVE, 0x10);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x13);
  MsDev_WriteData(MASTER_SLAVE, 0x00);
  MsDev_WriteData(MASTER_SLAVE, 0x00);
  
  MsDev_WriteCom(MASTER_SLAVE, 0xE0);
  MsDev_WriteData(MASTER_SLAVE, 0x01);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x00);
  MsDev_WriteData(MASTER_SLAVE, 0x13);
  MsDev_WriteData(MASTER_SLAVE, 0xE9);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x01);
  MsDev_WriteData(MASTER_SLAVE, 0x0F);
  MsDev_WriteData(MASTER_SLAVE, OTP_PWR[0]);
  MsDev_WriteData(MASTER_SLAVE, OTP_PWR[1]);
  MsDev_WriteData(MASTER_SLAVE, OTP_PWR[2]);
  MsDev_WriteData(MASTER_SLAVE, OTP_PWR[3]);
  MsDev_WriteData(MASTER_SLAVE, OTP_PWR[4]);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x06);
  MsDev_WriteData(MASTER_SLAVE, 0xD7);
  MsDev_WriteData(MASTER_SLAVE, 0xDE);
  MsDev_WriteData(MASTER_SLAVE, 0x12);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x61);
  MsDev_WriteData(MASTER_SLAVE, 0x00);
  MsDev_WriteData(MASTER_SLAVE, 0xC8);
  MsDev_WriteData(MASTER_SLAVE, 0x01);
  MsDev_WriteData(MASTER_SLAVE, 0x90);
  
  MsDev_WriteCom(MASTER_SLAVE, 0x82);
  MsDev_WriteData(MASTER_SLAVE, OTP_VCOM);
  
  MsDev_WriteCom(MASTER_SLAVE, 0xE3);
  MsDev_WriteData(MASTER_SLAVE, 0x01);
  
  MsDev_WriteCom(MASTER_SLAVE, 0xE9);
  MsDev_WriteData(MASTER_SLAVE, 0x01);
}

void EnterDeepsleep(void) {
  MsDev_WriteCom(MASTER_SLAVE, 0x07);
  MsDev_WriteData(MASTER_SLAVE, 0xA5);
  Serial.println("Entered deep sleep");
}

void Send_HV_Stripe_Data(void) {
  unsigned int col, row;
  
  Serial.println("Sending stripe data to MASTER");
  MsDev_WriteCom(MASTER_ONLY, 0x00);
  MsDev_WriteData(MASTER_ONLY, 0x13);
  MsDev_WriteData(MASTER_ONLY, 0xE9);
  
  MsDev_WriteCom(MASTER_ONLY, 0x10);
  Delay_ms(10);
  for(col = 0; col < 400; col++) {
    for(row = 0; row < 100; row++) {
      if(col >= 82 && col < 200 && row >= 10 && row <= 36) {
        MsDev_WriteData(MASTER_ONLY, WHITE);   // 白色
      } else if(col >= 82 && col < 200 && row > 36 && row <= 62) {
        MsDev_WriteData(MASTER_ONLY, YELLOW);  // 黄色
      } else if(col >= 82 && col < 200 && row > 62 && row <= 89) {
        MsDev_WriteData(MASTER_ONLY, GREEN);   // 绿色
      } else if(col >= 200 && col < 318 && row >= 10 && row <= 36) {
        MsDev_WriteData(MASTER_ONLY, BLACK);   // 黑色
      } else if(col >= 200 && col < 318 && row > 36 && row <= 62) {
        MsDev_WriteData(MASTER_ONLY, BLUE);    // 蓝色
      } else if(col >= 200 && col < 318 && row > 62 && row <= 89) {
        MsDev_WriteData(MASTER_ONLY, RED);     // 红色
      } else {
        MsDev_WriteData(MASTER_ONLY, WHITE);   // 白色
      }
    }
  }
  
  Serial.println("Sending stripe data to SLAVE");
  MsDev_WriteCom(SLAVE_ONLY, 0x00);
  MsDev_WriteData(SLAVE_ONLY, 0x17);
  MsDev_WriteData(SLAVE_ONLY, 0xE9);
  
  MsDev_WriteCom(SLAVE_ONLY, 0x10);
  Delay_ms(10);
  for(col = 0; col < 400; col++) {
    for(row = 0; row < 100; row++) {
      if(col >= 82 && col < 200 && row >= 10 && row <= 36) {
        MsDev_WriteData(SLAVE_ONLY, WHITE);    // 白色
      } else if(col >= 82 && col < 200 && row > 36 && row <= 62) {
        MsDev_WriteData(SLAVE_ONLY, YELLOW);   // 黄色
      } else if(col >= 82 && col < 200 && row > 62 && row <= 89) {
        MsDev_WriteData(SLAVE_ONLY, GREEN);    // 绿色
      } else if(col >= 200 && col < 318 && row >= 10 && row <= 36) {
        MsDev_WriteData(SLAVE_ONLY, BLACK);    // 黑色
      } else if(col >= 200 && col < 318 && row > 36 && row <= 62) {
        MsDev_WriteData(SLAVE_ONLY, BLUE);     // 蓝色
      } else if(col >= 200 && col < 318 && row > 62 && row <= 89) {
        MsDev_WriteData(SLAVE_ONLY, RED);      // 红色
      } else {
        MsDev_WriteData(SLAVE_ONLY, WHITE);    // 白色
      } 
    }
  }
  Serial.println("Stripe data sent");
}







// 发送 400x400 图片数据，主IC和从IC各负责400x100像素
void Send_HV_Stripe_imageData( const unsigned char *pic) {
  unsigned int col, row;
  unsigned char temp1, temp2, temp;

  // 发送数据到主IC（上半部分：行 0 到 99）
  Serial.println("Sending image data to MASTER (rows 0-99)");
  MsDev_WriteCom(MASTER_ONLY, 0x00);
  MsDev_WriteData(MASTER_ONLY, 0x13);
  MsDev_WriteData(MASTER_ONLY, 0xE9);
  
  MsDev_WriteCom(MASTER_ONLY, 0x10);
  Delay_ms(10);
  for(col = 0; col < 400; col++) {
    for(row = 0; row < 100; row++) {
      // 按列优先计算索引
      unsigned int index = col * 200 + row * 2;
      // 提取数据，模仿 Send_Color_Data_A
      temp1 = (pic[index] & 0xF0);          // 第一个字节的高4位
      temp2 = (pic[index + 1] >> 4);        // 第二个字节的高4位（右移4位）
      temp = temp1 | temp2;                    // 合并为一个字节
      MsDev_WriteData(MASTER_ONLY, temp);
      // ReadBusy(); // 可选：添加忙碌检查
    }
  }
  
  // 发送数据到从IC（下半部分：行 100 到 199）
  Serial.println("Sending image data to SLAVE (rows 100-199)");
  MsDev_WriteCom(SLAVE_ONLY, 0x00);
  MsDev_WriteData(SLAVE_ONLY, 0x17);
  MsDev_WriteData(SLAVE_ONLY, 0xE9);
  
  MsDev_WriteCom(SLAVE_ONLY, 0x10);
  Delay_ms(10);
  for(col = 0; col < 400; col++) {
    for(row = 0; row < 100; row++) {
      // 按列优先计算索引
      unsigned int index = col * 200 + row * 2;
      // 提取数据，模仿 Send_Color_Data_A
      temp1 = ((pic[index] & 0x0F) << 4);   // 第一个字节的低4位（左移4位）
      temp2 = (pic[index + 1] & 0x0F);      // 第二个字节的低4位
      temp = temp1 | temp2;                    // 合并为一个字节
      MsDev_WriteData(SLAVE_ONLY, temp);
      // ReadBusy(); // 可选：添加忙碌检查
    }
  }
  Serial.println("Image data sent");
}

//清屏
void Send_HV_Stripe_cleanData(void) {
  unsigned int col, row;
  
  Serial.println("Sending full white data to MASTER");
  MsDev_WriteCom(MASTER_ONLY, 0x00);
  MsDev_WriteData(MASTER_ONLY, 0x13);
  MsDev_WriteData(MASTER_ONLY, 0xE9);
  
  MsDev_WriteCom(MASTER_ONLY, 0x10);
  Delay_ms(10);
  for(col = 0; col < 400; col++) {
    for(row = 0; row < 100; row++) {
      MsDev_WriteData(MASTER_ONLY, WHITE);   // 白色
    }
  }
  
  Serial.println("Sending full white data to SLAVE");
  MsDev_WriteCom(SLAVE_ONLY, 0x00);
  MsDev_WriteData(SLAVE_ONLY, 0x17);
  MsDev_WriteData(SLAVE_ONLY, 0xE9);
  
  MsDev_WriteCom(SLAVE_ONLY, 0x10);
  Delay_ms(10);
  for(col = 0; col < 400; col++) {
    for(row = 0; row < 100; row++) {
      MsDev_WriteData(SLAVE_ONLY, WHITE);    // 白色
    }
  }
  Serial.println("Full white data sent");
}

//yuanma 
 


void EPD_Display(unsigned char display_bkg) {
  Serial.println("Starting EPD display");
  if(display_bkg == STRIPE) {
    Send_HV_Stripe_Data();
  }
  
  Serial.println("Sending power on command");
  MsDev_WriteCom(MASTER_SLAVE, 0x04);
  Serial.println("升压");
  ReadBusy();
  
  Serial.println("Sending refresh command");
  MsDev_WriteCom(MASTER_SLAVE, 0x12);
  MsDev_WriteData(MASTER_SLAVE, 0x00);
  Delay_ms(10);
  ReadBusy();
  
  Serial.println("Sending power off command");
  MsDev_WriteCom(MASTER_SLAVE, 0x02);
  MsDev_WriteData(MASTER_SLAVE, 0x00);
  ReadBusy();
  Delay_ms(20);
  Serial.println("EPD display completed");
}


void Send_HV_Stripe_redData(void) {
  unsigned int col, row;
  
  Serial.println("Sending full red data to MASTER");
  MsDev_WriteCom(MASTER_ONLY, 0x00);
  MsDev_WriteData(MASTER_ONLY, 0x13);
  MsDev_WriteData(MASTER_ONLY, 0xE9);
  
  MsDev_WriteCom(MASTER_ONLY, 0x10);
  Delay_ms(10);
  for(col = 0; col < 400; col++) {
    for(row = 0; row < 100; row++) {
      MsDev_WriteData(MASTER_ONLY, RED);   // 红色
    }
  }
  
  Serial.println("Sending full red data to SLAVE");
  MsDev_WriteCom(SLAVE_ONLY, 0x00);
  MsDev_WriteData(SLAVE_ONLY, 0x17);
  MsDev_WriteData(SLAVE_ONLY, 0xE9);
  
  MsDev_WriteCom(SLAVE_ONLY, 0x10);
  Delay_ms(10);
  for(col = 0; col < 400; col++) {
    for(row = 0; row < 100; row++) {
      MsDev_WriteData(SLAVE_ONLY, RED);    // 红色
    }
  }
  Serial.println("Full red data sent");
}



void setup() {
  Serial.begin(115200);

  while (!Serial); // 等待串口连接
  Serial.println("ESP32S3 EPD Color Bar Test Start");
  
  // turn on power to display
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);   // turn the Display on (HIGH is the voltage level)
  Serial.println("Display power ON");
  delay(500);   

  Sys_init();
  
  // 测试 SPI 通信：读取 IC 温度
  Serial.println("Testing SPI communication");
  Reset();
  ReadBusy();
  unsigned char temp = ReadTemptr();
  Serial.print("IC Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  
  // 如果温度值合理（20-30°C），SPI 通信正常；否则检查硬件连接
  if(temp < 10 || temp > 50) {
    Serial.println("SPI communication may have issues. Check connections!");
    // 不要停止，继续尝试显示
  }
  
  // 初始化并显示彩条
  Serial.println("Starting color bar display");
  Reset();
  Serial.println("1");
  ReadBusy();
  Serial.println("2");
  Read_OTP_PWR(TEMPTR_ON);
  Serial.println("3"); 
  Send_HV_Stripe_Data();  
  EPD_Display(PIC_A); 
  Serial.println("4"); 
  EnterDeepsleep();

 Delay_S(3);

  Read_OTP_PWR(TEMPTR_ON);
  Serial.println("3"); 
 Send_HV_Stripe_imageData(gImage1);  
  EPD_Display(PIC_A); 
  Serial.println("4"); 
  EnterDeepsleep();

 Delay_S(3);

  Read_OTP_PWR(TEMPTR_ON);
  Serial.println("3"); 
 Send_HV_Stripe_imageData(gImage2);  
  EPD_Display(PIC_A); 
  Serial.println("4"); 
  EnterDeepsleep();

 Delay_S(3);
  Read_OTP_PWR(TEMPTR_ON);
  Serial.println("3"); 
  Send_HV_Stripe_cleanData();  
  EPD_Display(PIC_A); 
  Serial.println("4"); 
  EnterDeepsleep();


  
}

void loop() {
  // 主循环不执行任何操作，只显示一次
 // Delay_S(60); // 长时间延迟
}