// ****************************************************************************
// Deska: ESP32C3 Dev Module
// USB CDC On Boot : Disabled
// Flash mode      : QIO
// Partition Scheme: Huge
// ----------------------------------------------------------------------------
// Model    : ESP32-C3
// Revision : 4
// Cores    : 1
// Freq MHz : 160
// Flash kB : 4194
// SDK ver. : v5.5.4
// ----------------------------------------------------------------------------
// Port     :  115200Bd
// ****************************************************************************

#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <RTClib.h> // Knihovna RTC
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUDP.h>
#include <Wire.h> // I2C
#include <Preferences.h>

#include "Kalendar_iso.cfg"

#define VBAT_RATIO 1.769388 // 1MOhm + 1.3MOhm
#define PIN_ADC      0
#define PIN_RxD      1  // Puvodne IO (J5)
#define PIN_SPI_MISO 2  // SPI
#define PIN_SPI_CS   3  // SPI
#define PIN_ON       4  // Napajeni periferii
#define PIN_BUTTON   5  // Press = Low
#define PIN_SPI_SCK  6  // SPI
#define PIN_SPI_MOSI 7  // SPI
#define PIN_LED      9  // RGB Led
#define PIN_TxD     10  // Puvodne DS18B20 (J3)
#define PIN_I2C_SCL 18  // I2C
#define PIN_I2C_SDA 19  // I2C


RTC_DS3231 rtc;
WiFiUDP udp;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN_LED, NEO_GRB + NEO_KHZ800);
Preferences preferences;

#define Log(...) Serial.print(__VA_ARGS__)
#define Logln(...) Serial.println(__VA_ARGS__)
#define Logf(...) Serial.printf(__VA_ARGS__)

#define Disp Serial1
#define Dis(...) Disp.print(__VA_ARGS__)
#define Disf(...) Disp.printf(__VA_ARGS__)

// ----------------------------------------------------------------------------
const char* ssid = "ssid";
const char* pass = "pass";
IPAddress ipAddr(192, 168, 0, 50);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(192, 168, 0, 1);
IPAddress dns2(1, 1, 1, 1);
const uint16_t ipPort = 54321;

// --- Tmep.cz ---
const char *TMEP1_HOST = "id1.tmep.cz";
const char *TMEP2_HOST = "id2.tmep.cz";
const uint16_t TMEP_PORT = 80;
const char *url = "/?temp=";

// --- OpenWeatherMap ---
const char *weatherUrl =
    "https://api.openweathermap.org/data/2.5/forecast?"
    "lat=50.0453044&lon=14.3039500"
    "&appid=...."
    "&cnt=10&units=metric&lang=cz";

// ----------------------------------------------------------------------------
char sLog[128];
bool bRtcOk = false;         // RTC System OK
bool bSdOk = false;          // SD System OK
bool bWifiOk = false;        // WiFi OK
bool bNtpOk = false;
bool bGraphRefresh = false;

// --- Preference ---
bool bDisplayOn = false;
bool bDisplayLog = false;
bool bSdOn = true;
bool bSdLog = false;
bool bSensLog = false;
bool bSensFilter = false;
bool bTmepOn = false;
bool bTmepLog = false;
bool bUdpLog = false;
bool bWeatherOn = false;
bool bWeatherLog = false;

long lTmin = 0, lTmax = 200, lTshift = 50, lTval = 0;
long lHmin = 0, lHmax = 1000, lHshift = 250, lHval = 0;
long lPmin = 9800, lPmax = 10200, lPshift = 100, lPval = 0;

#define NX_BUF 128
uint8_t nxBuf[NX_BUF];
uint8_t nxIndex = 0;
uint8_t nxCount = 0;

int iRok = 0, iMes = 0, iDen = 0, iHod = 0, iMin = 0, iSec = 0;
int idPocasi = 0;

// ----------------------------------------------------------------------------
void SystemInfo()
{
  Logln("\n\n--- ESP32-C3 ---");
  Logln("System info");
  Log(" Model    : ");Logln(ESP.getChipModel());
  Log(" Revision : ");Logln(ESP.getChipRevision());
  Log(" Cores    : ");Logln(ESP.getChipCores());
  Log(" Freq MHz : ");Logln(ESP.getCpuFreqMHz());
  Log(" Flash kB : ");Logln(ESP.getFlashChipSize() / 1000);
  Log(" Sketch kB: ");Logln(ESP.getSketchSize() / 1000);
  Log(" SkFree kB: ");Logln(ESP.getFreeSketchSpace() / 1000);
  Log(" SDK ver. : ");Logln(ESP.getSdkVersion());
  Log(" Chip ID  : ");Logln((uint32_t)ESP.getEfuseMac(), HEX);
  Logf(" Compiled : %s %s\n", __DATE__, __TIME__);
}

// ----------------------------------------------------------------------------
const int VMAX = 418; // 4.18 V
const int VMIN = 300; // 3.00 V
float     fBatVolt = 0.0f;
int       iBatPct = 0;
uint32_t  iBatSleep = 0;

void VBatRead(bool init = false)
{
  const int N = 16;
  uint32_t sum = 0;
  for (int i = 0; i < N; i++)
  {
    sum += analogReadMilliVolts(PIN_ADC);
    delay(2);
  }
  fBatVolt = (((sum / N)) * VBAT_RATIO) / 1000.0f;
  long v = lroundf(fBatVolt * 100.0f);
  // long v = (long) fBatVolt * 100.0f;
  int  p = map( v,VMIN,VMAX,0,100 );
  iBatPct = constrain(p, 0, 100);
}

// ----------------------------------------------------------------------------
uint32_t iRgbTime = 0, msRgb = 0;

void LedRGB(bool init = false)
{
  if (init)
  {
    pixels.begin();
    pixels.setBrightness(2); // Puvodne 10
    pixels.show();
    return;
  }
  if (iRgbTime == 0)
    return;

  if (millis() - msRgb > iRgbTime)
  {
    iRgbTime = 0;
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
  }
}

void LedRGB(uint8_t r, uint8_t g, uint8_t b, uint32_t t = 0)
{
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.show();
  if (t > 0)
  {
    iRgbTime = t;
    msRgb = millis();
  }
}

// ----------------------------------------------------------------------------
void SdAppend(const char *path, const char *message)
{
  File file = SD.open(path, FILE_APPEND);
  if (!file)
  {
    Logf("SD Chyba otevreni souboru: '%s'\n", path);
    return;
  }
  if (!file.print(message))
    Logf("SD Chyba zapisu do souboru '%s'\n", path);
  file.close();
}

void SdLog(const char *message) // --- Zapis logu ---
{
  char path[20];
  bool timeok = false;
  struct tm ti;

  if (time(nullptr) > 1000000)
  {
    time_t now = time(NULL);
    struct tm ti;
    localtime_r(&now, &ti);

    snprintf(path, sizeof(path), "/%04d%02d%02d.LOG", ti.tm_year + 1900,
             ti.tm_mon + 1, ti.tm_mday);
    timeok = true;
  }
  else
  {
    strcpy(path, "/SYSTEM.LOG");
  }

  File file = SD.open(path, FILE_APPEND);
  if (!file)
  {
    Logf("SD Chyba pristupu k souboru: '%s'\n", path);
    return;
  }

  if (file.size() > 1000000)
  {
    Logln("SD Zapis prekrocil 1 MB");
  }
  else
  {
    char line[32];
    if (timeok)
      sprintf(line, "%02d:%02d:%02d - ", ti.tm_hour, ti.tm_min, ti.tm_sec);
    else
      sprintf(line, "%lu - ", millis() / 1000);

    // --- Zapsat cas ---
    if (!file.print(line))
      Logln("SD: chyba zapisu");
    else
    {
      file.print(message);
      file.print('\n');
    }
  }
  file.close();
}

void SdSize() // --- Velikost karty ---
{
  const char *unit;
  uint32_t val;
  uint64_t s = SD.cardSize() / (1024 * 1024);
  uint64_t t = SD.totalBytes() / (1024 * 1024);
  uint64_t u = SD.usedBytes();

  Log("Velikost: ");
  Log(s);
  Log(" MB, total: ");
  Log(t);
  Log(" MB, pouzito: ");
  if (u < 1000)
  {
    val = u;
    unit = " B";
  }
  else if (u < 1000000)
  {
    val = u / 1000;
    unit = " kB";
  }
  else
  {
    val = u / 1000000;
    unit = " MB";
  }
  Log(val);
  Logln(unit);
}

bool SdReinit()
{
  Logln("SD Reinicializace");
  SD.end();
  SPI.end();
  delay(50);
  SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_CS);
  if (!SD.begin(PIN_SPI_CS, SPI, 1000000))
  {
    bSdOk = bSdOn = false;
    return false;
  }
  bSdOk = true;
  SdSize();
  return true;
}

bool SdInit(bool init = false) // --- Inicializace SD Karty ---
{
  Log("SD ");
  bSdOk = false;
  if (!bSdOn) // preference
  {
    Logln("Neni zapnuta");
    return false;
  }

  if (init)
  {
    if (!SD.begin(PIN_SPI_CS, SPI, 1000000))
    {
      Logln("Chyba inicializace");
      return false;
    }
  }

  uint8_t type = SD.cardType();
  if (type == CARD_NONE)
  {
    Logln("Neni vlozena");
    return false;
  }

  File f = SD.open("/"); // --- Rychly test ---
  if (!f)
  {
    Logln("Chyba FS");
    return false;
  }
  f.close();

  SdSize();

  if (init) // --- Test zapisem ---
  {
    File f = SD.open("/test.txt", FILE_WRITE);
    if (!f)
    {
      Logln("SD Chyba zapisu");
      return false;
    }

    f.print("SD Test zapisu/cteni");
    f.close();
    f = SD.open("/test.txt");
    if (!f)
    {
      Logln("SD Chyba testu w/r");
      return false;
    }
    while (f.available())
      Serial.write(f.read());
    f.close();
    Logln(" OK");
    
  }
  bSdOk = bSdOn;
  return true;
}

// ----------------------------------------------------------------------------
void RtcToSystemTime()
{
  bRtcOk = false;
  if (!rtc.begin())
  {
    Logln("RTC Modul nenalezen");
    return;
  }

  if (rtc.lostPower())
  {
    Logln("RTC Ztrata napajeni");
    // return;
  }

  // rtc.adjust(DateTime(time(NULL)));

  time_t now = time(NULL);
  struct tm loc;
  localtime_r(&now, &loc);
  // Logf("CPU %02d.%02d.%04d %02d:%02d:%02d\n",
  //      loc.tm_mday, loc.tm_mon + 1, loc.tm_year + 1900, loc.tm_hour, loc.tm_min, loc.tm_sec);

  if (loc.tm_year + 1900 <= 2020)
  {
    DateTime dtrtc = rtc.now(); // toto je UTC !
    if (dtrtc.year() <= 1970) return;
    // Logln("RTC Nastaveni casu CPU");
    struct timeval tv;
    tv.tv_sec = dtrtc.unixtime();
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);

    now = time(NULL);
    localtime_r(&now, &loc);
    Logf("CPU %02d.%02d.%04d %02d:%02d:%02d - Nastaveni z RTC \n", loc.tm_mday,
         loc.tm_mon + 1, loc.tm_year + 1900, loc.tm_hour, loc.tm_min,
         loc.tm_sec);
  }
  bRtcOk = true;
}

void RtcAdjust()
{
  // if (!bNtpOk) return;

  time_t sys = time(NULL);
  DateTime r = rtc.now();
  time_t rtcTime = r.unixtime();
  long diff = llabs(sys - rtcTime);
  if (diff > 3) // limit
  {
    char msg[48];
    snprintf(msg, sizeof(msg), "RTC Synchronizace casu, dif = %ld sec", diff);
    Logln(msg);

    // rtc.adjust(DateTime(sys));

    struct tm utc;
    gmtime_r(&sys, &utc);

    rtc.adjust(DateTime(
      utc.tm_year + 1900, 
      utc.tm_mon + 1, 
      utc.tm_mday,
      utc.tm_hour, 
      utc.tm_min, 
      utc.tm_sec
    ));

    DateTime r = rtc.now();
    Logf("RTC %02d.%02d.%04d %02d:%02d:%02d - Korekce z NTP\n", 
      r.day(),
      r.month(), 
      r.year(), 
      r.hour(), 
      r.minute(), 
      r.second()
    );
  }
}

// ----------------------------------------------------------------------------
struct structTempSens // --- Struktura dat ze senzoru ---
{
  const char *id;       // ID v systemu
  const char *unit;     // Merna jednotka
  const float filtr;    // Filtracni konstanta
  const float corr_off; // Korekce teploty u lokalniho cidla
  const float corr_mul; // Korekce teploty u lokalniho cidla
  float value;          // Zmerena teplota
  float valueF;         // Po filtraci
  uint32_t lastms;      // Posledni mereni
  int8_t change;        // Zmena 0:bez, 1:stoupa, -1: klesa
  bool connect;         // Pripojeno
  bool valid;           // Korektni hodnota
  bool updated;         // Mame novou hodnotu
  bool init;            // Inicializace
};

structTempSens Sensor[] = // Tabulka sensoru
{
  //ID    Unit   Flt  Off  Mul  Val  VlF  ms ch conn  upd   val   init
  {"B01", "Vdc", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"A01", "%  ", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"R01", "dBm", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"T11", "°C ", 0.2, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"T12", "°C ", 0.2, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"H11", "%  ", 0.2, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"P11", "hPa", 0.2, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"R11", "dBm", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"B11", "Vcc", 0.2, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"A11", "%  ", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"S11", "Min", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"T21", "°C ", 0.2, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"H21", "%  ", 0.2, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"R21", "dBm", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"B21", "V  ", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"A21", "%  ", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
  {"S21", "Sec", 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0, false,false,false,false},
};
const size_t SENS_NUM = sizeof(Sensor) / sizeof(Sensor[0]);

int SensCheckId(const char *id)
{
  for (uint8_t i = 0; i < SENS_NUM; i++)
  {
    if (memcmp(id, Sensor[i].id, 3) != 0) continue;
    return i;
  }
  return -1;
}

bool SensLocalWrite(const char *id, float val)
{
  for (uint8_t i = 0; i < SENS_NUM; i++)
  {
    if (memcmp(id, Sensor[i].id, 3) != 0) continue;
    Sensor[i].value = val;
    Sensor[i].valid = true;
    Sensor[i].connect = true;
    Sensor[i].lastms = millis();
    return true;
  }
  return false;
}

void SensorLocalScan()
{
  static uint32_t msSens = 0;
  uint32_t ms = millis();
  if (ms - msSens < 15000)
    return;
  msSens = ms;
  int id;

  // --- Intenzita WiFi signalu ---
  int8_t rssi = WiFi.RSSI();
  SensLocalWrite("R01", (float)rssi);

  // --- Provozni napeti ---
  VBatRead();
  SensLocalWrite("B01", fBatVolt);
  SensLocalWrite("A01", (float)iBatPct);
}

// ----------------------------------------------------------------------------
void SensFilter(uint8_t i) // --- FILTR SENSORU ---
{
  if (!Sensor[i].valid)
  {
    Sensor[i].init = false;
    Sensor[i].change = 0;
    return;
  }

  // --- Bez filtrace ---
  float A = Sensor[i].filtr;
  if (!Sensor[i].init || !bSensFilter || A <= 0.0 || A >= 1.0)
  {
    Sensor[i].valueF = Sensor[i].value;
    Sensor[i].init = true;
    Sensor[i].change = 0;
    return;
  }

  float fn = A * Sensor[i].value + (1.0 - A) * Sensor[i].valueF;
  // float fn = Sensor[i].valueF * (1.0f - A) + Sensor[i].value * A;

  float ch = Sensor[i].valueF - fn;
  Sensor[i].valueF = fn;
}

// ----------------------------------------------------------------------------
void SensorList(bool now = false)
{
  static uint32_t msList = 0;
  uint32_t ms = millis();
  if (ms - msList < 60000)
    return;
  msList = ms;

  if (iBatPct > 70)
    LedRGB(0, 255, 0, 20);
  else if (iBatPct > 30)
    LedRGB(0, 0, 255, 20);
  else
    LedRGB(255, 0, 0, 50);

  bool any = false;
  for (uint8_t i = 0; i < SENS_NUM; i++)
  {
    if (!Sensor[i].valid)
      continue;
    if (ms - Sensor[i].lastms > 600000)
    {
      Sensor[i].connect = false;
      Sensor[i].valid = false;
      Sensor[i].init = false;
      continue;
    }
    any = true;
  }
  if (!any)
    return;

  // --- Vypis sensoru ---
  for (uint8_t i = 0; i < SENS_NUM; i++)
  {
    structTempSens &s = Sensor[i];
    if (!s.valid)
      continue;
    if (!s.init)
    {
      s.valueF = s.value;
      s.init = true;
    }

    if (s.filtr > 0.0 && s.filtr < 1.0 && bSensFilter)
      SensFilter(i);
    else
      s.valueF = s.value;

    // --- Vypis ---
    if (bSensLog)
    {
      Log("#");
      Log(s.id);
      Log(": ");
      if (fabs(s.value) < 1000.0)
      {
        Log(" ");
      }
      if (fabs(s.value) < 100.0)
      {
        Log(" ");
      }
      if (s.value >= 0.0 && s.value < 10.0)
      {
        Log(" ");
      }
      Log(s.value, 2);
      Log(" ");
      Log(s.unit);

      if (s.filtr > 0.0 && bSensFilter) // --- Vypis filtrovanych ---
      {
        if (fabs(s.valueF - s.value) > 0.02)
        {
          Log(" *");
        }
        Log("\t");
        if (fabs(s.valueF) < 1000.0)
        {
          Log(" ");
        }
        if (fabs(s.valueF) < 100.0)
        {
          Log(" ");
        }
        if (s.valueF >= 0.0 && s.valueF < 10.0)
        {
          Log(" ");
        }
        Log(s.valueF, 2);
        Log(" ");
        Log(s.unit);
        if (s.change > 0)
        {
          Log(" [UP]");
        }
        else if (s.change < 0)
        {
          Log(" [DN]");
        }
      }

      Logln();
    }
  }
  // if (bSensLog) { Logln("---"); }
}

// ----------------------------------------------------------------------------
bool NtpGetTime(bool init = false)
{
  static bool synchro = false;
  time_t now;
  // --- Periodicka kontrola ---
  if (!init)
  {
    now = time(NULL);
    struct tm loc;
    localtime_r(&now, &loc);
    if (loc.tm_hour != 4)
    {
      synchro = false;
      return false;
    }
    synchro = true;
  }

  now = time(nullptr); // Vzdy UTC
  int retry = 0;
  while (now < 1000000000 && retry < 50)
  {
    delay(100);
    now = time(nullptr);
    retry++;
  }

  static bool ntperr = false;
  struct tm ti;
  if (!getLocalTime(&ti))
  {
    if (!ntperr)
    {
      ntperr = true;
      const char *msg = "NTP Chyba cteni";
      Logln(msg);
    }
    return false;
  }
  else if (ntperr)
  {
    ntperr = false;
    const char *msg = "NTP Obnova cteni";
    Logln(msg);
  }

  Logf(" NTP : %02d.%02d.%04d %02d:%02d:%02d\n", ti.tm_mday, ti.tm_mon + 1,
       ti.tm_year + 1900, ti.tm_hour, ti.tm_min, ti.tm_sec);

  now = time(NULL);
  struct tm loc;
  localtime_r(&now, &loc);

  static int iMin = -1;
  if (iMin != loc.tm_min)
  {
    iMin = loc.tm_min;
    RtcAdjust();
  }
  return true;
}

// ----------------------------------------------------------------------------
void UdpStart()
{
  uint16_t port = ipPort;
  bool udpOn = udp.begin(port);

  if (udpOn)
  {
    Log(" Port: ");
    Logln(port);
  }
  else
  {
    Logln("UDP Chyba startu");
  }
}

void UdpResponse(const char *s)
{
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write((const uint8_t *)s, strlen(s));
  udp.endPacket();

  if (bUdpLog)
  {
    Log("UDP ");
    Log(udp.remoteIP());
    // Log(":");
    // Log(udp.remotePort());
    Log(" <- ");
    Logln(s);
  }
}

bool UdpParse() // --- UDP PARSE ---
{
  int psize = udp.parsePacket();
  if (psize <= 0)
    return false;

  char udpRxBuf[256] = "";
  int n = udp.read(udpRxBuf, sizeof(udpRxBuf) - 1);
  if (n <= 0)
    return false;

  udpRxBuf[n] = '\0';

  if (bUdpLog)
  {
    Log("UDP ");
    Log(udp.remoteIP());
    Log(" -> ");
    Logln(udpRxBuf);
  }
  bool ackOn = false;
  bool any = false;

  for (char *p = udpRxBuf; *p;)
  {
    while (*p && *p != '!')
      ++p; // --- Na zacatek ---
    if (!*p)
      break;
    ++p; // za '!'

    char *end = strchr(p, '$'); // --- Hledat konec ---
    if (!end)
      break;
    *end = 0;

    // char* comma = strchr(p,','); // --- Hledat oddelovac ---
    char *comma = strpbrk(p, ":,"); // --- Hledat ':' nebo ',' ---
    if (comma)
    {
      *comma = 0;
      const char *id = p;
      const char *val = comma + 1;

      // --- Vyhledat sensor ---
      for (uint8_t i = 0; i < SENS_NUM; i++)
      {
        if (memcmp(id, Sensor[i].id, 3) != 0)
          continue; // Hledat ID

        Sensor[i].connect = true;
        Sensor[i].lastms = millis();

        // --- Ciselna hodnata ---
        if (isdigit((unsigned char)val[0]) || val[0] == '-' || val[0] == '+')
        {
          float f = atof(val);
          if (Sensor[i].corr_off != 0.0)
            f += Sensor[i].corr_off;
          if (Sensor[i].corr_mul != 1.0)
            f *= Sensor[i].corr_mul;
          Sensor[i].value = f;
          if (!Sensor[i].init || !bSensFilter || Sensor[i].filtr <= 0.0 ||
              Sensor[i].filtr >= 1.0)
          {
            Sensor[i].valueF = f;
            Sensor[i].init = true;
            Sensor[i].change = 0;
          }
          any = true;
          Sensor[i].valid = true;
        }
        // --- Textova hodnota ---
        else
        {
          // if (strcmp(val, "DISC") == 0) // DISC|LOW|HIGH
          Sensor[i].valid = false;
          Sensor[i].init = false;
        }
        break;
      }
    }
    else
    {
      if (strcmp(p, "ACK") == 0)
        return false;
      if (strcmp(p, "NODATA") == 0)
        any = true;
      else
        return false;
    }

    *end = '$'; // volitelné obnovit
    p = end + 1;
  }

  UdpResponse("!ACK$");
  return any;
}

// ----------------------------------------------------------------------------
bool WiFiConnect(bool init = false)
{
  if (init) // Jen v setupu
  {
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    WiFi.disconnect(false);
    delay(100);

    if (!WiFi.config(ipAddr, gateway, subnet, dns1, dns2))
    {
      Logln("WiFi Chyba pevne IPadresy");
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    bWifiOk = true;
    return true;
  }

  if (!init)
  {
    bWifiOk = false;
    static uint32_t wifiTimer = 0;
    if (millis() - wifiTimer < 30000)
      return false;
    wifiTimer = millis();

    Logln("WiFi Obnova pripojeni");
    WiFi.disconnect(false);
    delay(100);

    WiFi.begin(ssid, pass);
  }
  else
  {
    Log("WiFi Pripojovani");
    WiFi.begin(ssid, pass);
  }

  uint32_t t = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t < 10000)
  {
    delay(500);
    Log(".");
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    bWifiOk = false;
    Log("\n- Chyba pripojeni k ");
    Log(ssid);
    Log(" #");
    Logln(WiFi.status());
    return false;
  }
  bWifiOk = true;
  Logln(" OK");
  Log(" RSSI: ");Log(WiFi.RSSI());Logln(" dBm");
  Log(" MAC : ");Logln(WiFi.macAddress());
  Log(" IP  : ");Logln(WiFi.localIP());

  UdpStart();
  bNtpOk = NtpGetTime(true);
  return true;
}

// ----------------------------------------------------------------------------
struct stTmep
{
  const char *id;
  const char *GUID;
};

stTmep Tmep1[] = 
{
  {"T11", "Temp11"},  // Teplota
  {"H11", "Humi11"},  // Vlhkost
  {"P11", "Pres11"},  // Tlak
  {"B11", "voltage"}, // Napeti baterie
  {"R11", "rssi"},    // Uroven wifi
};
const size_t TMEP1_NUM = sizeof(Tmep1) / sizeof(Tmep1[0]);

stTmep Tmep2[] = 
{
  {"T21", "Temp21"},  // Teplota
  {"H21", "Humi21"},  // Vlhkost
  {"B21", "voltage"}, // Napeti baterie
  {"R21", "rssi"},    // Uroven wifi
};
const size_t TMEP2_NUM = sizeof(Tmep2) / sizeof(Tmep2[0]);

bool TmepSend(const char *host,const struct stTmep *tmep,size_t tmep_num,const char *txt = nullptr)
{
  static bool tmep_ok = true;
  char line[128] = "";
  size_t l = 0;
  bool send = false;

  for (size_t i = 0; i < tmep_num; i++)
  {
    for (size_t j = 0; j < SENS_NUM; j++)
    {
      if (strncmp(tmep[i].id, Sensor[j].id, 3) == 0)
      {
        if (Sensor[j].valid)
        {
          float f = (Sensor[j].init) ? Sensor[j].valueF : Sensor[j].value;
          l += snprintf(line + l, sizeof(line) - l, "%s%s=%.2f",
                        send ? "&" : "", tmep[i].GUID, f);
          send = true;
        }
      }
    }
    if (i == 0 && !send)
    {
      break;
    }
  }
  // if (txt)
  // {
  //   l += snprintf(line + l, sizeof(line) - l, "%smsg=%s", send ? "&" : "", txt);
  // }

  if (l <= 0 || l >= sizeof(line) - 1)
  {
    if (bTmepLog)
      Logln("TMP Nejsou data k odeslani");
    return false;
  }

  WiFiClient client;

  if (bTmepOn)
  {
    if (!client.connect(host, TMEP_PORT))
    {
      if (tmep_ok)
      {
        Logln("Tmep chyba pripojeni k serveru");
        tmep_ok = false;
      }
      return false;
    }
    client.print("GET /?");
    client.print(line);
    client.print(" HTTP/1.1\r\nHost: ");
    client.print(host);
    client.print("\r\nConnection: close\r\n\r\n");

    if (!tmep_ok)
    {
      Logln("Tmep pripojeno");
      tmep_ok = true;
    }
  }

  if (bTmepLog)
  {
    Log("Tmep <- '");
    Log(line);
    Logln("'");
  }

  // --- Ctení odpovědi ---
  if (bTmepOn)
  {
    uint32_t startWait = millis();
    bool anyData = false;
    while (!anyData && (millis() - startWait < 2000))
    {
      if (client.available())
        anyData = true;
    }

    // Nepřišla – vubec žádná odpověď
    if (!anyData)
    {
      Logln("Tmep server neodpovida");
      client.stop();
      delay(20);
      tmep_ok = false;
      return false;
    }

    char cmd[32];
    bool hascmd = TmepReadCmd(client, cmd, sizeof(cmd));
    client.stop();
    delay(20);

    if (strlen(cmd) > 5)
      TmepCmdEvaluation(cmd);
    else if (bTmepLog)
      Logln("Tmep -> OK");
  }
  return true;
}

// ----------------------------------------------------------------------------
typedef struct
{
  const char *command; // prefix
  bool       *bit;     // kam nastavit TRUE (volitelné)
  int16_t    *reg16;   // kam ulozit hodnotu (volitelné)
} stTmepCommand;

stTmepCommand TmepCommand[] = 
{
    // Povel          bool         int16_t
    //--------------- ------------ --------
    {"Test", nullptr, nullptr},
};
const size_t TMEP_CMD = sizeof(TmepCommand) / sizeof(TmepCommand[0]);

void TmepCmdEvaluation(const char *cmd) // Vyhodnoceni zpravy z Tmep1.cz
{
  // cmd napr: "Teplota 1 5.00" nebo "Alarm zap"
  if (!strlen(cmd))
    return;
  for (size_t i = 0; i < TMEP_CMD; i++)
  {
    const char *key = TmepCommand[i].command;
    const size_t klen = strlen(key);
    // --- Pocatecni vynulovani ---
    if (TmepCommand[i].bit != nullptr)
      *(TmepCommand[i].bit) = false;
    if (TmepCommand[i].reg16 != nullptr)
      *(TmepCommand[i].reg16) = 0;
    if (strncmp(cmd, key, klen) != 0)
      continue;

    double v;
    char c = cmd[klen];
    if (c != '\0' && !isspace((unsigned char)c))
      continue;
    // --- Bit ---
    if (TmepCommand[i].bit)
      *(TmepCommand[i].bit) = true;
    // --- Hodnota (volitelná) ---
    if (TmepCommand[i].reg16)
    {
      const char *p = cmd + klen;
      while (*p && isspace((unsigned char)*p))
        p++;
      // --- Pokud tam nic není, nic neukládat ---
      if (*p)
      {
        char *endp = nullptr;
        v = strtod(p, &endp); // bere i 5.00, -3.2, atd.
        if (endp != p)        // něco se opravdu přečetlo
        {
          // int16_t val = (int16_t)lround(v * 10.0); // Zaokrouhlit
          int16_t val = (int16_t)(v * 10.0); // Oriznout
          *(TmepCommand[i].reg16) = val;
        }
      }
    }
    Log("-> TMP Povel: ");
    Log(TmepCommand[i].command);
    if (TmepCommand[i].reg16 != nullptr)
    {
      Log(" = ");
      Log(v);
    }
    Logln();
    return; // první shoda stačí
  }
  Log("-> TMP Neznamy povel: '");
  Log(cmd);
  Logln("'");
}

bool TmepReadCmd(WiFiClient &client, char *cmdBuf, size_t cmdBufSize) // Tmet.cz - Cteni odpovedi
{
  if (!cmdBuf || cmdBufSize == 0)
    return false;
  cmdBuf[0] = '\0';

  // --- Preskoceni HTTP hlavice (\r\n\r\n) ---
  uint8_t s = 0;
  uint32_t t0 = millis();
  while ((client.connected() || client.available()) && (millis() - t0 < 2000))
  {
    if (!client.available())
    {
      delay(0);
      continue;
    }
    char c = client.read();
    t0 = millis();
    if (s == 0 && c == '\r')
      s = 1;
    else if (s == 1 && c == '\n')
      s = 2;
    else if (s == 2 && c == '\r')
      s = 3;
    else if (s == 3 && c == '\n')
      break;
    else
      s = 0;
  }

  // --- Cteni CHUNKED tela → slouceny text ---
  char body[128];
  size_t n = 0;
  t0 = millis();

  while ((client.connected() || client.available()) && (millis() - t0 < 2000))
  {
    // radek s hex delkou chunku
    char line[16];
    size_t l = 0;
    while ((client.connected() || client.available()) && (millis() - t0 < 2000))
    {
      if (!client.available())
      {
        delay(0);
        continue;
      }
      char c = client.read();
      t0 = millis();
      if (c == '\r')
        continue;
      if (c == '\n')
        break;
      if (l < sizeof(line) - 1)
        line[l++] = c;
    }
    line[l] = '\0';

    long chunkSize = strtol(line, nullptr, 16);
    if (chunkSize <= 0)
      break; // 0 = konec

    while (chunkSize-- > 0 && n < sizeof(body) - 1)
    {
      if (!client.available())
      {
        delay(0);
        continue;
      }
      body[n++] = client.read();
      t0 = millis();
    }

    if (client.available())
      client.read();
    if (client.available())
      client.read();
  }

  body[n] = '\0';

  // --- Orez bilych znaku (trim) ---
  char *p = body;
  while (*p && isspace((unsigned char)*p))
    p++;
  char *e = p + strlen(p);
  while (e > p && isspace((unsigned char)e[-1]))
    --e;
  *e = '\0';

  // --- Ochrana iMin. delky ---
  if (strlen(p) > 0 && strlen(p) < 6)
  {
    Logln("!! TMP Chyba prijate zpravy");
    return false;
  }

  // --- Vysledek ---
  strncpy(cmdBuf, p, cmdBufSize - 1);
  cmdBuf[cmdBufSize - 1] = '\0';
  return true;
}

void TmepMultiSend()
{
  static uint32_t msTmep = millis();
  if (millis() - msTmep < 150000)
    return;
  msTmep = millis();

  static bool tmep = true;
  if (WiFi.status() != WL_CONNECTED && bTmepOn)
  {
    if (tmep)
    {
      Logln("Tmep nelze odeslat, nepripojena WiFi");
      tmep = false;
    }
    return;
  }
  tmep = true;

  static bool sw = true;
  if (sw)
    TmepSend(TMEP1_HOST, Tmep1, TMEP1_NUM, "Mereni 1");
  else
    TmepSend(TMEP2_HOST, Tmep2, TMEP2_NUM, "Mereni 2");
  sw = sw ? false : true;
}

// ----------------------------------------------------------------------------
enum WeatherIco
{
  ICO_JASNO,ICO_POLOJASNO,ICO_ZAMRACENO,ICO_DEST,ICO_BOURKA,ICO_SNIH, // ID0
  ICO_MES0,ICO_MES1,ICO_MES2,ICO_MES3,ICO_MES4,ICO_MES5,ICO_MES6,ICO_MES7,ICO_MES8,  // ID6 NOV...
  ICO_RADAR, // ID15
  ICO_800D,ICO_800N,ICO_801D,ICO_801N,ICO_802D,ICO_802N,ICO_803,ICO_804, // ID16 Jasno...
  ICO_500,ICO_501,ICO_502,ICO_503,ICO_511, // ID 24 Dest az mrznouci
  ICO_520D,ICO_520N,ICO_600, // ID29 Prehanky
  ICO_211,ICO_202,ICO_212, // ID32 Bourky
  ICO_701,ICO_771, // ID35 Mlha, boure
  ICO_PANACI,ICO_DORT,ICO_KYTKA,ICO_ZAJICEK,
};

// ----------------------------------------------------------------------------
struct strPocasi
{
  float teplota;
  float teplota_pocit;
  float teplota_min;
  float teplota_max;
  float rychlost_vetru;
  uint32_t dt;
  uint16_t tlak;
  uint16_t tlak_nm;
  uint16_t id_pocasi;
  uint16_t smer_vetru;    // 360°
  uint16_t vychod_slunce; // hod*60 + min
  uint16_t zapad_slunce;  // hod*60 + min
  uint8_t vlhkost;
  uint8_t oblacnost;
};

strPocasi Pocasi;

int WeatherToIco(int id)
{
  if (id == 800)
    return ICO_JASNO;
  if (id >= 801 && id <= 802)
    return ICO_POLOJASNO;
  if (id >= 803 && id <= 804)
    return ICO_ZAMRACENO;
  if (id >= 701 && id <= 741)
    return ICO_ZAMRACENO; // Mlha
  if (id >= 500 && id <= 531)
    return ICO_DEST; // Destive
  if (id >= 300 && id <= 331)
    return ICO_DEST; // Mrholeni
  if (id >= 200 && id <= 232)
    return ICO_BOURKA; // Bourka
  if (id == 771)
    return ICO_BOURKA; // Boure
  if (id >= 600 && id <= 622)
    return ICO_SNIH; // Snezive

  // 701,741 - Mlha
  // 711 - Kour
  // 721 - Opar
  // 731,761 - Prach
  // 751 - Pisek
  // 761 - Sopecny popel
  // 771 - Boure
  // 781 - Tornado
  return ICO_JASNO;
}

// ----------------------------------------------------------------------------
int ForecastIdToIco(int id, bool night) // --- Prevod ID -> ICO ---
{
  switch (id)
  {
  case 800: // Jasno
    if (night)
      return ICO_800N;
    return ICO_800D;
  // case 801: // Oblacnost 11-24%
  //   if(night) return ICO_801N;
  //   return ICO_801D;
  case 801:
  case 802: // Oblacnost 25-50%
    if (night) return ICO_802N;
    return ICO_802D;
  case 803: return ICO_803; // Oblacnost 51-84%
  case 804: return ICO_804; // Oblacnost 85-100%
  case 500: return ICO_500; // Dest slaby
  case 501: return ICO_501; // Dest mirny
  case 502: return ICO_502; // Dest silny
  case 503:
  case 504: return ICO_503; // Dest velmi silny
  case 511: return ICO_511; // Dest mrznouci
  case 210:
  case 211: return ICO_211; // Bourka
  case 212: return ICO_212; // Silna bourka
  case 771:
  case 781: return ICO_771; // Boure, tornado
  default:
    if (id >= 200 && id <= 202) return ICO_202; // Bourka s destem
    if (id >= 230 && id <= 232) return ICO_202; // Bourka s destem
    if (id >= 520 && id <= 531)
    {
      if (night)
        return ICO_520N;
      return ICO_520D;
    } // Prehanky
    if (id >= 600 && id <= 620)
      return ICO_600; // Snehove
    if (id >= 701 && id <= 762)
      return ICO_701; // Mlha, opar, prach
  }
  return ICO_RADAR; // Radar
}

// ----------------------------------------------------------------------------
bool OpenWeatherRead() // --- Cteni predpovedi pocasi ---
{
  if (!bWeatherOn)
    return false;

  if (time(nullptr) > 1000000)
  {
    static int iHod = -1;
    time_t now = time(NULL);
    struct tm ti;
    localtime_r(&now, &ti);
    if (ti.tm_hour == iHod)
      return false;
    iHod = ti.tm_hour;
  }
  else
  {
    static uint32_t ms = 0;
    if (millis() - ms <= 3600000 && ms != 0)
      return false;
    ms = millis();
  }

  static bool weather_ok = true;
  bool cas = false;

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15000);

  HTTPClient https;
  https.setTimeout(10000);
  https.setReuse(false);
  // Logln(weatherUrl);

  // https.begin(client, weatherUrl);
  if (!https.begin(client, weatherUrl))
  {
    Logln("HTTPS begin chyba");
    return false;
  }

  // https.addHeader("User-Agent", "ESP32");

  int httpCode = https.GET();
  if (httpCode != 200)
  {
    https.end();
    if (weather_ok)
    {
      Logf("OpenWeather - chyba cteni #%d\n", httpCode);
      weather_ok = false;
    }
    return false;
  }

  if (!weather_ok) // --- Zaznamenat pripojeni ---
  {
    char txt[] = "OpenWeather - OK";
    SdLog(txt);
    Logln(txt);
    weather_ok = true;
  }

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, https.getStream());

  if (error)
  {
    if (weather_ok)
    {
      char txt[] = "OpenWeather - Chyba Json";
      // SdLog(txt);
      Logln(txt);
      weather_ok = false;
    }
    https.end();
    return false;
  }

  // serializeJsonPretty(doc, Serial);
  // String payload = https.getString();
  // Serial.println(payload);

  int i = 0;
  Pocasi.dt = doc["list"][i]["dt"] | 0;
  Pocasi.teplota = doc["list"][i]["main"]["temp"] | 0.0;
  Pocasi.teplota_pocit = doc["list"][i]["main"]["feels_like"] | 0.0;
  Pocasi.teplota_min = doc["list"][i]["main"]["temp_min"] | 0.0;
  Pocasi.teplota_max = doc["list"][i]["main"]["temp_max"] | 0.0;
  Pocasi.tlak = doc["list"][i]["main"]["grnd_level"] | 0;
  Pocasi.tlak_nm = doc["list"][i]["main"]["pressure"] | 0;
  Pocasi.vlhkost = doc["list"][i]["main"]["humidity"] | 0;
  Pocasi.rychlost_vetru = doc["list"][i]["wind"]["speed"] | 0.0;
  Pocasi.smer_vetru = doc["list"][i]["wind"]["deg"] | 0;
  Pocasi.oblacnost = doc["list"][i]["clouds"]["all"] | 0;
  Pocasi.id_pocasi = doc["list"][i]["weather"][0]["id"] | 0;

  idPocasi = Pocasi.id_pocasi;

  Log("Predpoved k: ");
  Logln(doc["list"][i]["dt_txt"] | "--:--:--");

  if (bWeatherLog)
  {
    Log("- Teplota   : ");Logln(Pocasi.teplota);
    Log("- Tepl min  : ");Logln(Pocasi.teplota_min);
    Log("- Tepl max  : ");Logln(Pocasi.teplota_max);
    Log("- Tepl pocit: ");Logln(Pocasi.teplota_pocit);
    Log("- Vlhkost   : ");Logln(Pocasi.vlhkost);
    Log("- Tlak      : ");Logln(Pocasi.tlak);
    Log("- Tlak n/m  : ");Logln(Pocasi.tlak_nm);
    Log("- Rych.vetru: ");Logln(Pocasi.rychlost_vetru * 3.6);
    Log("- Smer vetru: ");Logln(Pocasi.smer_vetru);
    Log("- Oblacnost : ");Logln(Pocasi.oblacnost);
    Log("- ID pocasi : ");Log(Pocasi.id_pocasi);Log(" - ");
    Logln(doc["list"][i]["weather"][0]["description"] | "");
  }

  // uint16_t timezone = doc["list"]["city"]["timezone"] | 0;
  time_t sunrise = doc["city"]["sunrise"] | 0;
  time_t sunset = doc["city"]["sunset"] | 0;

  if (sunrise > 0 && sunset > 0)
  {
    struct tm ti;
    localtime_r(&sunrise, &ti);
    Pocasi.vychod_slunce = (ti.tm_hour * 60) + ti.tm_min;
    if (bWeatherLog)
      Logf("- Vychod slunce: %02d:%02d\n", ti.tm_hour, ti.tm_min);
    localtime_r(&sunset, &ti);
    Pocasi.zapad_slunce = (ti.tm_hour * 60) + ti.tm_min;
    if (bWeatherLog)
      Logf("- Zapad slunce : %02d:%02d\n", ti.tm_hour, ti.tm_min);
  }

  // --- Nacist dalsi predpovedi ---
  const int xNum = 6;
  for (int i = 0; i < xNum; i++)
  {
    int k = i + 1;

    // --- Ikona: "04d" → den, "04n" → noc ---
    int id = doc["list"][k]["weather"][0]["id"] | 800;
    const char *icon = doc["list"][k]["weather"][0]["icon"] | "01d";
    bool night = false;
    if (icon[2] == 'n')
      night = true;
    int pic = ForecastIdToIco(id, night);
    Disf("p%d.pic=%d\xff\xff\xff", 3 + i, pic); // Ikona

    // --- Popis casu ---
    char txt[6];
    const char *dt = doc["list"][k]["dt_txt"] | "-";
    if (strlen(dt) >= 16)
    {
      strncpy(txt, dt + 11, 2); // posun na "12:00"
      txt[2] = 'h';
      txt[3] = '\0'; // ukončení řetězce
    }
    else
    {
      strcpy(txt, "--:--"); // fallback
    }
    Disf("t%d.txt=\"%s\"\xff\xff\xff", 19 + i, txt);

    // Logf("- #%d %s = %d, icon:%s - p%d.pic=%d\n", k, txt, id, 3 + i, icon, pic);
  }
  https.end();

  return true;
}

// ----------------------------------------------------------------------------
uint8_t NoonPhase()
{
  const float  PHASE_SHIFT   = 0.0;       // záporné = později, kladné = dříve
  const double SYNODIC_MONTH = 29.530588;
  const time_t EPOCH_NEW_MOON = 1773886980; // 19.3.2026 02:23 UTC
  time_t now;
  time(&now);   // UTC timestamp
  double days = (double)(now - EPOCH_NEW_MOON) / 86400.0;
  double age = fmod(days + PHASE_SHIFT, SYNODIC_MONTH);
  if (age < 0) age += SYNODIC_MONTH;
  double segment = SYNODIC_MONTH / 8.0;
  int index = (int)(age/segment);
  if (index > 7) index = 7;
  Logf("Faze mesice: age = %.2f idx = %d\n", age, index);
  return index;   // 0–7
}

// ----------------------------------------------------------------------------
int WaetherToPicture() // --- Vyhodnotit ikonu pocasi ---
{
  bool noc = false;
  struct tm ti;
  if (time(nullptr) > 1000000)
  {
    time_t now = time(NULL);
    struct tm ti;
    localtime_r(&now, &ti);
    uint16_t cas = (ti.tm_hour * 60) + ti.tm_min;
    if (bWeatherLog)
      Logf("Cas:%u, Vychod:%u, Zapad:%u\n", cas, Pocasi.vychod_slunce,
           Pocasi.zapad_slunce);
    noc = cas < Pocasi.vychod_slunce || cas > Pocasi.zapad_slunce;
  }
  int ico = WeatherToIco(Pocasi.id_pocasi);
  int noon = NoonPhase();
  if (noc && ico >= ICO_JASNO && ico <= ICO_POLOJASNO)
    return noon + ICO_MES0;
  return ico;
}

// ----------------------------------------------------------------------------
int DateKey(uint8_t m, uint8_t d) // --- Hodnota mesic+iDen ---
{
  return m * 100 + d;
}

// --- Vyhledat udalost ---
int FindNextEvent(uint8_t iMes, uint8_t iDen)
{

  int today = DateKey(iMes, iDen);

  // první průchod – hledáme >= dnes
  for (int i = 0; i < KAL_NUM; i++)
  {
    int key = DateKey(Kalendar[i].mesic, Kalendar[i].den);

    if (today <= key)
      return i;
  }

  // pokud nic nenalezeno → přechod roku
  return 0;
}

// --- Vypsat udalosti 1. a 2. radek ---
void DisplayEvents(uint8_t iMes, uint8_t iDen)
{
  int start = FindNextEvent(iMes, iDen);
  int today = DateKey(iMes, iDen);

  for (int j = 0; j < 2; j++)
  {
    int idx = start + j;
    if (idx >= KAL_NUM)
      idx = 0;

    int key = DateKey(Kalendar[idx].mesic, Kalendar[idx].den);

    // barva
    // Logf("Porovnani kalendare: M:%d D:%d - %d ? %d\n", iMes, iDen, key, today );
    if (key == today) // Dnes
    {
      Disf("t%d.pco=65504\xFF\xFF\xFF", 10 + j); // žlutá

      if (j == 0) // 1.radek
      {
        Disf("p1.pic=%d\xFF\xFF\xFF",
             ICO_PANACI +
                 Kalendar[idx].pic); // 0=panaci,1=dort,2=kytka,3=zajicek
      }
    }
    else
    {
      Disf("t%d.pco=65535\xFF\xFF\xFF", 10 + j); // bílá

      if (j == 0) // 1.radek
      {
        Disf("p1.pic=%d\xFF\xFF\xFF", ICO_PANACI); // Zakladni ikona
      }
    }

    // text
    char line[35];
    snprintf(line, sizeof(line), "%2d.%d. %s %s", Kalendar[idx].den,
             Kalendar[idx].mesic, sDvt[Kalendar[idx].dvt],
             Kalendar[idx].udalost);

    Disf("t%d.txt=\"%s\"\xFF\xFF\xFF", 10 + j, line);
    // strncpy(txt[j], line, 25);
  }
}

// ============================================================================
uint8_t DispParse(uint8_t *data, uint8_t len) // --- Odpovedi displeje ---
{
  if (len == 0) return 0;
  uint8_t ret = data[0];
  if (data[0] == 0x01) return 0; // Uspesna operace

  if (!bDisplayLog) return 0;

  // char log[128] = ""; char *p = log; uint8_t l = 0;
  // l += snprintf(log+l, sizeof(log)-l, "DIS -> 0x%02X ", data[0]);

  Logf("DIS -> 0x%02X ", data[0]);

  switch (data[0])
  {
  case 0x00:
    Logln("Chyba instrukce, nebo start");
    break; // Invalid instruction
  case 0x05:
    Logln("Invalid font");
    break;
  case 0x12:
    Logln("Chyba ID grafu");
    break; // Chyba jmena promenne
  case 0x1A:
    Logln("Chyba parametru");
    break; // Chyba parametru promenne
  case 0x1B:
    Logln("Chyba operace s promennou");
    break; // Neplatna operace s promennou
  case 0x64:
    Logf("Okno: %d\n", data[1]);
    break;   // Aktualni stranka  (0x64 0x01 0x00 0x00 0xFF 0xFF 0xFF)
  case 0x65: // Dotyk component ID  (0x65 0x00 0x01 0x01 0xFF 0xFF 0xFF)
    Logf("Dotyk Component ID:%u %s\n", data[2], data[3] ? "UP" : "DN");
    break;
  case 0x66: // Aktualni stranka  (0x66 0x01 0xFF 0xFF 0xFF)
    Logf("Okno: %d\n", data[1]);
    // bDispInit = true;
    break;
  case 0x67: // Dotyk v nespicim rezimu (0x67 0x00 0x7A 0x00 0x1E 0x01 0xFF 0xFF 0xFF)
    Logf("Dotyk: %3u X %3u %s - aktivni\n", data[1] * 256 + data[2],
         data[3] * 256 + data[4], data[5] ? "UP" : "DN");
    break;
  case 0x68: // Dotyk v spicim rezimu (0x67 0x00 0x7A 0x00 0x1E 0x01 0xFF 0xFF 0xFF)
    Logf("Dotyk: %3u X %3u %s - spici\n", data[1] * 256 + data[2],
         data[3] * 256 + data[4], data[5] ? "UP" : "DN");
    break;
  case 0x70: // Odpoved na "get t0.txt"  (0x70 0x61 0x62 0x31 0x32 0x33 0xFF 0xFF 0xFF)
  {
    char txt[100];
    memcpy(txt, &data[1], len - 1);
    txt[len - 1] = 0;
    Log("'");
    Log(txt);
    Logln("'");
    break;
  }
  case 0x71: // Odpoved na "get n0.val" (4 byte little endian) (0x71 0x01 0x02 0x03 0x04 0xFF 0xFF 0xFF)
  {
    uint32_t val = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
    Logln(val);
    break;
  }
  case 0x86: // Prechod do spiciho rezimu (0x86 0xFF 0xFF 0xFF)
    Logln("Prechod do spani");
    break;
  case 0x87: // Prechod do probuzeni (0x87 0xFF 0xFF 0xFF)
    Logln("Prechod do probuzeni");
    break;
  case 0x88: // Display PowerUp (0x88 0xFF 0xFF 0xFF)
    Logln("Start");
    break;
  case 0x90:
    Logf("Podsviceni=%d \%\n", map(data[1], 0, 255, 0, 100));
    break;
  case 0x91: // Dotyk vraci ID (x91 xID 3xFF)
    switch (data[1])
    {
    case 19: // Venkovni teplota
    default:
      Logf("Dotyk ID=%d\n", data[1]);
      break;
    }
    break;
  default:
    Logln("Neznama odpoved");
    break;
  }
  return ret;
}

uint8_t DispRead()
{
  uint8_t ret = 0;
  while (Disp.available())
  {
    uint8_t c = Disp.read();
    if (nxIndex < NX_BUF)
      nxBuf[nxIndex++] = c;
    if (c == 0xFF)
    {
      nxCount++;
      if (nxCount == 3)
      {
        ret = DispParse(nxBuf, nxIndex - 3); // bez FF
        nxIndex = 0;
        nxCount = 0;
      }
    }
    else
      nxCount = 0;
  }
  return ret; // true = restart 0x88
}

// ============================================================================
struct TrendRec // --- Struktura zaznamu trendu na SD ---
{
  long t;
  long h;
  long p;
};
TrendRec trend[400];

// --- Nacteni trendu z SD ---
bool TrendRedrawing(bool rewrite = false)
{
  // --- Nacist datum pro vyber souboru ---
  if (time(nullptr) < 1000000)
    return false;

  time_t now = time(NULL);
  struct tm ti;
  localtime_r(&now, &ti);
  char path[32];
  snprintf(path, sizeof(path), "/TRD_%04d%02d%02d.CSV", ti.tm_year + 1900,
           ti.tm_mon + 1, ti.tm_mday);

  // --- Otevrit soubor ---
  File file = SD.open(path);
  if (!file)
  {
    Logln("SD Chyba cteni dat");
    return false;
  }

  memset(trend, 0xFF, sizeof(trend));
  long tmin = LONG_MAX, tmax = LONG_MIN;
  long hmin = LONG_MAX, hmax = LONG_MIN;
  long pmin = LONG_MAX, pmax = LONG_MIN;

  char line[80];
  int count = 0, lastIdx = 0;

  // --- Necteni dat trendu ---
  while (file.available())
  {
    int n = file.readBytesUntil('\n', line, sizeof(line) - 1);
    line[n] = 0;
    int idx;
    long t = 0, h = 0, p = 0;
    if (sscanf(line, "%d;%ld;%ld;%ld", &idx, &t, &h, &p) == 4)
    {
      trend[count].t = t; // Teplota *10
      trend[count].h = h; // Vlhkost *10
      trend[count].p = p; // Tlak *10
      if (t < tmin) tmin = t;
      if (t > tmax) tmax = t;
      if (h < hmin) hmin = h;
      if (h > hmax) hmax = h;
      if (p < pmin) pmin = p;
      if (p > pmax) pmax = p;
      lastIdx = idx;
      count++;
    }
  }
  file.close();

  if (count > 0)
  {
    if (!rewrite)
    {
      Logf("Zaznamu grafu = %6d  Idx = %6d\n", count, lastIdx);
      Logf(" Teplota: Min = %6.1f  Max = %6.1f  MIN = %6.1f  MAX = %6.1f\n",
           tmin / 10.0, tmax / 10.0, lTmin / 10.0, lTmax / 10.0);
      Logf(" Vlhkost: Min = %6.1f  Max = %6.1f  MIN = %6.1f  MAX = %6.1f\n",
           hmin / 10.0, hmax / 10.0, lHmin / 10.0, lHmax / 10.0);
      Logf(" Tlak   : Min = %6.1f  Max = %6.1f  MIN = %6.1f  MAX = %6.1f\n",
           pmin / 10.0, pmax / 10.0, lPmin / 10.0, lPmax / 10.0);
    }
    else
    {
      Logln("Regenerace trendu");
      Disf("cle 1,255\xFF\xFF\xFF");
      delay(20);

      long l;
      for (int i = 0; i < count; i++)
      {
        l = map(trend[i].t, lTmin, lTmax, 0, 100);
        l = constrain(l, 0, 100);
        Disf("add 1,0,%ld\xFF\xFF\xFF", l);
        l = map(trend[i].h, lHmin, lHmax, 0, 100);
        l = constrain(l, 0, 100);
        Disf("add 1,1,%ld\xFF\xFF\xFF", l);
        l = map(trend[i].p, lPmin, lPmax, 0, 100);
        l = constrain(l, 0, 100);
        Disf("add 1,2,%ld\xFF\xFF\xFF", l);
      }
    }
    return true;
  }
  else
    Logf("SD nenalezen zaznam v souboru '%s'\n", path);
  return false;
}

// ============================================================================
void Klavesy()
{
  preferences.begin("setup", false); // false:zapis true:pouze cteni
  char c = Serial.read();
  switch (c)
  {
  case 'd': // Logovat display
    bDisplayLog = bDisplayLog ? false : true;
    Log("Display Log:");
    Logln(bDisplayLog ? "ZAP" : "VYP");
    preferences.putBool("displaylog", bDisplayLog);
    break;
  case 'D': // Povolit display
    bDisplayOn = bDisplayOn ? false : true;
    if(!bDisplayOn)
    {
      Disp.flush();
      Disp.end();
      pinMode(PIN_RxD,INPUT);
      pinMode(PIN_RxD,INPUT);
    }
    else
      Disp.begin(115200, SERIAL_8N1, PIN_RxD, PIN_TxD);    
    
    sprintf(sLog,"Display serial %s", bDisplayOn ? "Zapnut" : "Vypnut");
    Logln(sLog);
    if(bSdOn) SdLog(sLog);
    preferences.putBool("displayon", bDisplayOn);
    break;
  case 'g': // Vypis zaznamu karty
    TrendRedrawing();
    break;
  case 'G': // Generovat graf
    TrendRedrawing(true);
    sprintf(sLog,"Display regenerovani grafu");
    Logln(sLog);
    if(bSdOn) SdLog(sLog);
    break;
  case 's': // Logovat senzory
    bSensLog = bSensLog ? false : true;
    Log("Sensor Log:");
    Logln(bSensLog ? "ZAP" : "VYP");
    preferences.putBool("senslog", bSensLog);
    break;
  case 'S': // Povolit filtrace
    bSensFilter = bSensFilter ? false : true;
    // Log("Sens filtr:");
    // Logln(bSensFilter ? "ZAP" : "VYP");
    sprintf(sLog,"Sens filtr: %s", bSensFilter ? "ZAP" : "VYP");
    Logln(sLog);
    if(bSdOn) SdLog(sLog);
    preferences.putBool("sensfilter", bSensFilter);
    break;
  case 't': // Logovat Tmep.cz
    bTmepLog = bTmepLog ? false : true;
    Log("Log Tmep:");
    Logln(bTmepLog ? "ZAP" : "VYP");
    preferences.putBool("tmeplog", bTmepLog);
    break;
  case 'T': //Povolit Tmep.cz
    bTmepOn = bTmepOn ? false : true;
    // Log("Tmep On:");
    // Logln(bTmepOn ? "ZAP" : "VYP");
    sprintf(sLog,"Tmep On: %s", bTmepOn ? "ZAP" : "VYP");
    Logln(sLog);
    if(bSdOn) SdLog(sLog);
    preferences.putBool("tmepon", bTmepOn);
    break;
  case 'u': // Logovat UDP
    bUdpLog = bUdpLog ? false : true;
    Log("UDP Log:");
    Logln(bUdpLog ? "ZAP" : "VYP");
    preferences.putBool("udplog", bUdpLog);
    break;
  case 'w': // Logovat predpovedi
    bWeatherLog = bWeatherLog ? false : true;
    Log("Weather Log:");
    Logln(bWeatherLog ? "ZAP" : "VYP");
    preferences.putBool("weatherlog", bWeatherLog);
    break;
  case 'W': // Povolit predpovedi
    bWeatherOn = bWeatherOn ? false : true;
    // Log("Weather:");
    // Logln(bWeatherOn ? "ZAP" : "VYP");
    sprintf(sLog,"Weather: %s", bWeatherOn ? "ZAP" : "VYP");
    Logln(sLog);
    if(bSdOn) SdLog(sLog);
    preferences.putBool("weatheron", bWeatherOn);
    break;
  case 'z': // Logovat SD
    bSdLog = bSdLog ? false : true;
    Log("SD Log:");
    Logln(bSdLog ? "ZAP" : "VYP");
    preferences.putBool("sdlog", bSdLog);
    break;
  case 'Z': // Povolit SD
    bSdOn = bSdOn ? false : true;
    if(!bSdOn) // Vypnout
    {
      if(bSdOk)
      {
        strcpy(sLog,"SD Zapis: VYP");
        SdLog(sLog);
        SD.end();
        bSdOk = false;
      }
    }
    else // Zapnout
    {
      bSdOk = SdInit();
      if(!bSdOk) SdReinit();
      if(bSdOk)
      {
        bSdOk = bSdOn = true;
        strcpy(sLog,"SD Zapis: ZAP");
        SdLog(sLog);
      }
      else
      {
        bSdOk = bSdOn = false;
        strcpy(sLog,"SD Zapis chyba");
      }
    }
    Logln(sLog);
    preferences.putBool("sdon", bSdOn);
    break;
  }
  preferences.end();
}

// ============================================================================
void setup()
{
  Serial.begin(115200);
  Disp.begin(115200, SERIAL_8N1, PIN_RxD, PIN_TxD);
  pinMode(PIN_ON, OUTPUT);
  digitalWrite(PIN_ON, HIGH);
  LedRGB(true);
  delay(2000);
  SystemInfo();
  VBatRead(true);

  // --- I2C ---
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.nist.gov");
  RtcToSystemTime();

  // --- SPI ---
  SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_CS);
  SdInit(true);

  // --- WiFi ---
  WiFiConnect(true); // Setup

  // --- Preference ---
  preferences.begin("setup", true); // false:zapis true:pouze cteni
  bDisplayOn = preferences.getBool("displayon", bDisplayOn);
  bDisplayLog = preferences.getBool("displaylog", bDisplayLog);
  bSdOn = preferences.getBool("sdon", bSdOn);
  bSdLog = preferences.getBool("sdlog", bSdLog);
  bSensLog = preferences.getBool("senslog", bSensLog);
  bSensFilter = preferences.getBool("sensfilter", bSensFilter);
  bTmepOn = preferences.getBool("tmepon", bTmepOn);
  bTmepLog = preferences.getBool("tmeplog", bTmepLog);
  bUdpLog = preferences.getBool("udplog", bUdpLog);
  bWeatherOn = preferences.getBool("weatheron", bWeatherOn);
  bWeatherLog = preferences.getBool("weatherlog", bWeatherLog);
  lTmin = preferences.getLong("tmin", lTmin);
  lTmax = preferences.getLong("tmax", lTmax);
  lHmin = preferences.getLong("hmin", lHmin);
  lHmax = preferences.getLong("hmax", lHmax);
  lPmin = preferences.getLong("pmin", lPmin);
  lPmax = preferences.getLong("pmax", lPmax);
  preferences.end();

  // delay(500);
  Logln("--- START ---");
  if(bSdOn) SdLog("SYSTEM START");

  if(!bDisplayOn)
  {
    Disp.end();
    pinMode(PIN_RxD,INPUT);
    pinMode(PIN_RxD,INPUT);
    strcpy(sLog,"Display serial vypnut !!!");
    Logln(sLog);
    if(bSdOn) SdLog(sLog);
  }
}

// ============================================================================
void loop()
{
  if (Serial.available()) Klavesy();
  DispRead();

  static bool wifi_ok = false;
  WiFiConnect();
  if( bWifiOk )
  {
    if(!wifi_ok)
    {
      if(bSdOn) SdLog("WiFi pripojeni OK");
      wifi_ok = true;
    }
    UdpParse();
  }
  else if(wifi_ok)
  {
    if(bSdOn) SdLog("WiFi pripojeni OK");
    wifi_ok = false;
  }

  SensorLocalScan();
  SensorList();
  TmepMultiSend();

  if (bGraphRefresh) 
  {
    bGraphRefresh = false;
    TrendRedrawing(true);
  }

  // --- Casove ---
  static int iTrendIdx = 0;
  bool trend = false;

  if (time(nullptr) > 1000000)
  {
    time_t now = time(NULL);
    struct tm ti;
    localtime_r(&now, &ti);
    iSec = ti.tm_sec;         // 0-59
    iMin = ti.tm_min;         // 0-59
    iHod = ti.tm_hour;        // 0-23
    iDen = ti.tm_mday;        // 1-31
    iMes = ti.tm_mon + 1;     // 0-11
    iRok = ti.tm_year + 1900; // 0=1900

    // --- Vymazat trend ---
    static int dt = 0;
    if (dt == 0) // Restart procesoru
    {
      dt = iDen;
    }
    else if (dt != iDen) // Zmena dne
    {
      dt = iDen;
      Disf("cle 1,255\xFF\xFF\xFF");
    }

    // --- Ukladat trend ---
    int secOfDay = iHod * 3600 + iMin * 60 + iSec;
    iTrendIdx = secOfDay / 216 + 1;
    if (iTrendIdx > 400) iTrendIdx = 400;
    static int tdx = 0;
    if (tdx != iTrendIdx)
    {
      tdx = iTrendIdx;
      trend = true;
    }

    // --- Denni udalosti ---
    static int lastDay = -1;
    if (lastDay != iDen || lastDay == -1)
    {
      lastDay = iDen;
      DisplayEvents(iMes, iDen);
    }
  }

  // --- Pocasi - Zmeny nactenim predpovedi ---
  bool bPredpoved = OpenWeatherRead();

  if (bPredpoved)
  {
    // --- Ikona pocasi slunce/mesic ---
    int id = WaetherToPicture();
    Disf("p0.pic=%lu\xFF\xFF\xFF", id);
    if (bWeatherLog)
      Logf("Pocasi zmenana ICO = %d\n", id);

    // --- Bargrafy Noc/Den/Noc ---
    uint16_t iVychodSlunce = Pocasi.vychod_slunce;
    if (iVychodSlunce > 0)
    {
      int bg1 = constrain(iVychodSlunce, 0, 720);
      bg1 = map(bg1, 0, 720, 0, 100);
      Disf("j0.val=%d\xFF\xFF\xFF", bg1);
    }
    uint16_t iZapadSlunce = Pocasi.zapad_slunce;
    if (iZapadSlunce > iVychodSlunce)
    {
      int bg2 = constrain(iZapadSlunce, 720, 1440);
      bg2 = map(bg2, 720, 1440, 0, 100);
      Disf("j1.val=%d\xFF\xFF\xFF", bg2);
    }
  }

  // --- Synchronizace casu s displejem v 4:00 ---
  static bool disprtcsync = false;
  if(iHod == 4 && !disprtcsync)
  {
    if(NtpGetTime())
    {
      if(bSdOn) SdLog( "Disp synchro RTC" );
      Disf("rtc0=%u\xFF\xFF\xFF",iRok);
      Disf("rtc1=%u\xFF\xFF\xFF",iMes);
      Disf("rtc2=%u\xFF\xFF\xFF",iDen);
      Disf("rtc3=%u\xFF\xFF\xFF",iHod);
      Disf("rtc4=%u\xFF\xFF\xFF",iMin);
      Disf("rtc5=%u\xFF\xFF\xFF",iSec);
    }
    disprtcsync = true;
  }
  else if( disprtcsync && iHod != 4) disprtcsync = false;

  // --- Inicializace ---
  static bool initl = true;
  long val;
  int id;

  // --- Venkovni teplota ---
  bool tset = false;
  id = SensCheckId("T11");
  if (id >= 0 && Sensor[id].valid)
  {
    val = lroundf(Sensor[id].valueF * 10.0f);
    if (lTval != val)
    {
      lTval = val;
      Disf("x03.val=%ld\xFF\xFF\xFF", val);
    }
    if (trend)
    {
      while (val > lTmax)
      {
        lTmax += lTshift;
        lTmin += lTshift;
        tset = true;
      }
      while (val < lTmin)
      {
        lTmin -= lTshift;
        lTmax -= lTshift;
        tset = true;
      }
      long t = map(val, lTmin, lTmax, 0, 100);
      t = constrain(t, 0, 100);
      Disf("add 1,0,%ld\xFF\xFF\xFF", t);
    }
    if (tset)
    {
      preferences.begin("setup", false); // false:zapis
      preferences.putLong("tmin", lTmin);
      preferences.putLong("tmax", lTmax);
      preferences.end();
      Logf("Zmena rozsahu teploty: Min = %ld Max = %ld °C\n", lTmin / 10,
           lTmax / 10);
      bGraphRefresh = true;
    }
  }
  if (tset || initl)
  {
    Disf("x0.val=%ld\xFF\xFF\xFF", lTmax);
    Disf("x3.val=%ld\xFF\xFF\xFF", lTmin);
  }

  // --- Venkovni vlhkost ---
  bool hset = false;
  id = SensCheckId("H11");
  if (id >= 0 && Sensor[id].valid)
  {
    val = lroundf(Sensor[id].valueF * 10.0f);
    if (lHval != val)
    {
      lHval = val;
      Disf("x04.val=%ld\xFF\xFF\xFF", val);
    }
    if (trend)
    {
      long h = map(val, lHmin, lHmax, 0, 100);
      h = constrain(h, 0, 100);
      Disf("add 1,1,%ld\xFF\xFF\xFF", h);
    }
    if (hset)
    {
      preferences.begin("setup", false); // false:zapis
      preferences.putLong("hmin", lHmin);
      preferences.putLong("hmax", lHmax);
      preferences.end();
      Logf("Zmena rozsahu vlhkosti: Min = %ld Max = %ld °C\n", lHmin / 10,lHmax / 10);
      bGraphRefresh = true;
    }
  }
  if (hset || initl)
  {
    Disf("x1.val=%ld\xFF\xFF\xFF", lHmax);
    Disf("x4.val=%ld\xFF\xFF\xFF", lHmin);
  }

  // --- Atmosfericky tlak ---
  bool pset = false;
  id = SensCheckId("P11");
  if (id >= 0 && Sensor[id].valid)
  {
    val = lroundf(Sensor[id].valueF * 10.0f);
    if (lPval != val)
    {
      lPval = val;
      Disf("x05.val=%ld\xFF\xFF\xFF", val);
    }
    if (trend)
    {
      while (val > lPmax)
      {
        lPmax += lPshift;
        lPmin += lPshift;
        pset = true;
      }
      while (val < lPmin)
      {
        lPmin -= lPshift;
        lPmax -= lPshift;
        pset = true;
      }
      long p = map(val, lPmin, lPmax, 0, 100);
      p = constrain(p, 0, 100);
      Disf("add 1,2,%ld\xFF\xFF\xFF", p);
    }
    if (pset)
    {
      preferences.begin("setup", false); // false:zapis
      preferences.putLong("pmin", lPmin);
      preferences.putLong("pmax", lPmax);
      preferences.end();
      Logf("Zmena rozsahu tlaku: Min = %ld Max = %ld °C\n", lPmin / 10,lPmax / 10);
      bGraphRefresh = true;
    }
  }
  if (pset || initl)
  {
    Disf("x2.val=%ld\xFF\xFF\xFF", lPmax);
    Disf("x5.val=%ld\xFF\xFF\xFF", lPmin);
  }

  // --- Zapsat na SD kartu ---
  if (trend && bSdOn && (lTval || lHval || lPval))
  {
    char path[20];
    snprintf(path,sizeof(path),"/TRD_%04d%02d%02d.CSV",iRok,iMes,iDen);
    char message[120];
    snprintf(message,sizeof(message),"%d;%ld;%ld;%ld\n",iTrendIdx,lTval,lHval,lPval);
    SdAppend(path, message);
    if (bSdLog) Logf("SD Zapis trendu: %s - %s", path, message);
  }

  // --- Vnitrni teplota ---
  static long lTval2 = 0;
  id = SensCheckId("T21");
  if (id >= 0 && Sensor[id].valid)
  {
    val = lroundf(Sensor[id].valueF * 10.0f);
    if (lTval2 != val)
    {
      lTval2 = val;
      Disf("x01.val=%ld\xFF\xFF\xFF", val);
    }
  }

  // --- Vnitrni vlhkost ---
  static long lHval2 = 0;
  id = SensCheckId("H21");
  if (id >= 0 && Sensor[id].valid)
  {
    val = lroundf(Sensor[id].valueF * 10.0f);
    if (lHval2 != val)
    {
      lHval2 = val;
      Disf("x02.val=%ld\xFF\xFF\xFF", val);
    }
  }

  // --- Predpovedni teplota ---
  static long teplota = 0;
  val = lroundf(Pocasi.teplota * 10.0f);
  if (teplota != val)
  {
    teplota = val;
    Disf("x9.val=%ld\xFF\xFF\xFF", val);
  }

  static long pocitova = 0;
  val = lroundf(Pocasi.teplota_pocit * 10.0f);
  if (pocitova != val)
  {
    pocitova = val;
    Disf("x10.val=%ld\xFF\xFF\xFF", val);
  }

  static long tep_min = 0;
  val = lroundf(Pocasi.teplota_min * 10.0f);
  if (tep_min != val)
  {
    tep_min = val;
    Disf("x11.val=%ld\xFF\xFF\xFF", val);
  }

  static long tep_max = 0;
  val = lroundf(Pocasi.teplota_max * 10.0f);
  if (tep_max != val)
  {
    tep_max = val;
    Disf("x12.val=%ld\xFF\xFF\xFF", val);
  }

  id = SensCheckId("A01"); // --- Baterie hlavni ---
  if (id >= 0 && Sensor[id].valid)
  {
    static int8_t bat1 = 0;
    int8_t v = Sensor[id].value;
    if (bat1 != v)
    {
      bat1 = v;
      Disf("j2.val=%d\xFF\xFF\xFF", bat1); // Hlavni
    }
  }
  else
    Disf("j2.val=%d\xFF\xFF\xFF", 0);

  id = SensCheckId("A11"); // --- Baterie venkovni ---
  if (id >= 0 && Sensor[id].valid)
  {
    static int8_t bat3 = 0;
    int8_t v = Sensor[id].value;
    if (bat3 != v)
    {
      bat3 = v;
      Disf("j4.val=%d\xFF\xFF\xFF", bat3);
    }
  }
  else
    Disf("j4.val=%d\xFF\xFF\xFF", 0);

  id = SensCheckId("A21"); // --- Baterie vnitrni sensor ---
  if (id >= 0 && Sensor[id].valid)
  {
    static int8_t bat2 = 0;
    int8_t v = Sensor[id].value;
    if (bat2 != v)
    {
      bat2 = v;
      Disf("j3.val=%d\xFF\xFF\xFF", bat2);
    }
  }
  else
    Disf("j3.val=%d\xFF\xFF\xFF", 0);

  // --------------------------------------------------------------------------
  initl = false;
  LedRGB();
}
