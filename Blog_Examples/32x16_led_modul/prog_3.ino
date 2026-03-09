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

    myled.showmsg_single_scroll("Test beziciho textu", 2, 3, 0);
    //myled.showmsg_double_scroll("Test beziciho textu na 1. radku", "Test beziciho textu na 2. radku", 10, INF, 3, 1, 0);
    delay(1000);
}
void loop()
{

}
