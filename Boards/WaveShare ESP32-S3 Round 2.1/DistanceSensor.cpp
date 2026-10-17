#include "DistanceSensor.h"

#include <Arduino.h>
#include <VL53L0X.h>
#include <VL53L1X.h>

#include "I2C_Driver.h"

namespace {
constexpr uint8_t SENSOR_ADDRESS = 0x29;

// Rozpocet jednoho mereni rozhoduje o dosahu: cim dele senzor sbira fotony,
// tim slabsi odraz jeste rozezna od okolniho svetla. Perioda opakovani musi
// zustat delsi nez rozpocet, jinak zacne dalsi mereni driv, nez senzor
// uzavre predchozi. Oba senzory pouzivaji stejne hodnoty, at se displej
// chova stejne bez ohledu na to, co je pripojene.
constexpr uint32_t TIMING_BUDGET_US = 140000;
constexpr uint32_t MEASUREMENT_PERIOD_MS = 180;

// VL53L1X: minimalni sila odrazu, pod kterou senzor mereni zahodi.
// Registr je v pevne radove carce 9.7 (hodnota / 128 = MCPS). Vychozi 192
// (1,5 MCPS) odmita i stabilni mereni tmaveho nebo sikmeho povrchu.
constexpr uint16_t VL53L1X_SIGNAL_THRESHOLD = 32;  // 0,25 MCPS

// VL53L0X: nastaveni pro delsi dosah podle prikladu knihovny Pololu
// (nizsi prah sily odrazu, delsi laserove pulzy).
constexpr float VL53L0X_SIGNAL_LIMIT_MCPS = 0.1f;

const SensorProfile PROFILE_NONE = {SENSOR_NONE, "zadny", DEFAULT_SCALE_MAX_MM, 1.5f};
const SensorProfile PROFILE_VL53L1X = {SENSOR_VL53L1X, "VL53L1X", 4000, 1.5f};
// Prah silneho odrazu u VL53L0X je odhad - knihovna sama povazuje za platne
// mereni od 0,25 MCPS. Pokud by se silna mereni ukazovala jako "slaby odraz",
// snizte ho podle hodnoty signal v diagnostice.
const SensorProfile PROFILE_VL53L0X = {SENSOR_VL53L0X, "VL53L0X", 2000, 1.0f};

VL53L1X sensorL1;
VL53L0X sensorL0;
const SensorProfile *profile = &PROFILE_NONE;

// Cteni identifikacniho registru bez knihovny. VL53L1X ma 16bitove adresy
// registru, VL53L0X 8bitove, proto se zkousi nejdriv VL53L1X: jeho dotaz
// na VL53L0X jen zapise neskodnou hodnotu, kterou nasledna inicializace
// stejne prepise. Opacne poradi by mohlo VL53L1X zmast.
bool readIdVl53l1x(uint16_t &id) {
  Wire.beginTransmission(SENSOR_ADDRESS);
  Wire.write(0x01);  // IDENTIFICATION__MODEL_ID = 0x010F
  Wire.write(0x0F);
  if (Wire.endTransmission() != 0) return false;
  if (Wire.requestFrom(SENSOR_ADDRESS, static_cast<uint8_t>(2)) != 2) return false;
  id = static_cast<uint16_t>(Wire.read()) << 8;
  id |= Wire.read();
  return true;
}

bool readIdVl53l0x(uint8_t &id) {
  Wire.beginTransmission(SENSOR_ADDRESS);
  Wire.write(0xC0);  // IDENTIFICATION_MODEL_ID
  if (Wire.endTransmission() != 0) return false;
  if (Wire.requestFrom(SENSOR_ADDRESS, static_cast<uint8_t>(1)) != 1) return false;
  id = Wire.read();
  return true;
}

SensorModel detectModel() {
  uint16_t idL1 = 0;
  if (readIdVl53l1x(idL1) && idL1 == 0xEACC) return SENSOR_VL53L1X;
  uint8_t idL0 = 0;
  if (readIdVl53l0x(idL0) && idL0 == 0xEE) return SENSOR_VL53L0X;
  return SENSOR_NONE;
}

bool beginVl53l1x() {
  sensorL1.setBus(&Wire);
  sensorL1.setTimeout(500);
  if (!sensorL1.init()) return false;

  // Long mode zvlada az 4 m, coz odpovida rozsahu stupnice.
  sensorL1.setDistanceMode(VL53L1X::Long);
  sensorL1.setMeasurementTimingBudget(TIMING_BUDGET_US);
  // Prah se zapisuje az po init(), ktery ho vraci na vychozi hodnotu.
  sensorL1.writeReg16Bit(VL53L1X::RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS,
                         VL53L1X_SIGNAL_THRESHOLD);
  sensorL1.startContinuous(MEASUREMENT_PERIOD_MS);
  return true;
}

bool beginVl53l0x() {
  sensorL0.setBus(&Wire);
  sensorL0.setTimeout(500);
  if (!sensorL0.init()) return false;

  sensorL0.setSignalRateLimit(VL53L0X_SIGNAL_LIMIT_MCPS);
  sensorL0.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensorL0.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  sensorL0.setMeasurementTimingBudget(TIMING_BUDGET_US);
  sensorL0.startContinuous(MEASUREMENT_PERIOD_MS);
  return true;
}

bool pollVl53l1x(DistanceSample &sample) {
  if (!sensorL1.dataReady()) return false;

  // Neblokujici cteni - novy vzorek uz je k dispozici.
  const uint16_t millimeters = sensorL1.read(false);
  if (sensorL1.timeoutOccurred()) {
    sample.kind = SAMPLE_ERROR;
    sample.status = 255;
    return true;
  }

  sample.status = static_cast<uint8_t>(sensorL1.ranging_data.range_status);
  sample.mm = millimeters;
  sample.signal = sensorL1.ranging_data.peak_signal_count_rate_MCPS;
  sample.ambient = sensorL1.ranging_data.ambient_count_rate_MCPS;
  sample.kind = classifySample(verdictVl53l1x(sample.status), sample.signal,
                               profile->strongSignalMcps);
  return true;
}

bool pollVl53l0x(DistanceSample &sample) {
  if ((sensorL0.readReg(VL53L0X::RESULT_INTERRUPT_STATUS) & 0x07) == 0)
    return false;

  // Knihovna vraci jen vzdalenost; stav a silu odrazu cteme stejne jako
  // ST API (VL53L0X_GetRangingMeasurementData) z bloku 12 bajtu.
  uint8_t data[12];
  sensorL0.readMulti(VL53L0X::RESULT_RANGE_STATUS, data, sizeof(data));
  sensorL0.writeReg(VL53L0X::SYSTEM_INTERRUPT_CLEAR, 0x01);
  if (sensorL0.last_status != 0) {
    sample.kind = SAMPLE_ERROR;
    sample.status = 255;
    return true;
  }

  sample.status = static_cast<uint8_t>((data[0] & 0x78) >> 3);
  sample.mm = static_cast<uint16_t>((data[10] << 8) | data[11]);
  sample.signal = static_cast<float>((data[6] << 8) | data[7]) / 128.0f;
  sample.ambient = static_cast<float>((data[8] << 8) | data[9]) / 128.0f;
  sample.kind = classifySample(verdictVl53l0x(sample.status), sample.signal,
                               profile->strongSignalMcps);
  return true;
}
}  // namespace

void distanceSensorScanBus() {
  I2CBusGuard guard;
  Serial.print("I2C zarizeni:");
  uint8_t found = 0;
  for (uint8_t address = 0x08; address < 0x78; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf(" 0x%02X", address);
      ++found;
    }
  }
  if (found == 0) Serial.print(" zadne!");
  Serial.println();
}

bool distanceSensorBegin() {
  I2CBusGuard guard;
  profile = &PROFILE_NONE;

  const SensorModel model = detectModel();
  bool ok = false;
  if (model == SENSOR_VL53L1X) {
    profile = &PROFILE_VL53L1X;
    ok = beginVl53l1x();
  } else if (model == SENSOR_VL53L0X) {
    profile = &PROFILE_VL53L0X;
    ok = beginVl53l0x();
  }

  if (!ok) {
    Serial.println(model == SENSOR_NONE
                       ? "Senzor vzdalenosti nenalezen - zkontrolujte zapojeni I2C"
                       : "Senzor vzdalenosti se nepodarilo inicializovat");
    profile = &PROFILE_NONE;
    return false;
  }

  Serial.printf("%s pripraven, rozpocet %lu us, perioda %lu ms, stupnice %u mm\n",
                profile->name, static_cast<unsigned long>(TIMING_BUDGET_US),
                static_cast<unsigned long>(MEASUREMENT_PERIOD_MS),
                profile->scaleMaxMm);
  return true;
}

bool distanceSensorReady() { return profile->model != SENSOR_NONE; }

const SensorProfile &distanceSensorProfile() { return *profile; }

bool distanceSensorPoll(DistanceSample &sample) {
  I2CBusGuard guard;
  switch (profile->model) {
    case SENSOR_VL53L1X:
      return pollVl53l1x(sample);
    case SENSOR_VL53L0X:
      return pollVl53l0x(sample);
    default:
      return false;
  }
}

const char *distanceSensorStatusName(uint8_t status) {
  if (status == 255) return "timeout";
  if (profile->model == SENSOR_VL53L1X) {
    return VL53L1X::rangeStatusToString(static_cast<VL53L1X::RangeStatus>(status));
  }
  switch (status) {
    case 11:
      return "range valid";
    case 4:
      return "signal fail";
    case 6:
    case 9:
      return "phase fail";
    case 8:
    case 10:
      return "min range fail";
    case 1:
    case 2:
    case 3:
      return "hardware fail";
    default:
      return "no update";
  }
}
