void setup() {
    //inicializace Serial
    Serial.begin(115200);
}

void loop() {
    // proměnná pro čtení dat
    int hodnota = 0;
    // čtení hodnoty z hallova čidla
    hodnota = hallRead();
    // zobrazení hodnoty v serial terminal
    Serial.println(hodnota);

    // čekání
    delay(1000);
}
