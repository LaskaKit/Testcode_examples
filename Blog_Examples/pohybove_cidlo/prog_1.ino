void setup() {
    Serial.begin(9600);  // Nastaveni seriove konzole
    pinMode(6, INPUT);  // Nastaveni pinu D6 jako vstupni
    digitalWrite(6, LOW);  // Vychozi nastaveni pinu D6
}

void loop() {
    if(digitalRead(6) == HIGH) { // Precteme stav pinu D6 a pokud je jeho stav logicka 1
        Serial.println("Detekovan pohyb!");  // Vypiseme na konzoli hlaseni
    }
    delay(1000);  // Prodleva 1s pred dalsim merenim
}
