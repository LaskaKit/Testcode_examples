#include <Adafruit_NeoPixel.h>
#include <IRremote2.h>
#include <Arduino.h>
#include <EEPROM.h>
#define pinDIN 6
#define pocetLED 6
// inicializace LED modulu z knihovny
Adafruit_NeoPixel rgbWS = Adafruit_NeoPixel(pocetLED, pinDIN, NEO_GRB + NEO_KHZ800);  // datovy pin 6

// Zacatek projektu osvetleni vitrina 20-09-2023
// Skutecne naklady za jednotlive komponenty
// Arduino nano USB-C                          239 Kc
// Arduino terminal shield                     62 Kc
// Redukce pro napajeni z kulateho konektoru   29 Kc
// Zdroj 230V  9V 1A                           107 Kc
// Diody RGB  WS2812   6ks   celk. cena        66 Kc
// Propojovaci vodice                     cca. 10 Kc
// IR prijimac                                 59 Kc
// Krabicke pro arduino  -                     zdarma
// VSECHNY CENY JSOU VCETNE DPH
// Celkova cena bez prace je                   572 Kc

// verze SW25092023
// poradi barev je Green, Red, Blue  - tj. ZELENA, CERVENA, MODRA
// pri omezeni na max svit 250  je max. proud do vsech LEDek  200 mA
// IR prijimac je zapojen na D11

void setup() {
    // zahájení komunikace s LED modulem
    rgbWS.begin();
    byte zelena;
    byte cervena;
    byte modra;
    byte ledka;
    byte program=0;  // pokud je 0 vsechny ledky jsou na 0
    int RECV_PIN = 11;
    Serial.begin(9600);
    EEPROM.get(2, zelena);
    EEPROM.get(3, cervena);
    EEPROM.get(4, modra);

    if (program<0 or program>2) {program=1;};   //po prvnim spusteni stiskni OFF, zakomentuj tento blok a znova nahraj
    if (zelena<0 or zelena>254) {zelena=0;};
    if (cervena<0 or cervena>254) {cervena=0;};
    if (modra<0 or modra>254) {modra=0;};

    // program 0 - vsechny ledky jsou zhasnuty
    // program 1 - vsechny LEDky sviti dle nastavene barvy
    // program 2 - tlac.24 - vsechny LEDky pomalu meni samovolne barvy, plynuly prechod RED-GREEN-BLUE
    // program 3 - tlac.12 - postupne se rozsvecuji jednotlive LEDky vzdy s jinou barvou

    IRrecv irrecv(RECV_PIN);
    irrecv.enableIRIn();
    decode_results results;
    delay(1000);rgbWS.clear();rgbWS.show(); delay(1000);
};

void loop()
{
    int RECV_PIN = 11;
    byte zelena;
    byte cervena;
    byte modra;
    byte ledka;
    byte program;

    IRrecv irrecv(RECV_PIN);
    decode_results results;

    if (program==0) {nastavRGB(0,0,0,1); nastavRGB(0,0,0,2); nastavRGB(0,0,20,3);
        nastavRGB(0,0,0,4); nastavRGB(0,0,0,5); nastavRGB(0,0,0,6); delay(1000);
    };   // pri vypnuti zustane slabe svitit modre dioda c.3 - je to tak schvalne

    if (irrecv.decode(&results)) {
        if (results.value == 16752735 and program==1) {
            if (zelena<247 and zelena>3) {zelena=zelena+5;};
            if (cervena<247 and cervena>3) {cervena=cervena+5;};
            if (modra<247 and modra>3) {modra=modra+5;};for (byte ledka=1; ledka<7; ledka++){
                nastavRGB( zelena,cervena, modra, ledka);};
        };   // tlacitko zvyseni intenzity
        if (results.value == 4294967295) {irrecv.resume(); };
        if (results.value == 16720095 and program==1) {
            if (zelena>5){zelena=zelena-5;};
            if (cervena>5){cervena=cervena-5;};
            if (modra>5){modra=modra-5;}; for (byte ledka=1; ledka<7; ledka++){
                nastavRGB( zelena,cervena, modra, ledka);};
        };  // tlacitko snizeni intenzity
        if (results.value == 16736415) {
            EEPROM.put(1, program);EEPROM.put(2,zelena);EEPROM.put(3,cervena);EEPROM.put(4,modra);program=0;delay(200);
        };   // tlacitko vypnuti
        if (results.value == 16769055 and program==0) {
            EEPROM.get(1, program); EEPROM.get(2, zelena); EEPROM.get(3, cervena);
            EEPROM.get(4, modra);for (byte ledka=1; ledka<7; ledka++){
                nastavRGB( zelena,cervena, modra, ledka);};program=1;};   // tlacitko zapnuti
                if (results.value == 16748655) {
                    program=1; zelena=0; cervena=252; modra=0;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko sviti pouze cervena
                if (results.value == 16716015) {
                    program=1; zelena=252; cervena=0; modra=0;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko sviti pouze zelena

                if (results.value == 16732335) {
                    program=1; zelena=0; cervena=0; modra=252;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko sviti pouze modra

                if (results.value == 16764975) {
                    program=1; zelena=252; cervena=252; modra=252;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko sviti bila
                if (results.value == 16756815) {
                    program=1; zelena=63; cervena=252; modra=0;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.9
                if (results.value == 16724175) {
                    program=1; zelena=252; cervena=0; modra=63;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.10
                if (results.value == 16740495) {
                    program=1; zelena=0; cervena=63; modra=252;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.11

                if (results.value == 16754775) {
                    program=1; zelena=126; cervena=252; modra=0;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.13

                if (results.value == 16722135) {
                    program=1; zelena=252; cervena=0; modra=252;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.14

                if (results.value == 16738455) {
                    program=1; zelena=0; cervena=126; modra=126;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.15
                if (results.value == 16750695) {
                    program=1; zelena=190; cervena=253; modra=0;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.17
                if (results.value == 16718055) {
                    program=1; zelena=126; cervena=0; modra=126;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.18
                if (results.value == 16734375) {
                    program=1; zelena=0; cervena=190; modra=126;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.19
                if (results.value == 16746615) {
                    program=1; zelena=253; cervena=253; modra=0;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.21
                if (results.value == 16713975) {
                    program=1; zelena=80; cervena=0; modra=80;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.22
                if (results.value == 16730295) {
                    program=1; zelena=0; cervena=253; modra=253;
                    for (byte ledka=1; ledka<7; ledka++){
                        nastavRGB( zelena,cervena, modra, ledka);};
                };  // tlacitko c.23
                if (results.value == 16762935) {program=2; };    // tlacitko c.24
                if (results.value == 4294967295) {irrecv.resume(); };
                delay(10);
                irrecv.resume(); };

                if (program==2) {
                    zelena=0; cervena=0; modra=0;
                    IRrecv irrecv(RECV_PIN);
                    decode_results results;irrecv.resume();
                    for (int sila=0; sila<254; sila=sila+1) {zelena=253-sila; cervena=sila; modra=0; delay(40);
                        for (byte ledka=1; ledka<7; ledka++){
                            nastavRGB( zelena,cervena, modra, ledka);};    // prechod zelena-cervena
                            if (irrecv.decode(&results)) {program=1; sila=254; delay(10); if (results.value == 16736415) {
                                EEPROM.put(1, program);EEPROM.put(2,zelena);EEPROM.put(3,cervena);EEPROM.put(4,modra);program=0;delay(200);
                            };
                            }; };};

                            if (program==2) { irrecv.resume();delay(50);
                                zelena=0; cervena=0; modra=0;
                                for (int sila=0; sila<254; sila=sila+1) {zelena=0; cervena=253-sila; modra=sila; delay(40);
                                    for (byte ledka=1; ledka<7; ledka++){
                                        nastavRGB( zelena,cervena, modra, ledka);};    // prechod cervena-modra
                                        if (irrecv.decode(&results)) {program=1; sila=254; delay(10);
                                            if (results.value == 16736415) {
                                                EEPROM.put(1, program);EEPROM.put(2,zelena);EEPROM.put(3,cervena);EEPROM.put(4,modra);program=0;delay(200);
                                            };
                                        }; }; };

                                        if (program==2) { irrecv.resume();delay(50);
                                            zelena=0; cervena=0; modra=0;
                                            for (int sila=0; sila<254; sila=sila+1) {zelena=sila; cervena=0; modra=253-sila; delay(40);
                                                for (byte ledka=1; ledka<7; ledka++){
                                                    nastavRGB( zelena,cervena, modra, ledka);};     // prechod modra-zelena
                                                    if (irrecv.decode(&results)) {program=1; sila=254; delay(10);if (results.value == 16736415) {
                                                        EEPROM.put(1, program);EEPROM.put(2,zelena);EEPROM.put(3,cervena);EEPROM.put(4,modra);program=0;delay(200);
                                                    };
                                                    }; }; };
                                                    delay(40);
}

// poradi barev je Green, Red, Blue  - tj. ZELENA, CERVENA, MODRA
// funkce pro nastavení zadané barvy na zvolenou diodu
void nastavRGB (byte r, byte g, byte b, int cislo) {
    // vytvoření proměnné pro ukládání barev
    uint32_t barva;
    // načtení barvy do proměnné
    barva = rgbWS.Color(r, g, b);
    // nastavení barvy pro danou LED diodu,
    // číslo má pořadí od nuly
    rgbWS.setPixelColor(cislo - 1, barva);
    // aktualizace barev na všech modulech
    rgbWS.show();
}
