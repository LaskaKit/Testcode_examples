/*
 * MOSFET prakticky
 * Spínání LED přes tranzistor připojený k výstupu 3
 *
 * laskarduino.cz
 * (c) 2021
 */

void setup() {
    pinMode(3, OUTPUT); // výstup 3

}

void loop() {
    digitalWrite(3, HIGH); // zapni LED
    delay(1000);
    digitalWrite(3, LOW); // vypni LED
    delay(1000);
}
