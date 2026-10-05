#include <Arduino.h>
#include <esp_heap_caps.h>
#include <esp_sleep.h>

#include "EL100UF1.h"
// #include "bms.h"
#include"image.h"
void setup() {
  uint8_t * image_buf;

  Serial.begin(115200);
  delay(2000);
  
  //No Image Update. Just test T2001 can be booted up or not. For T2001 FW/WBF burning...
  Serial.println("EL100UF1_Init");
  epd.EL100UF1_Init();
  epd.EL100UF1_DeepSleep();
  Serial.println("EL100UF1_Deinit");
  epd.EL100UF1_Deinit();
  
  delay(2000);
  // bms_init();

  frame_buffer = (uint8_t *)heap_caps_malloc(EPD_FRAME_SIZE, MALLOC_CAP_SPIRAM);
  if(frame_buffer == NULL)
    Serial.println("CDC_CMD_PARSER: dst_image_buffer Allocate Failed!");

  memcpy(frame_buffer, image, EPD_FRAME_SIZE); // copy image data
  
  Serial.println("ESP32S3 + IT8591 + EL100UF1 Demo Start.");
  Serial.println("EL100UF1_Init");
  epd.EL100UF1_Init();
  Serial.println("EL100UF1_Flip");
  epd.EL100UF1_Flip(frame_buffer);
  Serial.println("EL100UF1_SendPicData");
  epd.EL100UF1_SendPicData(frame_buffer, EPD_FRAME_SIZE);
  Serial.println("EL100UF1_Update");
  epd.EL100UF1_Update();
  delay(1000);
  Serial.println("EL100UF1_DeepSleep");
  epd.EL100UF1_DeepSleep();
  delay(2000);
  Serial.println("EL100UF1_Deinit");
  epd.EL100UF1_Deinit();

  delay(1000);

  frame_buffer = (uint8_t *)heap_caps_malloc(EPD_FRAME_SIZE, MALLOC_CAP_SPIRAM);
  if(frame_buffer == NULL)
    Serial.println("CDC_CMD_PARSER: dst_image_buffer Allocate Failed!");

  memset(frame_buffer, 0xF8, EPD_FRAME_SIZE); //White
  
  Serial.println("ESP32S3 + IT8591 + EL100UF1 Demo Start.");
  Serial.println("EL100UF1_Init");
  epd.EL100UF1_Init();
  Serial.println("EL100UF1_Flip");
  epd.EL100UF1_Flip(frame_buffer);
  Serial.println("EL100UF1_SendPicData");
  epd.EL100UF1_SendPicData(frame_buffer, EPD_FRAME_SIZE);
  Serial.println("EL100UF1_Update");
  epd.EL100UF1_Update();
  Serial.println("EL100UF1_DeepSleep");
  epd.EL100UF1_DeepSleep();
  Serial.println("EL100UF1_Deinit");
  epd.EL100UF1_Deinit();

  delay(1000);


}

void loop() {
  // bms_loop();
  delay(1000);
}
