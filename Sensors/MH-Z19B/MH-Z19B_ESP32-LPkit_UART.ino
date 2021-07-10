/*
 * Test code for MH-Z19B with ESP32-LPkit
 * Serial2 is used as interface for CO2 sensor
 * RX - GPIO16
 * TX - GPIO17
 * 
 * 2021 made by laskarduino
 * 
 * Libraries:
 * https://github.com/Erriez/ErriezMHZ19B
 */

#include <ErriezMHZ19B.h>

#define mhzSerial Serial2 // Definition of Serial2, RX GPIO16; TX GPIO17

ErriezMHZ19B mhz19b(&mhzSerial); // Create a object

void setup() {
  Serial.begin(115200); // initialization of Serial;
  mhzSerial.begin(9600); // Set the baudrate for Serial2 for MH-Z19B

  delay(500);
  Serial.println("Test code for MH-Z19B");

  // Detect sensor
  while ( !mhz19b.detect() ) 
  {
      delay(2000);
      Serial.println("Sensor not detected.");
  };

  Serial.println("Sensor is detected.");
  Serial.println("Waiting for warming.");

  // Wait for warming-up completion
  while ( mhz19b.isWarmingUp() ) 
  {
      delay(2000);
      Serial.println("Warming-up.");
  };  
  Serial.println("Warming is done.");
}

void loop() {
    // wait for minimal interval
    if (mhz19b.isReady()) 
    {
      // Read and print CO2
      Serial.println(mhz19b.readCO2());
    }
}
