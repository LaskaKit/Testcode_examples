/*
 * Test code for MH-Z14 with ESP32-LPkit
 * Serial2 is used as interface for CO2 sensor
 * RX - GPIO16
 * TX - GPIO17
 * 
 * 2021 made by laskarduino
 * 
 * Libraries:
 * -
 */

#include <WiFi.h>

#define RXD2 16 // RX of Serial2
#define TXD2 17 // TX of Serial2

byte addArray[] = {
0xFF, 0x01, 0x86,
0x00, 0x00, 0x00,
0x00, 0x00, 0x79
};

char dataValue[9];
String dataString = "";

void setup() {
  Serial.begin(115200); // initialization of Serial;
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Set the baudrate for Serial2 for MH-Z14

  delay(500);
  Serial.println("Test code for MH-Z14");

}

void loop() {
  int co2 = 0;
  
  Serial2.write(addArray, 9);  
  Serial2.readBytes(dataValue, 9);  // read Values from MH-Z14
  co2 = (256*(int)dataValue[2])+(int)dataValue[3];  // Formula for CO2 divided into 2 bytes

  Serial.print("CO2 value: "); Serial.print(co2); Serial.println(" ppm");
  delay(2000);
}
