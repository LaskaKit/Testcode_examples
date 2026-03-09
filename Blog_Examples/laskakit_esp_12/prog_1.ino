/*
 * Blikani vestavenou LED na ESP-12 (GPIO2) a posilani zprav na seriovou konzoli
 *
 * https://github.com/LasKKit/ESP12-Board
 * laskarduino.cz
 * 2021
 *
 * Kompilujte jako "Generic ESP8266 Module"
 */

void setup() {
    Serial.begin(115200); // inicializace Serial, 115200 Bd
    pinMode(2, OUTPUT); // inicializace LED na ESP-12
}

void loop() {
    Serial.println("Blikani LED");
    digitalWrite(2, LOW);
    delay(500);
    digitalWrite(2, HIGH);
    delay(500);
}
