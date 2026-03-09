#include "TimerOne.h"
#include "SPI.h"
#include "LedP10.h"

LedP10 myled;

void setup()
{
    myled.init(3,4,8,9,1);
    /*
     *   P10 Pin A                - Arduino Pin 3
     *   P10 Pin B                - Arduino Pin 4
     *   P10 Store Pin            - Arduino Pin 8
     *   P10 OE Pin               - Arduino Pin 9
     *   Pocet propojenych panelu - 1 (Muze byt vice jak 10)
     */
}
void loop()
{
    int analogValue1 = analogRead(A0);
    int analogValue2 = analogRead(A1);
    myled.showmsg_single_static(analogValue1, 0);
    //myled.showmsg_double_static(analogValue1, analogValue2, 0);
    delay(500);
}
