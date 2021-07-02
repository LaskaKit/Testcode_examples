/*
 Example testing sketch for BMP180 temperature and air presure sensor
 REQUIRES the following Arduino libraries:
    - Adafruit_BMP085: https://github.com/adafruit/Adafruit-BMP085-Library

  BMP180 sensor attached with I2C bus as follows:
  * Arduino Uno:
    ** SDA - 4A SCL - 5A
  * LaskaKit ESP-12 Board
    ** SDA - 05 SDA - 04 
  * Wemos D1 mini 
    ** SCL - D1 SDA - D2
  * ESP-01 (Shield)
    ** SDA - 05 SDA - 04
 */

#include <Adafruit_BMP085.h>
#include <Wire.h>
//Addres 0x77
Adafruit_BMP085 bmp;
 
void setup() 
{
  Serial.begin(9600);
  //
  if (!bmp.begin()) 
  {
    Serial.println("Could not find BMP180 sensor");
    while (1) {}
  }
}
 
void loop() 
{
  Serial.print("Temprerature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" °C");
  
  Serial.print("Presure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
 
 
  Serial.println();
  delay(5000);
}
