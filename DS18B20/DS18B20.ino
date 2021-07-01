/*
 Example testing sketch for DS18B20 temperature and humidity sensor
 REQUIRES the following Arduino libraries:
    - OneWire https://github.com/PaulStoffregen/OneWire
    - DallasTemperature https://github.com/milesburton/Arduino-Temperature-Control-Library

  SHT30 sensor attached  as follows:
  * Arduino Uno:
    ** 
  * LaskaKit ESP-12 Board
    ** 04 
  * Wemos D1 mini 
    ** D2
  * ESP-01 (Shield)
    ** 04
 */


#include <OneWire.h>
#include <DallasTemperature.h>
//Sensors pin D2
const int oneWireBus = 4;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  //starts up the sensor
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures(); 
  Serial.print("Temperature in celsius: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.println("ºC");

  Serial.print("Temperature in fahrenheit: ");
  Serial.print(sensors.getTempFByIndex(0));
  Serial.println("ºF");
  delay(5000);
}
