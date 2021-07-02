/*
 Example testing sketch for SHT30 temperature and humidity sensor
 REQUIRES the following Arduino libraries:
    - WEMOS_SHT3X: https://github.com/wemos/WEMOS_SHT3x_Arduino_Library

  SHT30 sensor attached with I2C bus as follows:
  * Arduino Uno:
    ** SDA - 4A SCL - 5A
  * LaskaKit ESP-12 Board
    ** SDA - 05 SDA - 04 
  * Wemos D1 mini 
    ** SCL - D1 SDA - D2
  * ESP-01 (Shield)
    ** SDA - 05 SDA - 04
 */
#include <WEMOS_SHT3X.h>
//addres 0x45 or 0x44 
SHT3X sht30(0x45);
 
void setup() 
{
  Serial.begin(9600);
}
 
void loop() {
 
  if(sht30.get()==0){
    Serial.print("Temperature in Celsius : ");
    Serial.print(sht30.cTemp);
    Serial.println("°C");
    
    Serial.print("Temperature in Fahrenheit : ");
    Serial.print(sht30.fTemp);
    Serial.println("°F");
    
    Serial.print("Relative Humidity : ");
    Serial.print(sht30.humidity);
    Serial.println("%");
  }
  else
  {
    Serial.println("Error!");
  }
  delay(1000); 
}
