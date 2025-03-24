// ESP32 I2C Scanner

#include <Wire.h>

//#define PIN_ON 47   //47 ESP32-S3 DevKit
#define PIN_ON 2    //2 ESP32 DevKit

void setup(){
  pinMode(PIN_ON, OUTPUT);
  digitalWrite(PIN_ON, HIGH);
  Serial.begin (115200);  
  Wire.begin(21, 22);   // ESP32 = sda= GPIO_21 scl= GPIO_22
  //Wire.begin(19, 18);  // ESP-C3 = sda= GPIO_19 scl= GPIO_18
  //Wire.begin(42, 2);  //ESP32-S3
  //Wire.begin(8, 10);
  //Wire.begin(33, 32);  //ESPLan
  //Wire.setClock(10000L);
}

void Scanner(){
  Serial.println();
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println(")");
      count++;
    }
  }
  Serial.print("Found ");      
  Serial.print(count, DEC);        // numbers of devices
  Serial.println(" device(s).");
}

void loop(){
  Scanner();
  delay(1000);
}
