// Ciste vypocty bez vazby na Arduino, aby sly otestovat na pocitaci
// (viz tools/test_alarm_logic.cpp).
#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Rozsah stupnice po obvodu displeje odpovida dosahu pripojeneho senzoru
// (4 m pro VL53L1X, 2 m pro VL53L0X) a nastavuje se po jeho rozpoznani.
constexpr uint16_t DISTANCE_MIN_MM = 0;
constexpr uint16_t DEFAULT_SCALE_MAX_MM = 4000;

inline uint16_t &scaleMaxStorage() {
  static uint16_t scaleMax = DEFAULT_SCALE_MAX_MM;
  return scaleMax;
}
inline uint16_t scaleMaxMm() { return scaleMaxStorage(); }
inline void setScaleMaxMm(uint16_t millimeters) {
  scaleMaxStorage() = millimeters < 1000 ? 1000 : millimeters;
}

// Stupnice zacina vlevo dole, bezi po smeru hodinovych rucicek a konci vpravo
// dole; dolnich 120 stupnu zustava volnych pro ovladaci tlacitka. 0 stupnu
// miri doprava, uhel roste po smeru hodinovych rucicek.
constexpr int16_t ARC_START_DEG = 150;
constexpr int16_t ARC_SPAN_DEG = 240;

constexpr uint16_t THRESHOLD_MIN_MM = 100;
constexpr uint16_t THRESHOLD_STEP_MM = 50;
inline uint16_t thresholdMaxMm() { return scaleMaxMm(); }

// Pasmo kolem meze, ve kterem uz alarm bezi, ale jeste nezhasne. Brani
// drnceni bzucaku, kdyz vzdalenost kolisa presne na hranici.
constexpr uint16_t ALARM_HYSTERESIS_MM = 50;

// Vysledek jednoho mereni. Cil za dosahem senzoru je vlastni stav - neni to
// chyba, jen uz vzdalenost nejde zmerit.
enum MeasurementState : uint8_t {
  MEASUREMENT_INVALID = 0,
  MEASUREMENT_VALID = 1,
  MEASUREMENT_OVER_RANGE = 2,
};

enum AlarmDirection : uint8_t {
  ALARM_WHEN_CLOSER = 0,  // spusti se pod nastavenou mezi
  ALARM_WHEN_FARTHER = 1  // spusti se nad nastavenou mezi
};

struct AlarmSettings {
  bool enabled = true;
  uint16_t thresholdMm = 500;
  AlarmDirection direction = ALARM_WHEN_CLOSER;
};

inline uint16_t clampThreshold(int32_t valueMm) {
  if (valueMm < THRESHOLD_MIN_MM) return THRESHOLD_MIN_MM;
  if (valueMm > thresholdMaxMm()) return thresholdMaxMm();
  return static_cast<uint16_t>(valueMm);
}

// Posune mez o zadany pocet kroku; vychozi hodnotu nejprve zaokrouhli na
// nejblizsi nasobek kroku, aby stupnice zustala v peknych cislech.
inline uint16_t stepThreshold(uint16_t thresholdMm, int8_t steps) {
  const int32_t aligned =
      ((static_cast<int32_t>(thresholdMm) + THRESHOLD_STEP_MM / 2) /
       THRESHOLD_STEP_MM) *
      THRESHOLD_STEP_MM;
  return clampThreshold(aligned + static_cast<int32_t>(steps) * THRESHOLD_STEP_MM);
}

inline uint16_t clampDistance(int32_t valueMm) {
  if (valueMm < DISTANCE_MIN_MM) return DISTANCE_MIN_MM;
  if (valueMm > scaleMaxMm()) return scaleMaxMm();
  return static_cast<uint16_t>(valueMm);
}

// Uhel na stupnici pro danou vzdalenost.
inline float angleForDistance(uint16_t distanceMm) {
  const float ratio =
      static_cast<float>(clampDistance(distanceMm)) / scaleMaxMm();
  return ARC_START_DEG + ratio * ARC_SPAN_DEG;
}

// Druh vzorku podle toho, jak moc mu lze verit. Rozhoduje sila odrazu
// a stav senzoru dohromady - samotny stav nestaci: pri slabem odrazu VL53L1X
// hlasi "wrap target fail" i u zcela spravne vzdalenosti, a naopak "range
// valid" obcas i u nahodneho cisla.
enum SampleKind : uint8_t {
  SAMPLE_STRONG = 0,     // platne mereni se silnym odrazem, plati hned
  SAMPLE_WEAK = 1,       // pouzitelna vzdalenost, kterou musi potvrdit dalsi
  SAMPLE_NO_TARGET = 2,  // senzor nic nevidi
  SAMPLE_ERROR = 3,      // porucha, timeout, cil prilis blizko
};

// Spolecny verdikt nad stavem mereni. Kazdy senzor ma vlastni kody stavu,
// filtr ale pracuje jen s timhle - je mu jedno, ktery senzor je pripojeny.
enum RangeVerdict : uint8_t {
  VERDICT_VALID = 0,      // senzor mereni prijal
  VERDICT_UNCERTAIN = 1,  // vzdalenost je pouzitelna, ale senzor si neni jisty
  VERDICT_NO_TARGET = 2,  // nic v dosahu nebo prilis slaby odraz
  VERDICT_ERROR = 3,      // porucha, timeout, cil prilis blizko
};

// Kody range_status knihovny VL53L1X od Pololu.
inline RangeVerdict verdictVl53l1x(uint8_t rangeStatus) {
  switch (rangeStatus) {
    case 0:  // range valid
    case 3:  // valid, min range clipped
    case 6:  // valid, wrap check not done
      return VERDICT_VALID;
    case 7:  // wrap target fail - pri slabem odrazu casto spravna hodnota
      return VERDICT_UNCERTAIN;
    case 1:  // sigma fail
    case 2:  // signal fail
    case 4:  // phase out of bounds
      return VERDICT_NO_TARGET;
    default:  // hardware fail, min range fail, timeout
      return VERDICT_ERROR;
  }
}

// Vnitrni stav mereni VL53L0X - bity 6:3 registru RESULT_RANGE_STATUS,
// vyznam podle ST API (VL53L0X_get_pal_range_status).
inline RangeVerdict verdictVl53l0x(uint8_t deviceStatus) {
  switch (deviceStatus) {
    case 11:  // range complete
      return VERDICT_VALID;
    case 4:   // signal fail
    case 6:   // phase fail
    case 9:   // phase fail
      return VERDICT_NO_TARGET;
    default:  // hardware fail (1-3), min range fail (8, 10), zadne mereni
      return VERDICT_ERROR;
  }
}

// Zarazeni vzorku. Silny odraz plati hned, slabsi musi potvrdit dalsi vzorky.
// Prah silneho odrazu zavisi na senzoru (viz SensorProfile v DistanceSensor).
inline SampleKind classifySample(RangeVerdict verdict, float signalMcps,
                                 float strongSignalMcps) {
  switch (verdict) {
    case VERDICT_VALID:
      return signalMcps >= strongSignalMcps ? SAMPLE_STRONG : SAMPLE_WEAK;
    case VERDICT_UNCERTAIN:
      return SAMPLE_WEAK;
    case VERDICT_NO_TARGET:
      return SAMPLE_NO_TARGET;
    default:
      return SAMPLE_ERROR;
  }
}

// Filtr mereni. Slabe vzorky se nehodnoti jednotlive, ale podle toho, zda
// na sebe navazuji: skutecny cil dava retez hodnot blizko sebe (i kdyz se
// pohybuje), sum senzoru skace o stovky milimetru. Silny vzorek plati hned.
class DistanceFilter {
 public:
  // Kolik navazujicich slabych vzorku musi prijit, nez jim uverime.
  static constexpr uint8_t CHAIN_LENGTH = 3;
  // Nejvetsi rozdil dvou po sobe jdoucich vzorku retezu. Pokryva i cil,
  // ktery se pohybuje - pri 5 merenich za sekundu je to 0,75 m/s. Vic by
  // propoustelo nahodne retezy z rozptylenych vzorku pri slabem odrazu.
  static constexpr uint16_t CHAIN_STEP_MM = 150;
  // Retez se prerusi, kdyz mezi jeho vzorky uplyne vic casu.
  static constexpr uint32_t CHAIN_GAP_MS = 700;
  // Jak dlouho drzime posledni hodnotu bez potvrzeni, nez priznáme, ze uz
  // nic nemerime. Doba, ne pocet vzorku: na rychlosti mereni to nezavisi.
  static constexpr uint32_t HOLD_MS = 1200;

  // Zalozni cesta pro velmi slaby odraz: jednotlive vzorky se rozptyluji
  // o +-30 cm a retez nevznikne, jejich median je ale stabilni. Sum bez cile
  // se rozptyluje mnohem vic (odchylka 35-65 cm), takze se da rozlisit.
  static constexpr uint8_t WINDOW_SIZE = 7;
  static constexpr uint8_t WINDOW_MIN_SAMPLES = 5;
  static constexpr uint32_t WINDOW_MS = 2000;
  // Nejvetsi median absolutnich odchylek okna, ktery jeste bereme jako cil.
  static constexpr uint16_t WINDOW_MAX_SPREAD_MM = 300;

  void update(SampleKind kind, uint16_t sampleMm, uint32_t nowMs) {
    if (kind == SAMPLE_STRONG) {
      chainCount_ = 1;
      chainLastMm_ = sampleMm;
      chainLastMs_ = nowMs;
      pushWindow(sampleMm, nowMs);
      weakSignal_ = false;
      accept(sampleMm, nowMs, SMOOTHING_ALPHA, JUMP_THRESHOLD_MM);
      return;
    }

    if (kind == SAMPLE_WEAK) {
      const bool continues =
          chainCount_ > 0 && nowMs - chainLastMs_ <= CHAIN_GAP_MS &&
          absDiff(sampleMm, chainLastMm_) <= CHAIN_STEP_MM;
      chainCount_ = continues ? static_cast<uint8_t>(chainCount_ < 255 ? chainCount_ + 1 : 255) : 1;
      chainLastMm_ = sampleMm;
      chainLastMs_ = nowMs;
      chainRecent_[2] = continues ? chainRecent_[1] : sampleMm;
      chainRecent_[1] = continues ? chainRecent_[0] : sampleMm;
      chainRecent_[0] = sampleMm;
      pushWindow(sampleMm, nowMs);

      if (chainCount_ >= CHAIN_LENGTH) {
        // Median poslednich tri vzorku retezu - jeden ulet na jeho konci tak
        // hodnotu nestrhne a pohyb cile se zpozdi nejvys o jedno mereni.
        weakSignal_ = false;
        accept(median3(chainRecent_[0], chainRecent_[1], chainRecent_[2]), nowMs,
               SMOOTHING_ALPHA, JUMP_THRESHOLD_MM);
        return;
      }

      // Median prijmeme jen kdyz ho podporuje i nejnovejsi vzorek. Jinak by
      // starsi spravne vzorky v okne drzely hodnotu i po ztrate cile.
      uint16_t median = 0;
      if (windowMedian(nowMs, median) &&
          absDiff(sampleMm, median) <= WINDOW_MAX_SPREAD_MM) {
        // Median z rozptylenych vzorku se pohybuje o par centimetru i bez
        // pohybu cile - vyhlazujeme ho vic a skoky neprebirame naraz.
        weakSignal_ = true;
        accept(median, nowMs, WEAK_SMOOTHING_ALPHA, WEAK_JUMP_THRESHOLD_MM);
        return;
      }
      holdOrExpire(nowMs);
      return;
    }

    // Nevidi-li senzor nic, retez nerusime: u slabeho odrazu se takove vzorky
    // mezi spravna mereni obcas vmisi. Retez prerusi az casova mezera.
    lastFailure_ = kind == SAMPLE_NO_TARGET ? MEASUREMENT_OVER_RANGE
                                            : MEASUREMENT_INVALID;
    holdOrExpire(nowMs);
  }

  void reset() {
    state_ = MEASUREMENT_INVALID;
    lastFailure_ = MEASUREMENT_INVALID;
    lastAcceptMs_ = 0;
    chainCount_ = 0;
    chainLastMm_ = 0;
    chainLastMs_ = 0;
    chainRecent_[0] = chainRecent_[1] = chainRecent_[2] = 0;
    otherFailureStreak_ = 0;
    smoothed_ = 0.0f;
    windowCount_ = 0;
    windowNext_ = 0;
    weakSignal_ = false;
  }

  MeasurementState state() const { return state_; }
  bool valid() const { return state_ == MEASUREMENT_VALID; }
  // Hodnota je odhad z mediánu rozptylenych vzorku - mene presna, displej to
  // ma dat najevo.
  bool weakSignal() const { return state_ == MEASUREMENT_VALID && weakSignal_; }
  // Za dosahem hlasime konec stupnice, at ma alarm "dal nez" s cim pracovat.
  uint16_t distanceMm() const {
    if (state_ == MEASUREMENT_OVER_RANGE) return scaleMaxMm();
    return state_ == MEASUREMENT_VALID
               ? clampDistance(static_cast<int32_t>(smoothed_ + 0.5f))
               : 0;
  }

 private:
  static constexpr float SMOOTHING_ALPHA = 0.35f;
  // Vetsi zmena nez tahle se prevezme rovnou - uz ji potvrdil retez nebo
  // silny vzorek, vyhlazovani by ji jen zbytecne zpozdilo.
  static constexpr float JUMP_THRESHOLD_MM = 150.0f;
  static constexpr float WEAK_SMOOTHING_ALPHA = 0.25f;
  static constexpr float WEAK_JUMP_THRESHOLD_MM = 500.0f;
  // Kolikrat se musi zopakovat jiny druh selhani, nez prepiseme nadpis.
  static constexpr uint8_t FAILURE_KIND_TOLERANCE = 3;

  static uint16_t absDiff(uint16_t a, uint16_t b) { return a > b ? a - b : b - a; }

  static uint16_t median3(uint16_t a, uint16_t b, uint16_t c) {
    if (a > b) { const uint16_t t = a; a = b; b = t; }
    if (b > c) { const uint16_t t = b; b = c; c = t; }
    return a > b ? a : b;
  }

  void accept(uint16_t sampleMm, uint32_t nowMs, float alpha, float jumpMm) {
    const float sample = static_cast<float>(clampDistance(sampleMm));
    lastAcceptMs_ = nowMs;
    otherFailureStreak_ = 0;
    if (state_ != MEASUREMENT_VALID || fabsf(sample - smoothed_) > jumpMm) {
      smoothed_ = sample;
      state_ = MEASUREMENT_VALID;
      return;
    }
    smoothed_ += alpha * (sample - smoothed_);
  }

  void pushWindow(uint16_t sampleMm, uint32_t nowMs) {
    windowMm_[windowNext_] = sampleMm;
    windowMs_[windowNext_] = nowMs;
    windowNext_ = static_cast<uint8_t>((windowNext_ + 1) % WINDOW_SIZE);
    if (windowCount_ < WINDOW_SIZE) ++windowCount_;
  }

  static void sortValues(uint16_t *values, uint8_t count) {
    for (uint8_t i = 1; i < count; ++i) {
      const uint16_t value = values[i];
      uint8_t j = i;
      while (j > 0 && values[j - 1] > value) {
        values[j] = values[j - 1];
        --j;
      }
      values[j] = value;
    }
  }

  // Median nedavnych pouzitelnych vzorku, pokud je jejich rozptyl dost maly.
  bool windowMedian(uint32_t nowMs, uint16_t &median) const {
    uint16_t values[WINDOW_SIZE];
    uint8_t count = 0;
    for (uint8_t i = 0; i < windowCount_; ++i) {
      if (nowMs - windowMs_[i] <= WINDOW_MS) values[count++] = windowMm_[i];
    }
    if (count < WINDOW_MIN_SAMPLES) return false;

    sortValues(values, count);
    median = values[count / 2];

    uint16_t deviations[WINDOW_SIZE];
    for (uint8_t i = 0; i < count; ++i) deviations[i] = absDiff(values[i], median);
    sortValues(deviations, count);
    return deviations[count / 2] <= WINDOW_MAX_SPREAD_MM;
  }

  void holdOrExpire(uint32_t nowMs) {
    if (state_ == MEASUREMENT_VALID) {
      // Hodnotu chvili drzime - kratka serie nepotvrzenych vzorku displejem
      // nehne.
      if (nowMs - lastAcceptMs_ < HOLD_MS) return;
      state_ = lastFailure_;
      otherFailureStreak_ = 0;
      return;
    }
    // Uz nemerime. Druh selhani se casto vzorek od vzorku stridá, nadpis by
    // proto blikal mezi "mimo dosah" a pomlckami - prepneme az kdyz se novy
    // druh zopakuje.
    if (lastFailure_ == state_) {
      otherFailureStreak_ = 0;
    } else if (++otherFailureStreak_ >= FAILURE_KIND_TOLERANCE) {
      state_ = lastFailure_;
      otherFailureStreak_ = 0;
    }
  }

  MeasurementState state_ = MEASUREMENT_INVALID;
  MeasurementState lastFailure_ = MEASUREMENT_INVALID;
  uint32_t lastAcceptMs_ = 0;
  uint8_t chainCount_ = 0;
  uint16_t chainLastMm_ = 0;
  uint32_t chainLastMs_ = 0;
  uint16_t chainRecent_[3] = {};
  uint8_t otherFailureStreak_ = 0;
  float smoothed_ = 0.0f;
  uint16_t windowMm_[WINDOW_SIZE] = {};
  uint32_t windowMs_[WINDOW_SIZE] = {};
  uint8_t windowCount_ = 0;
  uint8_t windowNext_ = 0;
  bool weakSignal_ = false;
};

// Vyhodnoceni alarmu vcetne hystereze; vraci, zda ma bzucak pipat.
class AlarmEvaluator {
 public:
  bool update(bool measurementValid, uint16_t distanceMm,
              const AlarmSettings &settings) {
    if (!settings.enabled || !measurementValid) {
      triggered_ = false;
      return false;
    }

    const int32_t distance = distanceMm;
    const int32_t threshold = settings.thresholdMm;
    const int32_t release = settings.direction == ALARM_WHEN_CLOSER
                                ? threshold + ALARM_HYSTERESIS_MM
                                : threshold - ALARM_HYSTERESIS_MM;

    if (settings.direction == ALARM_WHEN_CLOSER) {
      triggered_ = triggered_ ? distance <= release : distance <= threshold;
    } else {
      triggered_ = triggered_ ? distance >= release : distance >= threshold;
    }
    return triggered_;
  }

  bool triggered() const { return triggered_; }
  void reset() { triggered_ = false; }

 private:
  bool triggered_ = false;
};

// Vzdalenost v metrech s desetinnou carkou, napriklad "1,24".
inline void formatMeters(uint16_t distanceMm, char *buffer, size_t size) {
  const uint16_t clamped = clampDistance(distanceMm);
  snprintf(buffer, size, "%u,%02u", clamped / 1000u, (clamped % 1000u) / 10u);
}
