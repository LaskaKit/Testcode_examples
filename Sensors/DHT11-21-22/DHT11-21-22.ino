/*
  Example testing sketch for various DHT humidity/temperature sensors
    REQUIRES the following Arduino libraries:
    - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
    - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

  DHT sensor attached as follows:
  * Arduino Uno:
    ** SDA - 4
  * LaskaKit ESP-12 Board (DTH-11/ -22 Shields)
    ** SDA - 2
  * Wemos D1 mini (DTH-11/ -22 Shields)
    ** SDA - D4
  * ESP-01 (Shield)
    ** SDA - 2
*/


#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
}
