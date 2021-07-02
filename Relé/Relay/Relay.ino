/*
 Example testing sketch for Relay shield

  Relay is attached as follows:
  * Arduino Uno:
    **
  * LaskaKit ESP-12 Board
    ** 05 
  * Wemos D1 mini 
    ** D1
  * ESP-01 (Shield)
    ** 05
 */

const int relayPin = D1;

void setup() {
  pinMode(relayPin, OUTPUT);
}

void loop() {
  digitalWrite(relayPin, HIGH); // turn on relay with voltage HIGH
  delay(1000);              // pause
  digitalWrite(relayPin, LOW);  // turn off relay with voltage LOW
  delay(1000);            // pause
}
