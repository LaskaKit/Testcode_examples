// Laserovy dalkomer na I2C konektoru desky. Firmware sam rozpozna, ktery
// z podporovanych senzoru je pripojeny, a podle nej nastavi rozsah stupnice:
//
//   VL53L1X - dosah az 4 m, knihovna "VL53L1X" od Pololu
//   VL53L0X - dosah az 2 m, knihovna "VL53L0X" od Pololu
//
// Oba maji I2C adresu 0x29, lisi se identifikacnim registrem.
#pragma once

#include <stdint.h>

#include "AlarmLogic.h"

enum SensorModel : uint8_t {
  SENSOR_NONE = 0,
  SENSOR_VL53L1X,
  SENSOR_VL53L0X,
};

// Vlastnosti senzoru, ktere zbytek firmwaru potrebuje znat.
struct SensorProfile {
  SensorModel model;
  const char *name;
  uint16_t scaleMaxMm;      // konec stupnice a nejvyssi mez alarmu
  float strongSignalMcps;   // od teto sily odrazu plati i jediny vzorek
};

struct DistanceSample {
  SampleKind kind = SAMPLE_ERROR;  // jak moc vzorku verit, viz AlarmLogic.h
  uint16_t mm = 0;       // vzdalenost v milimetrech
  uint8_t status = 0;    // stav mereni ve formatu daneho senzoru
  float signal = 0.0f;   // sila odrazu v MCPS
  float ambient = 0.0f;  // okolni svetlo v MCPS
};

// Najde a nastavi pripojeny senzor; false = zadny podporovany neodpovida.
bool distanceSensorBegin();
bool distanceSensorReady();
const SensorProfile &distanceSensorProfile();
// Vraci true, kdyz senzor dodal novy vzorek; jinak nechava sample beze zmeny.
bool distanceSensorPoll(DistanceSample &sample);

// Vypise na seriovou linku adresy nalezene na I2C sbernici.
void distanceSensorScanBus();
const char *distanceSensorStatusName(uint8_t status);
