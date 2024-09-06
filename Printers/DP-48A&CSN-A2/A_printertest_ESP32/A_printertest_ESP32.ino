/* Test code for DP-48A and CSN-A2 58mm thermal printers
 * Demonstrates a few text styles & layouts, bitmap printing, etc.
 * 
 * Board:   LaskaKit ESP32-DEVKit         https://www.laskakit.cz/laskakit-esp32-devkit/
 * Printer: DP-48A 58mm thermal printer:  https://www.laskakit.cz/dp-48a-58mm-modul-termotiskarny/
 *      or: CSN-A2 58mm thermal printer:  https://www.laskakit.cz/csn-a2-58mm-termotiskarna-do-panelu--uart/
 * 
 * Library example used from Adafruit for thermal printers: https://github.com/adafruit/Adafruit-Thermal-Printer-Library
 *
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */
 
#include "Adafruit_Thermal.h"
#include "adalogo.h"
#include "adaqrcode.h"

#include <HardwareSerial.h>
#define TX_PIN 17 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 16 // Arduino receive   GREEN WIRE   labeled TX on printer

// Using hardware serial:
HardwareSerial SerialPort(1);
Adafruit_Thermal printer(&SerialPort);     // Pass addr to printer constructor

void setup() {

  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  SerialPort.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  printer.begin();        // Init printer (same regardless of serial type)

  // Font options
  printer.setFont('B');
  printer.println("FontB");
  printer.println("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  printer.setFont('A');
  printer.println("FontA (default)");
  printer.println("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

  // Test inverse on & off
  printer.inverseOn();
  printer.println(F("Inverse ON"));
  printer.inverseOff();

  // Test character double-height on & off
  printer.doubleHeightOn();
  printer.println(F("Double Height ON"));
  printer.doubleHeightOff();

  // Set text justification (right, center, left) -- accepts 'L', 'C', 'R'
  printer.justify('R');
  printer.println(F("Right justified"));
  printer.justify('C');
  printer.println(F("Center justified"));
  printer.justify('L');
  printer.println(F("Left justified"));

  // Test more styles
  printer.boldOn();
  printer.println(F("Bold text"));
  printer.boldOff();

  printer.underlineOn();
  printer.println(F("Underlined text"));
  printer.underlineOff();

  printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
  printer.println(F("Large"));
  printer.setSize('M');
  printer.println(F("Medium"));
  printer.setSize('S');
  printer.println(F("Small"));

  printer.justify('C');
  printer.println(F("normal\nline\nspacing"));
  printer.setLineHeight(50);
  printer.println(F("Taller\nline\nspacing"));
  printer.setLineHeight(); // Reset to default
  printer.justify('L');

  // Barcode examples:
  // CODE39 is the most common alphanumeric barcode:
  printer.printBarcode("ADAFRUT", CODE39);
  printer.setBarcodeHeight(100);
  // Print UPC line on product barcodes:
  printer.printBarcode("123456789123", UPC_A);

  // Print the 75x75 pixel logo in adalogo.h:
  printer.printBitmap(adalogo_width, adalogo_height, adalogo_data);

  // Print the 135x135 pixel QR code in adaqrcode.h:
  printer.printBitmap(adaqrcode_width, adaqrcode_height, adaqrcode_data);
  printer.println(F("Adafruit!"));
  printer.feed(2);

  printer.sleep();      // Tell printer to sleep
  delay(3000L);         // Sleep for 3 seconds
  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults
}

void loop() {
}
