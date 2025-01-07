/* Display test for Good Display GDEP133UT3
 * example from GxEPD2 library is used
 * 
 * Board:   Epdiy V7 (for test need +27V, board has 22V)                             https://vroland.github.io/epdiy-hardware/
 *          Good Display Parallel Screen Board  https://www.laskakit.cz/
 * Display: Good Display GDEP133UT3             https://www.laskakit.cz/good-display-gdep133ut3-13-3--1600x1200-parallel-epaper-displej/
 * 
 * Set the board correct:
 * - For Epdiy V7:
 *    * Board: ESP32S3 Dev Module
 *    * Flash Size: 16MB
 *    * Partition Scheme: Default 4MB
 *    * PSRAM: OPI PSRAM
 *
 * - For Good Display Parallel Screen Board:
 *    * Board: ESP32 Wrover Module
 *    * Partition Scheme: Default 4MB
 *
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// Display Library example for parralel e-paper panels from Dalian Good Display.
// Requires EPDIY Library and https://github.com/vroland/epdiy

/**
 * This is the Arduino wrapper for the "Demo" example.
 * Please go to the main.c for the main example file.
 *
 * This example was developed for the ESP IoT Development Framework (IDF).
 * You can still use this code in the Arduino IDE, but it may not look
 * and feel like a classic Arduino sketch.
 * If you are looking for an example with Arduino look-and-feel,
 * please check the other examples.
 */

// Important: These are C functions, so they must be declared with C linkage!
extern "C" {
void idf_setup();
void idf_loop();
}

void setup() {
    if (psramInit()) {
        Serial.println("\nThe PSRAM is correctly initialized");
    } else {
        Serial.println("\nPSRAM does not work");
    }

    idf_setup();
}

void loop() {
    idf_loop();
}
