#define LED_BUILTIN 1 // integrovana LED dioda na pinu PB1
#define PWM_IN_PIN 0  // PWM signal od prijimace na pinu PB0

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);  // Pin LED jako vystup
    pinMode(PWM_IN_PIN, INPUT);  // pin PWM jako vstup
}

void loop() {
    unsigned long highTime = pulseIn(PWM_IN_PIN, HIGH); // Zmerime delku impulzu v log H
    unsigned long lowTime = pulseIn(PWM_IN_PIN, LOW);  // Zmerime delku impulzu v log L
    unsigned long cycleTime = highTime + lowTime;  // Vypocitame celkovou delku PWM cyklu
    float dutyCycle = ((float)highTime / (float)cycleTime) * 100; // Vypocitame stridu PWM v %

    if ((dutyCycle >= 45.0) && (dutyCycle <= 50.0)){  // Pokud je strida v rozsahu 45 - 50%
        digitalWrite(LED_BUILTIN, HIGH);  // rozsviti se integrovana LED
    }else{
        digitalWrite(LED_BUILTIN, LOW);  // zhasne LED
    }

    delay(100);
}
