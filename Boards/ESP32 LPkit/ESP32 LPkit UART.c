/*
* Example code for ESP32-LPkit evaluation board made by laskarduino.cz
* Firmware sends message through serial com port (UART)
* "laskarduino.cz" 
* "ESP32 LP-kit"
* The message is sent every second
*
* Created by (c) laskarduino.cz 2021
*
* Libraries:
* -
*/
 
void setup() {
  Serial.begin(115200);
  while (!Serial) 
  {
    ; // waiting for serial com port
  }
}
 
void loop() {
  Serial.println("laskarduino.cz");
  Serial.println("ESP32-LPkit");
  delay(1000);
}
