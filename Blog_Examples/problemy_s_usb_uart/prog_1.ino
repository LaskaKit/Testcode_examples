#include <HardwareSerial.h>
HardwareSerial HW_Serial1(1);
HW_Serial1.begin(115200, SERIAL_8N1, XX, YY); // 115200 – rychlost; SERIAL_8N1 – typ paketu; XX je číslo pinu pro RX a YY je číslo pinu pro TX – katalogový list výrobce.
HW_Serial1.println(„HW UART“); // odeslání dat přes HW UART
