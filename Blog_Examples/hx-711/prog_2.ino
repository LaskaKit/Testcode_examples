// Arduino AD 24-bit převodník s 2 kanály
// piny pro připojení SCK a DT z modulu
int pSCK = 2;
int pDT = 3;
float output1;float output2;
// definování různých nastavení kanálů a jejich zesílení
#define kanal_A_zesil_128 1
#define kanal_B_zesil_32 2
#define kanal_A_zesil_64 3

void setup() {
    // nastavení pinů modulu jako výstup a vstup
    pinMode(pSCK, OUTPUT);
    pinMode(pDT, INPUT);
    // set komunikace přes sériovou linku rychlostí 115200 baud
    Serial.begin(115200);delay(2000);
    // probuzení modulu z power-down módu
    digitalWrite(pSCK, LOW);
    // spuštění prvního měření pro nastavení měřícího kanálu
    output1=spusteniMereni(kanal_A_zesil_64);
}

void loop() {
    // 2.měření a jeho výsledku
    output2=spusteniMereni(kanal_A_zesil_64);
    Serial.print(" output1: ");
    Serial.print(output1);
    Serial.print(" output2: ");
    Serial.println(output2);
    delay(5000);
}

//-------------------------------------------------------------------------------
// vytvoření funkce pro měření z nastaveného kanálu
long spusteniMereni(byte mericiMod) {
    byte index;
    long vysledekMereni = 0L;
    // načtení 24-bit dat z modulu
    while(digitalRead(pDT));
    for (index = 0; index < 24; index++) {
        digitalWrite(pSCK, HIGH);
        vysledekMereni = (vysledekMereni << 1) | digitalRead(pDT);
        digitalWrite(pSCK, LOW);
    }
    // nastavení měřícího módu
    for (index = 0; index < mericiMod; index++) {
        digitalWrite(pSCK, HIGH);
        digitalWrite(pSCK, LOW);
    }
    // konverze z 24-bit dvojdoplňkového čísla na 32-bit znaménkové číslo
    if (vysledekMereni >= 0x800000) vysledekMereni = vysledekMereni | 0xFF000000L;

    // přepočet výsledku na mikrovolty podle zvoleného kanálu a zesílení
    /*switch(mericiMod) {
     * case 1: vysledekMereni = vysledekMereni/128/2; break;
     * case 2: vysledekMereni = vysledekMereni/32/2; break;
     * case 3: vysledekMereni = vysledekMereni/64/2; break;
}*/
    return vysledekMereni ;
}
