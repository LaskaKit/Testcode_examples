/*
 * MOSFET prakticky
 * Ovládání LED přes tranzistor připojený k výstupu 3
 *
 * laskarduino.cz
 * (c) 2021
 */

void setup() {
    pinMode(3, OUTPUT); // výstup D3

}

void loop() {
    for(int i=0; i<255; i++)
    {
        analogWrite(3, i);
        delay(10);
    }
    for(int i=255; i>0; i--)
    {
        analogWrite(3, i);
        delay(10);
    }
}
