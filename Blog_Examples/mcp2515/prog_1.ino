// demo: MCP2515 CAN-BUS , čtení informací
#include <mcp_can.h>
#include <SPI.h>
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
MCP_CAN CAN0(10);                               // nastavit CS k pinu 10
void setup()
{
    Serial.begin(115200);
    // init can bus : baudrate = 500k
    if(CAN0.begin(CAN_500KBPS, MCP_8MHz) == CAN_OK) Serial.print("can init ok!!\r\n");
    else Serial.print("Can init fail!!\r\n");

    pinMode(2, INPUT);                            // nastavení pinu 2 pro /INT vstup
    Serial.println("MCP2515 Library Receive Example...");
}
void loop()
{
    if(!digitalRead(2))                         // Jestli pin 2 je low, čtení informací z bufferu
    {
        CAN0.readMsgBuf(&len, rxBuf);              // čtení informací: len = délka dat, buf = data byte(s)
        rxId = CAN0.getCanId();                    // čtení ID
        Serial.print("ID: ");
        Serial.print(rxId, HEX);
        Serial.print("  Data: ");
        for(int i = 0; i<len; i++)                // Tisk každého bytu dat
        {
            if(rxBuf[i] < 0x10)                     // Jestli byte dat míň než 0x10, přidat nulu před
            {
                Serial.print("0");
            }
            Serial.print(rxBuf[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}
