#define LED_BUILTIN 1 // Integrovana LED dioda na pinu PB1
#define BUTTON 0 // Tlacitko na pinu PB0
#define PWM_OUT 4 // Vystup PWM pro vysilac na pinu PB4

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);  // Pin LED jako vystup
    pinMode(BUTTON, INPUT);  // Pin tlacitka jako vstup
    pinMode(PWM_OUT, OUTPUT);  // Pin pro PWM vystup jako vystup
}

void loop() {
    if (digitalRead(BUTTON)){ // Pokud je tlacitko sepnuto
        digitalWrite(LED_BUILTIN, HIGH); // rozsviti se integrovana LED
        analogWrite(PWM_OUT, 127); // vygeneruje se PWM 50%
    }else{ // Pokud tlacitko sepnuto neni
        digitalWrite(LED_BUILTIN, LOW); // zhasne LED
        analogWrite(PWM_OUT, 0);  // PWM 0%
    }

    delay(100);
}
