void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    int i;
    Serial.println("test");
    for(i=0; i < 3; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);

        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
    }

    delay(500);

    for(i=0; i < 2; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);

        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
}
