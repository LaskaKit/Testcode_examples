/* Display dragon test for Good Display GDEP133UT3
 * example from GxEPD2 library is used
 * 
 * Board:   Epdiy V7                            https://vroland.github.io/epdiy-hardware/
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
 * - For Good Display Parallel Board:
 *    * Board: ESP32 Wrover Module
 *    * Partition Scheme: Default 4MB
 *
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

// Display Library example for parralel e-paper panels from Dalian Good Display.
// Requires EPDIY Library and https://github.com/vroland/epdiy

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
