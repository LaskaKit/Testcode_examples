/*** Program pro řízení krok. motoru pro navíjení cívek ***
 *
 *   Arduino UNO + Motor Control Shield 2xL293D
 *   Zobrazení stavu na LCD1602 připojeném přes převodník I2C
 *   Ovládání menu pomocí pěti tlačítek VPRAVO, VLEVO, NAHORU, DOLU a ENTER
 *   Řízení rychlosti motoru potenciometrem 25k (zapojení +5V, A0, GND)
 */
#include <AFMotor.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int MaxOt = 100; // default zadání počtu požadovaných závitů
int PolMen = 0;  // indikátor běhu programu 0=setup, 1=start
int S = 11;     //
int D = 12;     //  pozice pro zobrazení na LCD Stovky, Desítky, Jednotky
int J = 13;     //
int PosCur;
// Tlačítka připojená na digitální vstupy proti zemi
const int Tleft = 0;
const int Tright = 1;
const int Tup = 2;
const int Tdown = 9;
const int Tenter = 10;
int StavTl = 1;        // stav, kdy není stisknuté tlačítko
int StavTr = 1;
int StavTu = 1;
int StavTd = 1;
int StavTe = 1;
int PGMrun = 0;       // během běhu navíjení nezobrazit MENU, 0 => MENU, 1 => RUN
// Menu
char Msetup[] = "=> SETUP start";
char Mstart[] = "setup => START";

const int stepsPerRevolution = 100;  // motor STH-39G102
// 3,6deg/step => 100step/rev; 12V
const int partStep = 10;             // počet kroků provedených stejnou rychlostí
const int MaxSpeed = 100;            // nastavení max. rychlosti otáčení motoru na 100 rpm:
const int Smer = FORWARD;            // směr otáčení motoru FORWARD nebo BACKWARD
LiquidCrystal_I2C lcd(0x27, 16, 2);
AF_Stepper myStepper(stepsPerRevolution, 1);

void setup() {
    lcd.begin();
    // Turn on the backlight and print a message.
    lcd.backlight();
    lcd.home();
    lcd.print("Navijecka civek");
    lcd.setCursor(0, 1);
    lcd.print(Msetup);    //zobrazení menu setup nebo start
    // nastavení portů pro tlačítka
    pinMode(Tleft,INPUT_PULLUP);
    pinMode(Tright,INPUT_PULLUP);
    pinMode(Tup,INPUT_PULLUP);
    pinMode(Tdown,INPUT_PULLUP);
    pinMode(Tenter,INPUT_PULLUP);
}

void loop() {
    if (PGMrun < 1){
        Menu();
        switch(PolMen){
            case 0:     //setup
                Nastav();
                break;
            case 1:     //start
                while(StavTe > 0){            // čekání na stisk Enter (*)
                    lcd.home();///////////
                    lcd.print("Pocet z. = ");
                    if (MaxOt < 100){lcd.print(" ");}
                    if (MaxOt < 10){lcd.print(" ");}
                    lcd.print(MaxOt);
                    lcd.setCursor(0, 1);
                    lcd.print("Stisk * => START");
                    StavTe = digitalRead(Tenter); // test na stisk (*)
                }
                StavTe = 1;     // zrušit stisk *
                PGMrun = 1;     // povolení startu
                break;
        }
    }
    else {
        lcd.setCursor(0, 1);///////
        lcd.print("Zbyva z. =      ");
        Start(MaxOt);
    }
}

void JednaOtacka(int Step){         // provedení jedné otáčky s možností zastavení
    int i=Step;                       // pro případ zasukování drátu, po odstranění
    while(i>0){                       // problému lze pokračovat
        // nastavení rychlosti:
        int sensorReading = analogRead(A0);   // čtení hodnoty potenciometru
        // map it to a range from 0 to 100:
        int motorSpeed = map(sensorReading, 0, 1023, 1, MaxSpeed); //
        if (motorSpeed < 2){
            while (motorSpeed < 2){       // čekání na odstranění zasukování
                sensorReading = analogRead(A0);
                // map it to a range from 0 to 100:
                motorSpeed = map(sensorReading, 0, 1023, 1, MaxSpeed);
                delay(1000);
            }
        }
        myStepper.setSpeed(motorSpeed);   // nastavení rychlosti motoru
        myStepper.step(partStep, Smer, DOUBLE);         //
        i=i-10;
    }
}

void Start(int Xotacek){            // spuštění motoru pro navinutí X otáček
    for (int y=0;y<Xotacek;y++){
        JednaOtacka(stepsPerRevolution);
        ZobrazCislo(Xotacek-y-1);       // zobrazení zbývajících otáček
    }
    PGMrun = 0;     //zobrazení menu
    lcd.home();
    lcd.print("Navinuto ");
    lcd.print(MaxOt);
    lcd.print(" z.  ");
    lcd.setCursor(0, 1);
    lcd.print(Msetup);
    PolMen = 0;
    delay(500);
}

void ZobrazCislo(int Cislo){    // zobrazení čísla na požadovanou pozici
    int poz;
    if (Cislo>99)
    {
        poz = S;
    }
    else if (Cislo>9)
    {
        poz = D;
    }
    else
    {
        poz = J;
    }
    lcd.setCursor(poz-1, 1);
    lcd.print(" ");
    lcd.setCursor(poz, 1);
    lcd.print(Cislo);
}

void Menu(){      //výběr položek menu
    while(StavTe > 0){            // čekání na stisk Enter (*)
        StavTr = digitalRead(Tright); // při stisku pravého tlačítka změň zobrazení na => START
        if (StavTr == 0){
            lcd.setCursor(0, 1);
            lcd.print(Mstart);
            PolMen = 1;                 // vybráno START
        }
        StavTl = digitalRead(Tleft);  // při stisku levého tlačítka změ%n na => SETUP
        if (StavTl == 0){
            lcd.setCursor(0, 1);
            lcd.print(Msetup);
            PolMen = 0;                 // vybráno SETUP
        }
        StavTe = digitalRead(Tenter); // test na stisk (*)
    }
    delay(500);
    StavTe = 1;
}

void Nastav(){      // nastavení počtu závitů v SETUPu
    if (MaxOt > 99){
        PosCur = S;
    }
    else if (MaxOt > 9){
        PosCur = D;
    }
    else{
        PosCur = J;
    }
    lcd.home();
    lcd.print("Pocet zavitu   ");    // zobrazit výchozí stav
    lcd.setCursor(0, 1);
    lcd.print("       x =    ");    //
    lcd.setCursor(PosCur, 1);
    lcd.print(MaxOt);
    lcd.setCursor(PosCur, 1);
    lcd.cursor();         // zapnout kurzor a blikat?
    //      lcd.blink();
    delay(1000);
    while(StavTe > 0){    // čekání na stisk Enter (*)
        StavTu = digitalRead(Tup);  // při stisku tl. nahoru zvětšit MaxOt dle pozice kurzoru
        if (StavTu == 0){
            switch (PosCur){
                case 11:
                    if (MaxOt < 900){
                        MaxOt = MaxOt + 100;
                    }
                    ZobrazCislo(MaxOt);
                    lcd.setCursor(PosCur, 1);
                    delay(500);
                    break;
                case 12:
                    if (MaxOt < 980){
                        MaxOt = MaxOt + 10;
                    }
                    ZobrazCislo(MaxOt);
                    lcd.setCursor(PosCur, 1);
                    delay(500);
                    break;
                case 13:
                    if (MaxOt < 998){
                        MaxOt = MaxOt + 1;
                    }
                    ZobrazCislo(MaxOt);
                    lcd.setCursor(PosCur, 1);
                    delay(500);
                    break;
            }
            StavTu =1;
        }
        StavTd = digitalRead(Tdown);  // při stisku tl. dolu zmenšit MaxOt dle pozice kurzoru
        if (StavTd == 0){
            switch (PosCur){
                case 11:
                    if (MaxOt >= 100){
                        MaxOt = MaxOt - 100;
                    }
                    ZobrazCislo(MaxOt);
                    lcd.setCursor(PosCur, 1);
                    delay(500);
                    break;
                case 12:
                    if (MaxOt >= 20){
                        MaxOt = MaxOt - 10;
                    }
                    ZobrazCislo(MaxOt);
                    lcd.setCursor(PosCur, 1);
                    delay(500);
                    break;
                case 13:
                    if (MaxOt >= 1){
                        MaxOt = MaxOt - 1;
                    }
                    ZobrazCislo(MaxOt);
                    lcd.setCursor(PosCur, 1);
                    delay(500);
                    break;
            }
            StavTd = 1;
        }
        StavTr = digitalRead(Tright);  // při stisku tl. doprava posunout pozici kurzoru
        if (StavTr == 0){
            if (PosCur < 13){
                PosCur++;
                lcd.setCursor(PosCur, 1);
                delay(500);
            }
            StavTr = 1;
        }
        StavTl = digitalRead(Tleft);  // při stisku tl. doleva posunout pozici kurzoru
        if (StavTl == 0){
            if (PosCur > 11){
                PosCur--;
                lcd.setCursor(PosCur, 1);
                delay(500);
            }
            StavTl = 1;
        }
        StavTe = digitalRead(Tenter);  // test na stisk (*)
        //delay(500);
    }
    StavTe = 1;
    lcd.noCursor();   //vypnout kurzor
    lcd.setCursor(0, 1);
    lcd.print(Msetup);
    delay(500);
}
