#include <HardwareSerial.h>

HardwareSerial HW_Serial(0);

void setup() {

    HW_Serial.begin(115200, SERIAL_8N1, 16, 17); // 115200 – rychlost; SERIAL_8N1 – typ paketu; 16 je číslo pinu pro RX a 17 je číslo pinu pro TX.

}

void loop() {

    HW_Serial.println(„test“);

    delay(1000);

}
