#include <DS3231.h>
#include <Wire.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <pitches.h>

#define CLK 2
#define DIO 3

DS3231 clock;
RTCDateTime dt;

// Funguje s Arduino Nano Old Bootloader
// Cas funguje, alarm se nevymaze, reproduktory hraji, senzorove tlacitko funguje
// zapojeni RTC  SDA-A4, SCL-A5
// zapojeni TM1637  CLK-D2, DLO-D3
// volne piny D9, D10, D11
// D12 - senzorove tlacitko k zobrazeni casu alarmu

byte jas = 7; // pres den je 7  vecer a v noci 1
int jeAlarm = 0;
int casalarmuhod = 18;
int casalarmumin = 18;
byte budetobudit = 0;  // pokud ano 1, pokud ne 0
int pot1Value = 500;  // pin A0
int pot2Value = 500;  // pin A1
int pot3Value = 500;  // pin A2
byte funkce = 0;
byte pin8 = 1;
byte pin12 = 0;
byte c1 = 1;
byte c2 = 2;
byte c3 = 5;
byte c4 = 9;
byte hodina = 23;
byte minuta = 53;
byte sekunda = 50;
int rok = 2000;
byte denvtydnu = 1;
byte den = 1;
byte mesic = 1;
byte a1hodina = 1;
byte a1minuta = 1;
byte a2hodina = 0;
byte a2minuta = 0;
int teplota = 10;
byte zap = 0;
byte vyp = 0; // pomocna promenna
byte zap1 = 0;
byte vyp1 = 0;
int pokus = 0;
byte t1hodina = 0;
byte t1minuta = 0;
byte t1den = 1;
byte t1mesic = 1;
byte zobrazujcas = 1;
byte alarmdenvtydnu = 9;
int opakovanialarmu = 1;  //pokud je 1 opakovani funguje, pokud je 0 budik se ten den uz neopakuje
byte minutaposunutehoalarmu = 10;
byte hodinaposunutehoalarmu = 12;
int pozadavekopakovanialarmu = 1;
int pozadavekposunutiminuty = 2;
int verze=2607;  //editovat pri zmene

int melody0[] = { NOTE_C4, NOTE_E4, NOTE_G4};
int pocetnot0 = 3;
int noteDurations0[] = { 4, 4, 4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
// notes in the melody: Slys jako to nad rekou  53 not vcetne pomlk

int melody1[] = {
    NOTE_D3, NOTE_G3, NOTE_G3, NOTE_B3, NOTE_G3, NOTE_G3, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3, 0,
    NOTE_D3, NOTE_G3, NOTE_G3, NOTE_B3, NOTE_G3, NOTE_G3, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3, 0,
    NOTE_D3, NOTE_C4, 0, NOTE_C4, NOTE_B3, 0, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_D4, 0, NOTE_D3, NOTE_C4, 0, NOTE_C4, NOTE_B3, 0,
    NOTE_B3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3, 0
};
int pocetnot1 = 51;
int noteDurations1[] = {
    2,2,2,2,2,2,2,2,2,4,4,2,2,2,2,2,2,2,2,2,2,2,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4,4,2,2
};




TM1637Display displej(CLK, DIO);

const uint8_t SEG_DB[] = {
    SEG_G,  // pomlcka
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, // D
    SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // B
    SEG_G    // pomlcka
};

const uint8_t SEG_VER[] = {
    SEG_C | SEG_F | SEG_G, // V leva cast
    SEG_B | SEG_E | SEG_G, // V prava cast
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E
    SEG_E | SEG_G // R
};

const uint8_t SEG_OPAK[] = {
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
    SEG_A | SEG_B | SEG_E | SEG_F | SEG_G, // P
    SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G, // A
    SEG_G // pomlcka
};

const uint8_t SEG_NENE[] = {
    SEG_C | SEG_E | SEG_G, // n
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, // E
    SEG_C | SEG_E | SEG_G, // n
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G // E
};



const uint8_t SEG_ALARM[] = {
    SEG_G,  // pomlcka
    SEG_A | SEG_B | SEG_C | SEG_G | SEG_E | SEG_F, // A
    SEG_F | SEG_E | SEG_D, // L
    SEG_G    // pomlcka
};

const uint8_t SEG_ULOZ[] = {
    SEG_G,  // pomlcka
    SEG_D | SEG_B | SEG_C | SEG_E | SEG_F, // U
    SEG_F | SEG_E | SEG_D, // L
    SEG_G    // pomlcka
};

const uint8_t SEG_ROK[] = {
    SEG_E | SEG_G,           // r
    SEG_C | SEG_D | SEG_E | SEG_G, // O
    SEG_E | SEG_F | SEG_G,  // leva cast  K
    SEG_A | SEG_D | SEG_E | SEG_F , // prava cast  K
};

const uint8_t SEG_MESIC[] = {
    SEG_A | SEG_B | SEG_E | SEG_F,
    SEG_A | SEG_B | SEG_C | SEG_F,
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
};

const uint8_t SEG_DEN[] = {
    SEG_B | SEG_C | SEG_E | SEG_D | SEG_G,
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_C | SEG_E | SEG_G,
    SEG_G,
};

const uint8_t SEG_TEPLOTA[] = {
    SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,
    SEG_D | SEG_E | SEG_F,
};

void setup()
{
    pinMode(4, OUTPUT); // nastavuji alarm
    pinMode(5, OUTPUT); // nastavuji cas
    pinMode(6, OUTPUT); // nastavuji mesic a den
    pinMode(7, OUTPUT); // sviti pokud to bude budit
    pinMode(9, OUTPUT); // zapojene dva 1W repracky paralelne
    pinMode(10, OUTPUT); // nic

    pinMode(12, INPUT); // senzorovy spinac
    pinMode(8, INPUT_PULLUP); // vypinac alarm on-off
    pinMode(13, OUTPUT);

    displej.setBrightness(jas); // pres den 7 , vecer a v noci 1

    Serial.begin(9600);

    // Initialize DS3231
    // Serial.println("Initialize DS3231");;
    clock.begin();

    // Set sketch compiling time
    // clock.setDateTime(__DATE__, __TIME__);
    clock.setBattery(1,0); // Dulezite nastaveni

    clock.armAlarm1(false);
    clock.armAlarm2(false);
    // clock.clearAlarm1();
    // clock.clearAlarm2();

    // Set Alarm - Every 01h:10m:30s in each day
    // setAlarm1(Date or Day, Hour, Minute, Second, Mode, Armed = true)
    //clock.setAlarm1(0, (casalarmuhod),(casalarmumin), 0, DS3231_MATCH_H_M_S);
    checkAlarms();

    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    displej.clear(); displej.setSegments(SEG_DB);
    delay(2000);
    displej.clear(); displej.setSegments(SEG_VER);
    delay(2000);
    displej.clear(); displej.showNumberDec(verze, true);
    delay(2000);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    for (int thisNote = 0; thisNote < pocetnot0; thisNote++) {
        int noteDuration0 = 1000 / noteDurations0[thisNote];
        tone(9, melody0[thisNote], noteDuration0);
        int pauseBetweenNotes = noteDuration0 * 1.30;
        delay(pauseBetweenNotes); };
        noTone(9);

}


void checkAlarms()
{
    RTCAlarmTime a1;
    RTCAlarmTime a2;

    if (clock.isArmed1())
    {
        a1 = clock.getAlarm1();

        Serial.print("Alarm1 is triggered ");
        switch (clock.getAlarmType1())
        {
            case DS3231_EVERY_SECOND:
                Serial.println("every second");
                break;
            case DS3231_MATCH_S:
                Serial.print("when seconds match: ");
                Serial.println(clock.dateFormat("__ __:__:s", a1));
                break;
            case DS3231_MATCH_M_S:
                Serial.print("when minutes and sencods match: ");
                Serial.println(clock.dateFormat("__ __:i:s", a1));
                break;
            case DS3231_MATCH_H_M_S:
                Serial.print("when hours, minutes and seconds match: ");
                Serial.println(clock.dateFormat("__ H:i:s", a1)); budetobudit = 1;
                break;
            case DS3231_MATCH_DT_H_M_S:
                Serial.print("when date, hours, minutes and seconds match: ");
                Serial.println(clock.dateFormat("d H:i:s", a1));
                break;
            case DS3231_MATCH_DY_H_M_S:
                Serial.print("when day of week, hours, minutes and seconds match: ");
                Serial.println(clock.dateFormat("l H:i:s", a1));
                break;
            default:
                Serial.println("UNKNOWN RULE");
                break;
        }
    } else
    {
        Serial.println("Alarm1 is disarmed.");
    }
}

void loop() {


    pot1Value = analogRead(A0); // funkce 0 az 4  tedy 5 rozsahu 0,1,2,3,4
    // hodnoty a rozsahy   0 je 0-205,  1 je 206-411,  2 je 412-617, 3 je 618-823, 4 je 824-1023
    pot2Value = analogRead(A1);
    pot3Value = analogRead(A2);

    // funkce - 0 - Bezi cas, nic se nenastavuje, funguje budik pokud je zapnuty
    // funkce - 1 - Nastavuje se alarm, pokud neni zapnut D8. Pokud je D8 zapnuty, nic se nedeje.
    // funkce - 2 - Nastavuje se  cas. Pokud je D8 zapnuto, nic se nedeje.
    // funkce - 3 - Nastavuje se opakovani stejneho dne a cas opakovani v minutach. Pokud je D8 zapnuto, nic se nedeje.
    // funkce - 4 - Zobrazuje den,mesic,rok, teplota, cas, budik neni aktivni vse stale dokola automaticky

    if (pot1Value > -1 and pot1Value < 206) {funkce=0; digitalWrite(4, LOW); digitalWrite(5, LOW); digitalWrite(6, LOW);};
    if (pot1Value > 205 and pot1Value < 412) {funkce=1; digitalWrite(4, HIGH); digitalWrite(5, LOW); digitalWrite(6,LOW);};
    if (pot1Value > 411 and pot1Value < 618) {funkce=2; digitalWrite(4, LOW); digitalWrite(5, HIGH); digitalWrite(6, LOW);};
    if (pot1Value > 617 and pot1Value < 824) {funkce=3; digitalWrite(4, LOW); digitalWrite(5, LOW); digitalWrite(6, HIGH);};
    if (pot1Value > 823 and pot1Value < 1024) {funkce=4; digitalWrite(4, HIGH); digitalWrite(5, HIGH); digitalWrite(6, HIGH);};

    // val = map(val, 0, 1023, 0, 255); ukazka funkce premapovani

    displej.setBrightness(jas);
    dt = clock.getDateTime();

    sekunda=(dt.second);
    hodina=(dt.hour); minuta=(dt.minute);
    rok=(dt.year); denvtydnu=(dt.dayOfWeek);
    den=(dt.day); mesic=(dt.month);
    a1hodina=(clock.getAlarm1().hour);
    a1minuta=(clock.getAlarm1().minute);

    //  nasledujici blok je docasne zneplatnen
    //  a2hodina=(clock.getAlarm2().hour); // je zde ulozeno opakovani ano nebo ne
    //  a2minuta=(clock.getAlarm2().minute); // je zde hodnota opakovani v minutach
    // if (a2hodina>1) {a2hodina=1; a2hodina=opakovanialarmu;};
    // opakovanialarmu=a2hodina;
    // pozadavekposunutiminuty=a2minuta;


    // Nacte teplotu do pameti
    clock.forceConversion();
    teplota = (clock.readTemperature())-1;

    // nasledujici blok nachysta 4 cislice pro zobrazeni casu
    if (hodina > 19) { c1=2; c2=hodina-20;} ;
    if (hodina > 9 and hodina<20) { c1=1; c2=hodina-10;};
    if (hodina < 10) { c1=0; c2=hodina;} ;
    if (minuta > 49) { c3=5; c4=minuta-50 ;} ;
    if (minuta > 39 and minuta < 50) { c3=4; c4=minuta-40 ;} ;
    if (minuta > 29 and minuta < 40) { c3=3; c4=minuta-30 ;} ;
    if (minuta > 19 and minuta < 30) { c3=2; c4=minuta-20 ;} ;
    if (minuta > 9 and minuta < 20) { c3=1; c4=minuta-10 ;} ;
    if (minuta < 10) {c3=0; c4=minuta; } ;
    if (minuta > 59) {c3=9; c4=9 ;} ;
    if (minuta < 0) {c3=8; c4=8 ;} ;
    if (hodina > 23) {c1=9; c2=9 ;} ;
    if (hodina < 0) {c1=8; c2=8 ;} ;

    // PIN4 HIGH - nastavuji alarm
    // PIN5 HIGH - nastavuji cas
    // PIN6 HIGH - nastavuji opakovani
    // PIN7 HIGH - sviti pokud to bude budit
    // PIN9 - dva reproduktory

    pin8=digitalRead(8); //kontroluje vypinac
    pin12=digitalRead(12); //kontrola senzor spinace
    if (pin12 == 1) { displej.setSegments(SEG_ALARM);
        delay(1500);displej.clear();
        displej.showNumberDec(a1hodina,false,2,0);displej.showNumberDec(a1minuta,false,2,2);
        delay(2000); displej.setSegments(SEG_OPAK); delay(2000);
        if (opakovanialarmu==0) { displej.setSegments(SEG_NENE);};
        if (opakovanialarmu==1) { displej.clear ();displej.showNumberDec(pozadavekposunutiminuty,false,2,2);};
        delay(2000);
    };

    pin8=digitalRead(8);
    pin12=digitalRead(12);
    if (pin8 == 0 ) {budetobudit=1; zap=zap+1; vyp=0;} ; //vypinac je zapnuty

    if (zap == 1 and pot1Value > 205 and pot1Value < 412) {displej.clear(); displej.setSegments(SEG_ULOZ);
        clock.setAlarm1(0, (casalarmuhod),(casalarmumin), 0, DS3231_MATCH_H_M_S);digitalWrite(7,LOW);delay(2000);}; //ulozen cas alarmu hod,min,0 sekund

        if (zap == 1 and pot1Value > 411 and pot1Value < 618) {displej.clear(); displej.setSegments(SEG_ULOZ);
            clock.setDateTime(rok,mesic,den,t1hodina,t1minuta,0);digitalWrite(7,LOW);delay(2000);}; //ulozena hodina, minuta

            if (zap == 1 and pot1Value > 617 and pot1Value < 824) {displej.clear(); displej.setSegments(SEG_ULOZ); opakovanialarmu=pozadavekopakovanialarmu;
                digitalWrite(7,LOW);delay(2000);};
                // cast vyjmuta z radku c.345 clock.setAlarm2(0, (opakovanialarmu),(pozadavekposunutiminuty), DS3231_MATCH_H_M);

                if (zap > 10) {zap=10;};
                if (pin8 == 1 ) {budetobudit=0; vyp=vyp+1; zap=0; }; //vypinac je vypnuty - nebudime
                if (vyp > 10) {vyp=10;};
                if (budetobudit == 1) {digitalWrite(7, HIGH);} else {digitalWrite(7, LOW);}; //dioda D7 sviti pokud je alarm zapnuty

                uint8_t vypis[] = { 0, 0, 0, 0 };
                vypis[0] = displej.encodeDigit(c1);
                vypis[1] = displej.encodeDigit(c2);
                vypis[2] = displej.encodeDigit(c3);
                vypis[3] = displej.encodeDigit(c4);
                displej.setSegments(vypis);

                if (clock.isAlarm1() and budetobudit==1)
                {displej.setBrightness(7);
                    for (int thisNote = 0; thisNote < pocetnot1; thisNote++) {
                        int noteDuration1 = 1000 / noteDurations1[thisNote];
                        tone(9, melody1[thisNote], noteDuration1);
                        pin8=digitalRead(8); if (pin8==1) {noTone(9); jeAlarm=70; thisNote=54;};
                        int pauseBetweenNotes = noteDuration1 * 1.30;
                        delay(pauseBetweenNotes);
                        noTone(9);
                    }
                    delay(100); displej.setBrightness(jas); }; //ted probiha alarm

                    pin8=digitalRead(8);
                    pin12=digitalRead(12);
                    pot1Value = analogRead(A0);
                    pot2Value = analogRead(A1);
                    pot3Value = analogRead(A2);
                    if (jeAlarm > 60) {jeAlarm=1; digitalWrite(7, LOW); budetobudit=0; delay(100) ; } ; // Alarm skoncil vyprsel cas nebo bylo vypnuto vypinacem
                    delay(200);

                    if (budetobudit==1 and opakovanialarmu==1) {minutaposunutehoalarmu=a1minuta+pozadavekposunutiminuty; hodinaposunutehoalarmu=a1hodina;};
                    if (minutaposunutehoalarmu>59) {minutaposunutehoalarmu=minutaposunutehoalarmu-60; hodinaposunutehoalarmu=hodinaposunutehoalarmu+1;};
                    if (budetobudit==1 and opakovanialarmu==1 and hodina==hodinaposunutehoalarmu and minuta==minutaposunutehoalarmu and sekunda==0) {
                        displej.setBrightness(7);
                        for (int thisNote = 0; thisNote < 54; thisNote++) {
                            int noteDuration1 = 1000 / noteDurations1[thisNote];
                            tone(9, melody1[thisNote], noteDuration1);
                            pin8=digitalRead(8); if (pin8==1) {noTone(9); jeAlarm=70; thisNote=54;};
                            int pauseBetweenNotes = noteDuration1 * 1.30;
                            delay(pauseBetweenNotes);
                            noTone(9);
                        };

                    };


                    // uint8_t vypis[] = { 0, 0, 0, 0 };
                    vypis[0] = displej.encodeDigit(c1);
                    vypis[1] = displej.encodeDigit(c2);
                    vypis[2] = displej.encodeDigit(c3);
                    vypis[3] = displej.encodeDigit(c4);
                    displej.setSegments(vypis);
                    delay(200);
                    pot1Value = analogRead(A0); // volic funkce
                    pot2Value = analogRead(A1); // meni parametr - hodiny nebo mesic
                    pot3Value = analogRead(A2); // meni parament - minuty nebo den v mesici
                    pin8=digitalRead(8); //kontroluje vypinac
                    pin12=digitalRead(12); //kontroluje vypinac

                    // PIN4 HIGH - nastavuji alarm
                    // PIN5 HIGH - nastavuji cas
                    // PIN6 HIGH - nastavuji opakovani
                    // PIN7 HIGH - sviti pokud to bude budit
                    // PIN9 - repracky

                    // funkce - 0 - Bezi cas, nic se nenastavuje, funguje budik pokud je zapnuty
                    // funkce - 1 - Nastavuje se alarm, pokud neni zapnut D8. Pokud je D8 zapnuty, nic se nedeje.
                    // funkce - 2 - Nastavuje se  cas. Pokud je D8 zapnuto, nic se nedeje.
                    // funkce - 3 - Nastavuje se opakovani. Pokud je D8 zapnuto, nic se nedeje.
                    // funkce - 4 - Zobrazuje se den,mesic,rok, teplota, cas, budik neni aktivni vse stale dokola
                    // u funkci 1 az 3 pokud zapnete D8 hodnota na displeji se ulozi do RTC

                    vypis[1] = vypis[1]+128;
                    displej.setSegments(vypis); digitalWrite(13, HIGH);
                    delay(200);

                    pot1Value = analogRead(A0); // volic funkce
                    pot2Value = analogRead(A1);
                    pot3Value = analogRead(A2);
                    pin8=digitalRead(8);
                    pin12=digitalRead(12); //kontrola senzor spinace
                    if (pin12 == 1) { displej.setSegments(SEG_ALARM);
                        delay(1500);displej.clear();
                        displej.showNumberDec(a1hodina,false,2,0);displej.showNumberDec(a1minuta,false,2,2);
                        delay(2000); displej.setSegments(SEG_OPAK); delay(2000);
                        if (opakovanialarmu==0) { displej.setSegments(SEG_NENE);};
                        if (opakovanialarmu==1) { displej.clear();displej.showNumberDec(pozadavekposunutiminuty,false,2,2);};
                        delay(2000);
                    };
                    if (funkce>0) delay(200);

                    while (pot1Value > 205 and pot1Value < 412 and pin8 == 1) {pin8=digitalRead(8); pin12=digitalRead(12); pot1Value = analogRead(A0);
                        pot2Value = analogRead(A1); casalarmuhod=pot2Value/44;  pot3Value = analogRead(A2);
                        casalarmumin=pot3Value/17.5; displej.showNumberDec(casalarmuhod,false,2,0);
                        displej.showNumberDec(casalarmumin,false,2,2); digitalWrite(4, HIGH);digitalWrite(5, LOW); digitalWrite(6,LOW);}; // funkce c.1 nastavuji alarm

                        while (pot1Value > 411 and pot1Value < 618 and pin8 == 1) {pin8=digitalRead(8); pin12=digitalRead(12); pot1Value = analogRead(A0);
                            pot2Value = analogRead(A1); t1hodina=pot2Value/44;  pot3Value = analogRead(A2);
                            t1minuta=pot3Value/17.5; displej.showNumberDec(t1hodina,false,2,0);
                            digitalWrite(4, LOW); digitalWrite(5, HIGH); digitalWrite(6, LOW);
                            displej.showNumberDec(t1minuta,false,2,2);}; // funkce c.2 nastavuji cas - hodiny a minuty

                            // funkce c.3 - Opakovani budiku v den buzeni   ano/ne  pokud ano pot2 nastavuje prodlevu buzeni v minutach
                            while (pot1Value > 617 and pot1Value < 824 and pin8 == 1) {pin8=digitalRead(8); pin12=digitalRead(12); pot1Value = analogRead(A0);
                                pot2Value = analogRead(A1); pot3Value = analogRead(A2);
                                if (pot2Value < 514) {pozadavekopakovanialarmu=0;};
                                if (pot2Value > 513) {pozadavekopakovanialarmu=1;};
                                displej.showNumberDec(pozadavekopakovanialarmu,false,2,0);
                                pozadavekposunutiminuty = ((pot3Value+1)/50)+2;
                                digitalWrite(4, LOW); digitalWrite(5, LOW); digitalWrite(6, HIGH);
                                displej.showNumberDec(pozadavekposunutiminuty,false,2,2);delay(10);}; // funkce c.3

                                pot1Value = analogRead(A0);
                                while (pot1Value > 823) {pot1Value = analogRead(A0); digitalWrite(4, HIGH); digitalWrite(5, HIGH); digitalWrite(6, HIGH);
                                    clock.forceConversion(); teplota = (clock.readTemperature());
                                    displej.clear(); displej.setSegments(SEG_TEPLOTA); delay(1500); pot1Value = analogRead(A0);
                                    displej.clear(); displej.showNumberDec(teplota); delay(1500); pot1Value = analogRead(A0);
                                    displej.clear(); displej.setSegments(SEG_ROK); delay(1500); pot1Value = analogRead(A0);
                                    displej.clear(); displej.showNumberDec(rok); delay(1500); pot1Value = analogRead(A0);
                                    displej.clear(); displej.setSegments(SEG_MESIC); delay(1500); pot1Value = analogRead(A0);
                                    displej.clear(); displej.showNumberDec(mesic); delay(1500); pot1Value = analogRead(A0);
                                    displej.clear(); displej.setSegments(SEG_DEN); delay(1500); pot1Value = analogRead(A0);
                                    displej.clear(); displej.showNumberDec(den); delay(1500); pot1Value = analogRead(A0);
                                };  // funkce c.4

                                if (hodina > 21 or hodina <6) {jas = 1; } else {jas = 7 ;}; // vecer a v noci je jas na 1

                                pin8=digitalRead(8); //kontroluje zapnuty vypinac
                                digitalWrite(13, LOW);
                                pot1Value = analogRead(A0); // volic funkce
                                pot2Value = analogRead(A1);
                                pot3Value = analogRead(A2);
                                pin8=digitalRead(8); pin12=digitalRead(12);
                                displej.setBrightness(jas);

}
