/*
* Example code for detector of lighting AS3935 with ESP32-LPkit evaluation board
* Connected through I2C bus
* 
* Connection
* A1 - 3.3V
* A0 - 3.3V
* EN_V - Nezapojen
* IRQ - GPIO4
* SI - 3.3V // GND = SPI; 3.3V = I2C
* CS - GND
* MISO - GND
* MOSI - GPIO21
* SCL - GPIO22
* GND - GND
* VCC - 3.3V 
*
* Libraries
* https://github.com/sparkfun/SparkFun_AS3935_Lightning_Detector_Arduino_Library
*
* Made by laskarduino.cz (2021)
*/
 
#include <Wire.h>
#include "SparkFun_AS3935.h"
 
#define AS3935_ADDR 0x03
#define INDOOR 0x12 
#define OUTDOOR 0x0E
#define LIGHTNING_INT 0x08
#define DISTURBER_INT 0x04
#define NOISE_INT 0x01
 
SparkFun_AS3935 lightning(AS3935_ADDR);
 
const int intPin = 4; // Interrupt pin
int intValue = 0; // value
 
void setup() {
  // Interrupt pin for detection, HIGH = active interrupt
  pinMode(intPinv, INPUT); 
 
  Serial.begin(115200); 
  Serial.println("AS3935 initialization"); 
 
  Wire.begin(); // I2C initialization
 
  // Initialization of sensor
  if( !lightning.begin() ) 
  { 
    Serial.println ("Problem with an initialization, check the connection"); 
    while(1); 
  }
  else
    Serial.println("Lightning sensor ready");
 
  // Uncomment the line below in case, you want to use a sensor outdoor
  //lightning.setIndoorOutdoor(OUTDOOR);
}
 
void loop() {
  if(digitalRead(intPin) == HIGH) // Interrupt active - lightning, noise, disturbing
  {
    intValue = lightning.readInterruptReg(); // check what caused the interrupt
    
    if(intValue == NOISE_INT){
      Serial.println("Noise was detected!"); 
      // If you see the noise very often, uncomment the line below
      //lightning.setNoiseLevel(Value); // value from 1 to 7
    }
    else if(intValue == DISTURBER_INT){
      Serial.println("Disturing was detected!"); 
      // If you see the disturbing very often, uncomment the line below
      //lightning.watchdogThreshold(Value);  // Value from 1 to 10
 
      // If you see the disturing very often, uncomment the line below and improve the filtering
      //lightning.maskDisturber(true); // hodnota true nebo false
    }
    else if(intValue == LIGHTNING_INT){
      Serial.println("LIGHTNING was detected!"); 
 
      // Read the approx distance of lightning
      byte lightningDistance = lightning.distanceToStorm(); 
      Serial.print("Approximately: "); 
      Serial.print(lightningDistance); 
      Serial.println("km far!"); 
 
      // Read the approx energy of lightning
      long lightningEnergy = lightning.lightningEnergy(); 
      Serial.print("Approximate the energy of lightning: "); 
      Serial.println(lightningEnergy); 
    }
  }
  delay(100);
}
