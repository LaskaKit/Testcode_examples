void setup() {

    Serial.begin(115200);

    Serial.println(„ESP32-C3 USB-CDC USBSerial test“);

}

void loop() {

    Serial.println(„LaskaKit“);

    delay(2000);

    /* Pokud chces uspat cip*/

    //Serial.flush();

    //esp_sleep_enable_timer_wakeup(PocetSekund * 1000000);

    //esp_deep_sleep_start();

}
