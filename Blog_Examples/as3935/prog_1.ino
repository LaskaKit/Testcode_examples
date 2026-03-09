/*
 * Vzorovy kod pro pouziti detektoru blesku s ESP32-LPkit
 * Pripojeni pres I2C sbernici
 *
 * Zapojeni
 * A1 - 3.3V
 * A0 - 3.3V
 * EN_V - Nezapojen
 * IRQ - GPIO4
 * SI - 3.3V // GND = SPI; 3.3V = I2C
 * CS - GND
 * MISO - GND
 * MOSI - GPIO21
 * SCL - GPIO22
 * GND - GND
 * VCC - 3.3V
 *
 * Potrebne knihovny
 * https://github.com/sparkfun/SparkFun_AS3935_Lightning_Detector_Arduino_Library
 */

#include <Wire.h>
#include "SparkFun_AS3935.h"

#define AS3935_ADDR 0x03
#define INDOOR 0x12
#define OUTDOOR 0x0E
#define LIGHTNING_INT 0x08
#define DISTURBER_INT 0x04
#define NOISE_INT 0x01

SparkFun_AS3935 lightning(AS3935_ADDR);

const int pinPreruseni = 4; // Interrupt pin
int hodnota = 0; // prectena hodnota

void setup() {
    // Interrupt pin pro detekci, HIGH = aktivni preruseni
    pinMode(pinPreruseni, INPUT);

    Serial.begin(115200);
    Serial.println("AS3935 Inicializace");

    Wire.begin(); // I2C inicializace

    // Inicialiazce detektoru blesků
    if( !lightning.begin() )
    {
        Serial.println ("Problem s inicializaci. Zkontrolujte propojeni.");
        while(1);
    }
    else
        Serial.println("Detektor blesku pripraven");

    // Odkomentujte v pripade, ze chcete pouzit detektor venku
    //lightning.setIndoorOutdoor(OUTDOOR);
}

void loop() {
    if(digitalRead(pinPreruseni) == HIGH) // Preruseni - blesk, sum nebo ruseni detekovano
    {
        hodnota = lightning.readInterruptReg(); // kontrola, co aktivovalo interrupt

        if(hodnota == NOISE_INT){
            Serial.println("Detekovan sum!");
            // Pokud preruseni casto způsobuje sum, odkomentujte radek nize
            //lightning.setNoiseLevel(Hodnota); // hodnota od 1 do 7
        }
        else if(hodnota == DISTURBER_INT){
            Serial.println("Detekovano ruseni!");
            // Pokud prerusni casto zpusobuje ruseni, odkomentujte radek nize
            //lightning.watchdogThreshold(Hodnota);  // hodnota od 1 do 10

            // Pokud vidite casto ruseni, muzete povolit dalsi filtraci
            //lightning.maskDisturber(true); // hodnota true nebo false
        }
        else if(hodnota == LIGHTNING_INT){
            Serial.println("Detekovan BLESK!");

            // Cteni odhahodave vzdalenosti blesku
            byte vzdalenostBlesku = lightning.distanceToStorm();
            Serial.print("Priblizne: ");
            Serial.print(vzdalenostBlesku);
            Serial.println("km daleko!");

            // Cteni odhadovane energie blesku
            long energieBlesku = lightning.lightningEnergy();
            Serial.print("Odhadovana energie blesku: ");
            Serial.println(energieBlesku);
        }
    }
    delay(100);
}
